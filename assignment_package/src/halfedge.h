#ifndef HALFEDGE_H
#define HALFEDGE_H

#include <glm/glm.hpp>
#include "menuitem.h"
#include "vertex.h"
#include "face.h"

#ifndef HALFEDGE_IDX
#define HALFEDGE_IDX
#define SELF_HALFEDGE_IDX       0
#define SYMMETRIC_HALFEDGE_IDX  1
#define NEXT_HALFEDGE_IDX       2
#endif

struct CommonResult
{
    bool common;
    int ia;
    int ib;
};

class HalfEdge : public MenuItem
{
public:
    HalfEdge();
    ~HalfEdge();

public:
    inline void SetNext(HalfEdge* edge)
    {
        m_Edges[NEXT_HALFEDGE_IDX] = edge;
    }
    inline HalfEdge* GetNext() const { return m_Edges[NEXT_HALFEDGE_IDX]; }

    inline void SetSymmetric(HalfEdge* edge)
    {
        m_Edges[SYMMETRIC_HALFEDGE_IDX] = edge;
        edge->m_Edges[SYMMETRIC_HALFEDGE_IDX] = this;
    }
    inline HalfEdge* GetSymmetric() const { return m_Edges[SYMMETRIC_HALFEDGE_IDX]; }

    inline void SetVertex(Vertex* v)
    {
        v->SetHalfEdge(this);
        m_Vertex = v;
    }
    inline Vertex* GetVertex() const { return m_Vertex; }
    inline void SetFace(Face* f)
    {
        f->SetHalfEdge(this);
        m_Face = f;
    }
    inline Face* GetFace() { return m_Face; }

    inline void SetNormal(const glm::vec3& normal) { m_Normal = normal; }
    inline glm::vec3& GetNormal() { return m_Normal; }
    inline void SetUV(const glm::vec2& uv) { m_UV = uv; }
    inline glm::vec2& GetUV() { return m_UV; }

    inline glm::vec3 GetDirection() const { return (GetVertex()->GetPosition()) - (GetSymmetric()->GetVertex()->GetPosition()); }
protected:
    std::array<HalfEdge*, 3> m_Edges;
    Vertex* m_Vertex;
    Face* m_Face;

    glm::vec3 m_Normal;
    glm::vec2 m_UV;

public:
    static void ChangeCount(const int& value);

    static CommonResult CommonFace(const HalfEdge& a, const HalfEdge& b);
    static CommonResult CommonVertex(const HalfEdge& a, const HalfEdge& b);

    // try to find the edge loop form by this edge and the given halfEdge
    static bool TryFindEdgeLoop(HalfEdge* a,
                                HalfEdge* b,
                                std::vector<HalfEdge*>& edgeLoop,
                                std::vector<unsigned int>& pattern);
public:
    static unsigned int count;
};

#endif // HALFEDGE_H
