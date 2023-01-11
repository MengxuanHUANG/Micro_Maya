#ifndef JOINTDISPLAY_H
#define JOINTDISPLAY_H

#include <drawable.h>

class Joint;

class JointDisplay : public Drawable
{
public:
    JointDisplay(OpenGLContext* context);

    virtual void create() override;
    virtual GLenum drawMode() override;
    inline void ClearSelection()
    {
        m_SelectedJoint = nullptr;
        count = 0;
    }
    inline void SelectJoint(Joint* joint) { m_SelectedJoint = joint; }
    inline Joint* GetSelectedJoint() { return m_SelectedJoint; }
protected:
    Joint* m_SelectedJoint;
};

#endif // JOINTDISPLAY_H
