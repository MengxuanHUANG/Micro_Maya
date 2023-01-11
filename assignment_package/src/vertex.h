#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include "menuitem.h"

class HalfEdge;

class Vertex : public MenuItem
{
public:
    Vertex(const glm::vec3& position);
    ~Vertex();
public:
    inline glm::vec3& GetPosition() { return m_Position; }
    inline void SetPosition(const glm::vec3& position) { m_Position = position; }

    inline HalfEdge* GetHalfEdge() { return m_HalfEdge; }
    inline void SetHalfEdge(HalfEdge* halfEdge) { m_HalfEdge = halfEdge; }
protected:
    glm::vec3 m_Position;
    HalfEdge* m_HalfEdge;

public:
    // count the incoming half edges of a vertex
    static unsigned int GetHalfEdgeCount(Vertex* v);
public:
    static unsigned int count;
};

#endif // VERTEX_H
