#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <optional>
#include <glm/glm.hpp>

class QListWidgetItem;
class Mesh;
class Vertex;
class HalfEdge;
class Face;
class Camera;

enum class ElementType{
    NONE,
    VERTEX,
    HALFEDGE,
    FACE
};

struct RayCastResult{
    ElementType type;
    QListWidgetItem* item;
    float t;
    glm::vec3 position;
};

namespace RayCaster
{
    std::optional<RayCastResult> CastRayToMesh(glm::vec3 startPoint,
                                         glm::vec3 rayDir,
                                         Camera* camera,
                                         Mesh* mesh);

    std::optional<RayCastResult> RVIntersection(glm::vec3& startPoint,
                                                glm::vec3& rayDir,
                                                glm::vec3& eye,
                                                glm::vec3& look,
                                                Vertex* v,
                                                const float& vertexRadius);

    std::optional<RayCastResult> REIntersection(glm::vec3& startPoint,
                                                glm::vec3& rayDir,
                                                glm::vec3& eye,
                                                glm::vec3& look,
                                                HalfEdge* e,
                                                const float& edgeRadius);

    std::optional<RayCastResult> RFIntersection(glm::vec3& startPoint,
                                                glm::vec3& rayDir,
                                                Face* f);

    float HitSphere(glm::vec3& startPoint,
                   glm::vec3& rayDir,
                   const glm::vec3& center,
                   const float& radius);
};

#endif // RAYCASTER_H
