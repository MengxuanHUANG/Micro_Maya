#ifndef MODE_H
#define MODE_H

#include "qevent.h"

class MyGL;

class Mode : public QObject
{
    Q_OBJECT
public:
    Mode(MyGL* mygl, Mode* parent)
        :m_MyGL(mygl), m_Parent(parent)
    {}

    virtual void Load() = 0;
    virtual void UnLoad() = 0;

    virtual void ClearSelection() {}

    virtual void Init() {}

    virtual void Tick(const float&) {}
    virtual void Update() {}

    // define how to draw
    virtual void Draw() {}
    virtual void DrawOnTop() {}

    // Input events
    virtual void keyPressEvent(QKeyEvent *e) {}
    virtual void MousePressEvent(QMouseEvent *event) {}
    virtual void MouseReleaseEvent(QMouseEvent *event) {}
    virtual void MouseDoubleClickEvent(QMouseEvent *event) {}

protected:
    Mode* m_Parent;
    MyGL* m_MyGL;
};

#endif // MODE_H
