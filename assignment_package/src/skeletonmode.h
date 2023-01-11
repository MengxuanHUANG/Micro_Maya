#ifndef SKELETONMODE_H
#define SKELETONMODE_H

#include "mode.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include <skeletalmeshshaderprogram.h>
#include "scene/jointdisplay.h"

class Camera;
class GUIVec3Editor;
class VertexJointWeightEditor;
class QTreeWidget;
class QPushButton;
class QTabWidget;
class SkeletalMesh;

class SkeletonMode : public Mode
{
    Q_OBJECT
public:
    SkeletonMode(MyGL* mygl, Mode* parent);

    virtual void Load() override;
    virtual void UnLoad() override;
    virtual void ClearSelection() override;

    virtual void Init() override;
    virtual void Update() override;

    // define how to draw
    virtual void Draw() override;
    virtual void DrawOnTop() override;

    // Input events
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void MousePressEvent(QMouseEvent *event) override;
    virtual void MouseDoubleClickEvent(QMouseEvent *event) override;

public slots:
    void slot_SelectedJointChanged();

    void slot_UnBindSkeletalMesh();
    void slot_EidtModeChanged(int);

public:
    SkeletalMesh* m_SkeletalMesh;
    Skeleton* m_Skeleton;
    Camera* m_Camera;
    std::unique_ptr<JointDisplay> m_JointDisplay;

    // UI
    GUIVec3Editor* m_JointPosEditor;
    GUIVec3Editor* m_JointRotEditor;
    VertexJointWeightEditor* m_VertexJointWeightEditor;

    QTabWidget* m_ModeTab;
    QTabWidget* m_EditModeTab;
    QTreeWidget* m_JointTree;

    QPushButton* BN_UnBind;

    ShaderProgram* m_progFlat;
    std::unique_ptr<SkeletalMeshShaderProgram> m_progSkeletalMesh;

protected:
    Mode* currentEditMode;
    std::vector<std::unique_ptr<Mode>> m_EditModes;
};

#endif // SKELETONMODE_H
