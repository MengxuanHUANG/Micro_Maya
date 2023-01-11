#include "vertex.h"
#include "halfedge.h"

unsigned int Vertex::count = 0;

unsigned int Vertex::GetHalfEdgeCount(Vertex* v)
{
    unsigned int count = 0;
    HalfEdge* e = v->GetHalfEdge();
    do
    {
        ++count;
        e = e->GetNext()->GetSymmetric();
    }
    while(e != v->GetHalfEdge());

    return count;
}

Vertex::Vertex(const glm::vec3& position)
    :MenuItem(count++), m_Position(position)
{}

Vertex::~Vertex()
{
    --count;
}
