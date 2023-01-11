#include "halfedge.h"
#include "vertex.h"
#include "face.h"

unsigned int HalfEdge::count = 0;

CommonResult HalfEdge::CommonFace(const HalfEdge& a, const HalfEdge& b)
{
    for(int i = 0; i < 2; ++i)
    {
        for(int j = 0; j < 2; ++j)
        {
            if(a.m_Edges[i]->m_Face == b.m_Edges[j]->m_Face)
            {
                return {true, i, j};
            }
        }
    }
    return {false, -1, -1};
}

CommonResult HalfEdge::CommonVertex(const HalfEdge& a, const HalfEdge& b)
{
    for(int i = 0; i < 2; ++i)
    {
        for(int j = 0; j < 2; ++j)
        {
            if(a.m_Edges[i]->m_Vertex == b.m_Edges[j]->m_Vertex)
            {
                return {true, i, j};
            }
        }
    }
    return {false, -1, -1};
}

bool HalfEdge::TryFindEdgeLoop(HalfEdge* a,
                               HalfEdge* b,
                               std::vector<HalfEdge*>& edgeLoop,
                               std::vector<unsigned int>& pattern)
{
    unsigned int inHalfEdgeCount = 0;
    // check if two edges shares the same vertices
    CommonResult vertexResult = CommonVertex(*a, *b);
    CommonResult faceResult = CommonFace(*a, *b);
    if(vertexResult.common)
    {
        a = a->m_Edges[vertexResult.ia];
        b = b->m_Edges[vertexResult.ib]->GetSymmetric();

        inHalfEdgeCount = Vertex::GetHalfEdgeCount(a->GetVertex());

        pattern.emplace_back(NEXT_HALFEDGE_IDX);
        // find the pattern from half-edge a to half-edge b
        HalfEdge* e = a->GetNext();
        while(e != b)
        {
            e = e->GetSymmetric();
            e = e->GetNext();

            pattern.emplace_back(SYMMETRIC_HALFEDGE_IDX);
            pattern.emplace_back(NEXT_HALFEDGE_IDX);
        }

        // find edgeLoop
        e = a;
        do
        {
            edgeLoop.emplace_back(e);
            for(int i: pattern)
            {
                e = e->m_Edges[i];
            }
        }
        while(e != a && Vertex::GetHalfEdgeCount(e->GetVertex()) == inHalfEdgeCount);
        pattern.clear();
        return e == a;
    }
    else if(faceResult.common)
    {
        a = a->m_Edges[faceResult.ia];
        b = b->m_Edges[faceResult.ib];

        HalfEdge* e = a;
        while(e != b)
        {
            e = e->GetNext();
            pattern.emplace_back(NEXT_HALFEDGE_IDX);
        }
        pattern.emplace_back(SYMMETRIC_HALFEDGE_IDX);

        // find edgeLoop
        e = a;
        do
        {
            edgeLoop.emplace_back(e);
            for(int i: pattern)
            {
                e = e->m_Edges[i];
            }
        }
        while(e != a);
        return e == a;
    }

    return false;
}

HalfEdge::HalfEdge()
    :MenuItem(count++), m_Edges({this, nullptr, nullptr})
{}

HalfEdge::~HalfEdge()
{
    --count;
}
