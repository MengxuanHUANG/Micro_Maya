#include "face.h"
#include "halfedge.h"

#include "glm/gtx/quaternion.hpp"

unsigned int Face::count = 0;

Face::Face()
    :MenuItem(count++)
{}

Face::Face(const glm::vec3& color)
    :MenuItem(count++), m_Color(color)
{}

Face::~Face()
{
    --count;
}

glm::vec3 Face::GetNormal() const
{
    glm::vec3 normal(0);
    HalfEdge* e = this->GetHalfEdge();
    do
    {
        // calculate normal
        const glm::vec3& pre_pos = e->GetSymmetric()->GetVertex()->GetPosition();
        const glm::vec3& cur_pos = e->GetVertex()->GetPosition();
        const glm::vec3& next_pos = e->GetNext()->GetVertex()->GetPosition();

        glm::vec3 result = glm::cross(cur_pos - pre_pos, next_pos - cur_pos);
        normal += (glm::length2(result) > 0.f ? glm::normalize(result) : result);

        e = e->GetNext();
    }
    while(e != this->GetHalfEdge());

    return glm::normalize(normal);
}

glm::vec3 Face::GetBarycenter() const
{
    glm::vec3 barycenter(0);
    float area_sum = 0;
    glm::vec3& position = this->GetHalfEdge()->GetVertex()->GetPosition();
    HalfEdge* e = this->GetHalfEdge()->GetNext();
    do
    {
        const glm::vec3& cur_pos = e->GetVertex()->GetPosition();
        const glm::vec3& next_pos = e->GetNext()->GetVertex()->GetPosition();
        // barycenter of this triangle
        glm::vec3 sub_barycenter = (position + cur_pos + next_pos) / 3.0f;
        // area of this triangle
        float area = glm::length(glm::cross(cur_pos - position, next_pos - cur_pos));

        area_sum += area;
        barycenter += area * sub_barycenter;
        e = e->GetNext();
    }
    while(e->GetNext() != this->GetHalfEdge());

    return barycenter / area_sum;
}
