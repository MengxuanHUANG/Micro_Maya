#ifndef FACE_H
#define FACE_H

#include <glm/glm.hpp>
#include "menuitem.h"

class HalfEdge;

class Face : public MenuItem
{
public:
    Face();
    Face(const glm::vec3& color);
    ~Face();

    glm::vec3 GetNormal() const;
    glm::vec3 GetBarycenter() const;
public:
    inline glm::vec3& GetColor() { return m_Color; }
    inline void SetColor(const glm::vec3& color) { m_Color = color; }

    inline HalfEdge* GetHalfEdge() const { return m_HalfEdge; }
    inline void SetHalfEdge(HalfEdge* halfEdge) { m_HalfEdge = halfEdge; }

protected:
    glm::vec3 m_Color;
    HalfEdge* m_HalfEdge;

public:
    static void ChangeCount(const int& value);
public:
    static unsigned int count;
};

#endif // FACE_H
