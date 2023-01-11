#ifndef JOINTWEIGHTEDITMODE_H
#define JOINTWEIGHTEDITMODE_H

#include "mode.h"
#include "scene/vertexdisplay.h"

class SkeletalMesh;
class VertexDisplay;
class Camera;
class ShaderProgram;
class VertexJointWeightEditor;

class JointWeightEditMode : public Mode
{
    Q_OBJECT
public:
    JointWeightEditMode(MyGL* mygl, Mode* parent);

    virtual void Load() override;
    virtual void UnLoad() override;

    virtual void ClearSelection() override;

    virtual void Init() override;
    virtual void DrawOnTop() override;

    virtual void MousePressEvent(QMouseEvent *event) override;

protected:
    void VertexSelectionChanged(Vertex* v);
    Vertex* SelectAt(const unsigned int&, const unsigned int&);
private slots:
    void slot_JointChanged(const QString& preJoint, const QString& currentJoint);
    void slot_WeightChanged(const QString& currentJoint, double weight);

public:
    Camera* m_Camera;

    std::unique_ptr<VertexDisplay> m_VertexDisplay;
    SkeletalMesh* m_SkeletalMesh;

    ShaderProgram* m_progFlat;

    VertexJointWeightEditor* m_VertexJointWeightEditor;
};

#endif // JOINTWEIGHTEDITMODE_H
