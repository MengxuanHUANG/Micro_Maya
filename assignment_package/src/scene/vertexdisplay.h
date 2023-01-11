#ifndef VERTEXDISPLAY_H
#define VERTEXDISPLAY_H

#include <drawable.h>
#include <unordered_set>

class Vertex;

class VertexDisplay : public Drawable
{
public:
    VertexDisplay(OpenGLContext* context);

    void SelectVertex(Vertex* v);

    void ClearSelection();

    inline bool IsSelected(Vertex* v) { return m_SelectedVertices.find(v) != m_SelectedVertices.end(); }

    inline std::unordered_set<Vertex*>& GetSelectedVertices() { return m_SelectedVertices; }
    inline Vertex* GetCurrentSelectedVertex() const { return m_CurrentSelectedVertex; }
public:
    virtual void create() override;
    virtual GLenum drawMode() override;

protected:
    Vertex* m_CurrentSelectedVertex;
    std::unordered_set<Vertex*> m_SelectedVertices;
};

#endif // VERTEXDISPLAY_H
