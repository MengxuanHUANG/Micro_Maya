#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>

#include <scene/squareplane.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <memory>
#include <vector>
#include <QListWidget>

#include "mode.h"

#ifndef MODE
#define MODE
#define MESH_MODE 0
#define SKELETON_MODE 1
#endif

namespace Ui {
class MainWindow;
}
class Mesh;
class Joint;
class Skeleton;
class SkeletalMesh;

class WireframeSphere;
class QTreeWidgetItem;
class GUIVec3Editor;
class VertexJointWeightEditor;

class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<Skeleton> m_Skeleton;
    std::unique_ptr<SkeletalMesh> m_SkeletalMesh;
    std::unique_ptr<WireframeSphere> m_WireFrameSphere;

    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;

    Mode* m_CurrentMode;
    std::vector<std::unique_ptr<Mode> > m_Modes;

public:
    Ui::MainWindow* ui;
    GUIVec3Editor* m_VertPosEditor;
    GUIVec3Editor* m_FaceColorEditor;

    GUIVec3Editor* m_JointLocationEditor;
    GUIVec3Editor* m_JointOrientationEditor;
    VertexJointWeightEditor* m_VertexJointWeightEditor;

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void ChangeMode(int id);

    // load obj file from given path
    void UpdateMesh(const QString& path);

    // load skeleton from json file
    void LoadSkeleton(const QString& path);

    // given a position in screen space, get its world position
    glm::vec3 GetPxielInWorld(const float& x, const float& y, const float& z) const;

    // given a position in NDC space, get its world position
    glm::vec3 GetNDCPositionInWorld(const float& x, const float& y, const float& t) const;

    void JointSelectedChanged(const QList<QTreeWidgetItem*>&);
signals:
    void sig_AddListItems(QListWidgetItem**,
                        const unsigned int&,
                        const std::function<QListWidget*(Ui::MainWindow*)>&);

    void sig_SetListSelection(QListWidgetItem*,
                              QItemSelectionModel::SelectionFlags flag,
                              const std::function<QListWidget*(Ui::MainWindow*)>&);

    void sig_SendSkeletonTreeRoot(QTreeWidgetItem* root);
private slots:
    void slot_ModeChanged(int);
    void slot_Skinning();

protected:
    void UpdateDrawable();

    void keyPressEvent(QKeyEvent *e) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // MYGL_H
