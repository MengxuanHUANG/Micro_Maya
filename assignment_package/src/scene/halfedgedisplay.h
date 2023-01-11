#ifndef HALFEDGEDISPLAY_H
#define HALFEDGEDISPLAY_H

#include <unordered_set>
#include <vector>
#include <drawable.h>

class HalfEdge;

struct EdgeLoop
{
    std::vector<HalfEdge*> m_EdgeLoop;
    std::vector<unsigned int> m_Pattern;

    inline void clear()
    {
        m_EdgeLoop.clear();
        m_Pattern.clear();
    }
};

class HalfEdgeDisplay : public Drawable
{
public:
    HalfEdgeDisplay(OpenGLContext* context);

    void SelectHalfEdge(HalfEdge* e);
    void ClearSelection();

    inline bool IsSelected(HalfEdge* e) { return m_SelectedHalfEdges.find(e) != m_SelectedHalfEdges.end(); }

    inline std::unordered_set<HalfEdge*>& GetSelectedHalfEdges() { return m_SelectedHalfEdges; }
    inline HalfEdge* GetCurrentSelectedHalfEdge() const { return m_CurrentSelectedHalfEdge; }
    inline HalfEdge* GetPreviousSelectedHalfEdge() const { return m_PreviousSelectedHalfEdge; }

    inline EdgeLoop& GetEdgeLoop() { return m_EdgeLoop; }
public:
    virtual void create() override;
    virtual GLenum drawMode() override;

protected:
    HalfEdge* m_CurrentSelectedHalfEdge;
    HalfEdge* m_PreviousSelectedHalfEdge;
    std::unordered_set<HalfEdge*> m_SelectedHalfEdges;

    EdgeLoop m_EdgeLoop;
};

#endif // HALFEDGEDISPLAY_H
