#ifndef JOINTEDITMODE_H
#define JOINTEDITMODE_H

#include "mode.h"
#include <glm/glm.hpp>

class Camera;
class JointDisplay;
class Joint;
class Skeleton;
class GUIVec3Editor;
class QTreeWidget;

class JointEditMode : public Mode
{
    Q_OBJECT
public:
    JointEditMode(MyGL* mygl, Mode* parent);

    virtual void Load() override;
    virtual void UnLoad() override;

    virtual void MousePressEvent(QMouseEvent *event) override;

public slots:
    void slot_JointPosChanged(const glm::vec3&);
    void slot_JointRotChanged(const glm::vec3&);

protected:
    Joint* SelectAt(const unsigned int&, const unsigned int&);

public:
    Camera* m_Camera;
    Skeleton* m_Skeleton;
    JointDisplay* m_JointDisplay;

    // UI
    GUIVec3Editor* m_JointPosEditor;
    GUIVec3Editor* m_JointRotEditor;

    QTreeWidget* m_JointTree;
};

#endif // JOINTEDITMODE_H
