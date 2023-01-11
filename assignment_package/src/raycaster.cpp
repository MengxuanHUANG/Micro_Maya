#include "raycaster.h"

#include "mesh.h"
#include "halfedge.h"
#include "camera.h"

#include <limits>

namespace RayCaster
{
    std::optional<RayCastResult> CastRayToMesh(glm::vec3 startPoint,
                                         glm::vec3 rayDir,
                                         Camera* camera,
                                         Mesh* mesh)
    {
        std::optional<RayCastResult> result;

        float vertexRadius = 0.08f;
        float edgeRadius = 0.05f;
        float t = std::numeric_limits<float>::max();
        glm::mat4 inverseModel = glm::inverse(mesh->GetModelMatrix());

        startPoint = glm::vec3(glm::vec4(startPoint, 1.0) * inverseModel);
        rayDir = glm::vec3(glm::vec4(rayDir, 0.0) * inverseModel);
        glm::vec3 eye = glm::vec3(glm::vec4(camera->eye, 1.0) * inverseModel);
        glm::vec3 look = glm::vec3(glm::vec4(camera->look, 0.0) * inverseModel);

        // check every face
        for(std::unique_ptr<Face>& f : mesh->m_Faces)
        {
            glm::vec3& normal = f->GetHalfEdge()->GetNormal();
            float dotResult = glm::dot(rayDir, normal);

            std::optional<RayCastResult> fi = RFIntersection(startPoint, rayDir, f.get());
            if(fi.has_value() && fi->t < t)
            {
                t = fi->t;
                result = std::move(fi);
            }
            HalfEdge* e = f->GetHalfEdge();
            do
            {
                std::optional<RayCastResult> vi = RVIntersection(startPoint,
                                                                 rayDir,
                                                                 eye,
                                                                 look,
                                                                 e->GetVertex(),
                                                                 vertexRadius);
                if(vi.has_value() && vi->t < t)
                {
                    t = vi->t;
                    result = std::move(vi);
                }
                std::optional<RayCastResult> ei = REIntersection(startPoint,
                                                                 rayDir,
                                                                 eye,
                                                                 look,
                                                                 e,
                                                                 edgeRadius);
                if(ei.has_value() && ei->t < t)
                {
                    t = ei->t;
                    result = std::move(ei);
                }

                e = e->GetNext();
            }
            while(e != f->GetHalfEdge());
        }
        if(result.has_value())
        {
            result->position = glm::vec3(glm::vec4(result->position, 1.0) * mesh->GetModelMatrix());
        }
        return result;
    }

    std::optional<RayCastResult> RVIntersection(glm::vec3& startPoint,
                                                glm::vec3& rayDir,
                                                glm::vec3& eye,
                                                glm::vec3& look,
                                                Vertex* v,
                                                const float& vertexRadius)
    {
        std::optional<RayCastResult> result;

        // calculate radius based on camera

        // distance from camera to the plane perpendicular to camera's look dir
        float distance = glm::dot(look, v->GetPosition() - eye);
        float radius = vertexRadius * distance / 10.0;
        float t = HitSphere(startPoint, rayDir, v->GetPosition(), radius);
        if(t >= 0)
        {
            result = {
                ElementType::VERTEX,
                v,
                t,
                startPoint + t * rayDir
            };
        }

        return result;
    }

    std::optional<RayCastResult> REIntersection(glm::vec3& startPoint,
                                                glm::vec3& rayDir,
                                                glm::vec3& eye,
                                                glm::vec3& look,
                                                HalfEdge* e,
                                                const float& edgeRadius)
    {
        std::optional<RayCastResult> result;



        // calculate intersection for cylinder and the ray
        const glm::vec3& p1 = e->GetSymmetric()->GetVertex()->GetPosition();
        const glm::vec3& p2 = e->GetVertex()->GetPosition();

        // distance from camera to the plane perpendicular to camera's look dir
        float distance = glm::dot(look, p2 - eye);
        float radius = edgeRadius * distance / 10.0;

        float r2 = radius * radius;
        glm::vec3 edgeDir = glm::normalize(p2 - p1);

        // cylinder's axis
        glm::vec3& z_axis = edgeDir; // align to edge direction
        glm::vec3 x_axis = glm::normalize(glm::cross(edgeDir, rayDir));
        glm::vec3 y_axis = glm::normalize(glm::cross(z_axis, x_axis));

        // cylinder's frame
        glm::mat4 T{
            {1, 0, 0, -p1.x},
            {0, 1, 0, -p1.y},
            {0, 0, 1, -p1.z},
            {0, 0, 0, 1}
        };

        glm::mat4 R{
            {x_axis.x, x_axis.y, x_axis.z, 0},
            {y_axis.x, y_axis.y, y_axis.z, 0},
            {z_axis.x, z_axis.y, z_axis.z, 0},
            {0, 0, 0, 1.0}
        };

        glm::mat4 IT{
            {1, 0, 0, p1.x},
            {0, 1, 0, p1.y},
            {0, 0, 1, p1.z},
            {0, 0, 0, 1}
        };

        glm::vec3 oR = glm::vec3(glm::vec4(startPoint, 1.) * T * R); // start point in cylinder's frame of reference
        glm::vec3 dR = glm::vec3(glm::vec4(rayDir, 0.) * T * R); // direction in cylinder's frame of reference
        glm::vec3 v2 = glm::vec3(glm::vec4(p2, 1.) * T * R);

        float A = dR.x * dR.x + dR.y * dR.y;
        float B = 2 * (dR.x * oR.x + dR.y * oR.y);
        float C = oR.x * oR.x + oR.y * oR.y - r2;

        float delta = B * B - 4.f * A * C;
        if(delta >= 0)
        {
            float t1 = (-B + glm::sqrt(delta)) / 2.0 / A;
            float t2 = (-B - glm::sqrt(delta)) / 2.0 / A;
            float t = t2 > 0 ? t2 : t1;

            float z_max = v2.z;
            float z = oR.z + dR.z * t;

            if(z >= 0 && z <= z_max)
            {
                glm::vec3 pos = glm::vec3(glm::vec4(oR + dR * t, 1) * glm::transpose(R) * IT);
                float t = glm::length(pos - startPoint);
                result = {
                    ElementType::HALFEDGE,
                    e,
                    t,
                    pos
                };
            }
        }

        return result;
    }

    std::optional<RayCastResult> RFIntersection(glm::vec3& startPoint,
                                                glm::vec3& rayDir,
                                                Face* f)
    {
        std::optional<RayCastResult> result;

        HalfEdge* start_e = f->GetHalfEdge();
        HalfEdge* current_e = f->GetHalfEdge()->GetNext();
        while(current_e->GetNext() != start_e)
        {
            HalfEdge* next_e = current_e->GetNext();

            //calculate surface normal
            std::array<glm::vec3, 3> positions = {
                start_e->GetVertex()->GetPosition(),
                current_e->GetVertex()->GetPosition(),
                next_e->GetVertex()->GetPosition(),
            };

            std::array<glm::vec3, 3> vecs = {
                positions[1] - positions[0],
                positions[2] - positions[1],
                positions[0] - positions[2],
            };
            glm::vec3 normal = glm::normalize(glm::cross(vecs[0], vecs[1]));

            if(glm::dot(rayDir, normal) >= 0)
            {
                return result;
            }

            float t = glm::dot(normal, positions[0] - startPoint) / glm::dot(rayDir, normal);

            glm::vec3 position = startPoint + t * rayDir;

            int sign = 1;
            for(int i = 0; sign == 1 && i < 3; ++i)
            {
                glm::vec3 v = position - positions[i];
                sign = glm::sign(glm::dot(glm::cross(vecs[i], v), normal));
            }
            if(sign == 1)
            {
                return result = {
                    ElementType::FACE,
                    f,
                    t,
                    position
                };
            }
            current_e = next_e;
        }
        return result;
    }

    float HitSphere(glm::vec3& startPoint,
                   glm::vec3& rayDir,
                   const glm::vec3& center,
                   const float& radius)
    {
        // calculate intersection for sphere and the ray
        float A = glm::length2(rayDir);
        float B = 2 * (rayDir.x * (startPoint.x - center.x) +
                       rayDir.y * (startPoint.y - center.y) +
                       rayDir.z * (startPoint.z - center.z));

        float C = (startPoint.x - center.x) * (startPoint.x - center.x) +
                  (startPoint.y - center.y) * (startPoint.y - center.y) +
                  (startPoint.z - center.z) * (startPoint.z - center.z) - radius * radius;

        float delta = B * B - 4.f * A * C;
        if(delta >= 0)
        {
            float t1 = (-B + glm::sqrt(delta)) / 2.0 / A;
            float t2 = (-B - glm::sqrt(delta)) / 2.0 / A;
            return (t2 > 0 ? t2 : t1);
        }
        return -1;
    }
};


