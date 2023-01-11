#ifndef FACEDISPLAY_H
#define FACEDISPLAY_H

#include <drawable.h>
#include <unordered_set>

class Face;

class FaceDisplay : public Drawable
{
public:
    FaceDisplay(OpenGLContext* context);

    void SelectFace(Face* f);
    void ClearSelection();

    inline bool IsSelected(Face* f) { return m_SelectedFaces.find(f) != m_SelectedFaces.end(); }

    inline Face* GetCurrentSelectedFace() const { return m_CurrentSelectedFace; }
    inline std::unordered_set<Face*>& GetSelectedFaces() { return m_SelectedFaces; }

public:
    virtual void create() override;
    virtual GLenum drawMode() override;

protected:
    Face* m_CurrentSelectedFace;
    std::unordered_set<Face*> m_SelectedFaces;
};

#endif // FACEDISPLAY_H
