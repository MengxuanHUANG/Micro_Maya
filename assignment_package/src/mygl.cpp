#include "mygl.h"
#include <la.h>

#include <QKeyEvent>
#include <ui_mainwindow.h>

#include "mesh.h"

#include "scene/wireframesphere.h"
#include "scene/skeleton.h"
#include "scene/joint.h"
#include "scene/skeletalmesh.h"

#include "meshmode.h"
#include "skeletonmode.h"

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_Mesh(std::make_unique<Mesh>(this)),
      m_Skeleton(std::make_unique<Skeleton>(this)),
      m_SkeletalMesh(std::make_unique<SkeletalMesh>(this)),
      m_WireFrameSphere(std::make_unique<WireframeSphere>(this)),
      m_geomSquare(this),
      m_progLambert(this),
      m_progFlat(this),
      m_glCamera()
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    //m_geomSquare.create();

    std::unique_ptr<Mode> mode_0 = std::make_unique<MeshMode>(this, nullptr);
    MeshMode* meshMode = reinterpret_cast<MeshMode*>(mode_0.get());

    meshMode->m_Camera = &m_glCamera;
    meshMode->m_Mesh = m_Mesh.get();
    meshMode->m_progLambert = &m_progLambert;
    meshMode->m_progFlat = &m_progFlat;

    meshMode->m_VertexList = ui->vertsListWidget;
    meshMode->m_HalfEdgeList = ui->halfEdgesListWidget;
    meshMode->m_FaceList = ui->facesListWidget;
    meshMode->m_VertexPosEditor = m_VertPosEditor;
    meshMode->m_FaceColEditor = m_FaceColorEditor;
    meshMode->BN_AddVertex = ui->BN_AddVertex;
    meshMode->BN_TriangulateFace = ui->BN_TriangulateFace;
    meshMode->BN_ExtrudeFace = ui->BN_ExtrudeFace;
    meshMode->BN_Bevel = ui->BN_Bevel;
    meshMode->BN_InsertEdgeLoop = ui->BN_InsertEdgeLoop;
    meshMode->BN_Subdivision = ui->BN_Subdivision;
    meshMode->BN_MarkSharp = ui->BN_MarkSharp;

    std::unique_ptr<Mode> mode_1 = std::make_unique<SkeletonMode>(this, nullptr);
    SkeletonMode* skeletonMode = reinterpret_cast<SkeletonMode*>(mode_1.get());

    skeletonMode->m_SkeletalMesh = m_SkeletalMesh.get();
    skeletonMode->m_Skeleton = m_Skeleton.get();
    skeletonMode->m_Camera = &m_glCamera;
    skeletonMode->m_progFlat = &m_progFlat;

    skeletonMode->m_JointPosEditor = m_JointLocationEditor;
    skeletonMode->m_JointRotEditor = m_JointOrientationEditor;
    skeletonMode->m_VertexJointWeightEditor = m_VertexJointWeightEditor;
    skeletonMode->m_EditModeTab = ui->SkeletonEditTab;
    skeletonMode->m_ModeTab = ui->ModeTabWidget;
    skeletonMode->m_JointTree = ui->SkeletonTree;
    skeletonMode->BN_UnBind = ui->BN_UnBind;

    mode_0->Init();
    mode_1->Init();

    m_Modes.emplace_back(std::move(mode_0));
    m_Modes.emplace_back(std::move(mode_1));

    m_CurrentMode = meshMode;
    m_CurrentMode->Load();
    //Create the instances of Mesh.
    m_Mesh->Init();
    m_Skeleton->Init();
    m_SkeletalMesh->Init();
    m_Mesh->SetModelMatrix(glm::mat4(1.0f));
    m_WireFrameSphere->create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_CurrentMode->Draw();

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    glDisable(GL_DEPTH_TEST);
    if(m_Skeleton->elemCount() > 0)
    {
        m_progFlat.setModelMatrix(glm::mat4(1));
        m_progFlat.draw(*static_cast<Drawable*>(m_Skeleton.get()));

        for(Joint* j : m_Skeleton->GetJoints())
        {
            m_progFlat.setModelMatrix(glm::transpose(j->getOverallTransformation().GetTransformMat4()));
            m_progFlat.draw(*static_cast<Drawable*>(m_WireFrameSphere.get()));
        }
    }

    m_CurrentMode->DrawOnTop();

    glEnable(GL_DEPTH_TEST);

}

void MyGL::ChangeMode(int id)
{
    m_CurrentMode->UnLoad();
    m_CurrentMode = m_Modes[id].get();
    m_CurrentMode->Load();

    update();
}

void MyGL::UpdateMesh(const QString& path)
{
    sig_SetListSelection(nullptr,
                         QItemSelectionModel::Clear,
                         [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
    sig_SetListSelection(nullptr,
                         QItemSelectionModel::Clear,
                         [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
    sig_SetListSelection(nullptr,
                         QItemSelectionModel::Clear,
                         [](Ui::MainWindow* ui) { return ui->facesListWidget;});

    // remove previous object and load the new object

    m_CurrentMode->ClearSelection();

    m_SkeletalMesh->UnBind();
    m_Mesh->ReleaseOBJ();
    m_Mesh->LoadOBJ(path);

    // add vertices, halfEdges, and faces to lists
    sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Vertices[0]),
                    m_Mesh->m_Vertices.size(),
                    [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
    sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_HalfEdges[0]),
                    m_Mesh->m_HalfEdges.size(),
                    [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});

    sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Faces[0]),
                    m_Mesh->m_Faces.size(),
                    [](Ui::MainWindow* ui) { return ui->facesListWidget;});

    m_Mesh->create();
    update();
}

void MyGL::LoadSkeleton(const QString& path)
{
    m_SkeletalMesh->UnBind();
    m_CurrentMode->ClearSelection();

    Skeleton::LoadSkeleton(m_Skeleton.get(), path);
    sig_SendSkeletonTreeRoot(m_Skeleton->GetRootJoint());
    m_Skeleton->create();

    update();
}

// given a position in screen space, get its world position
glm::vec3 MyGL::GetPxielInWorld(const float& x, const float& y, const float& z) const
{
    //from screen space to NDC
    float sx = (2.0f * x / this->width()) - 1.0f;
    float sy = 1.f - (2.0f * y / this->height());
    return this->GetNDCPositionInWorld(sx, sy, z);
}

glm::vec3 MyGL::GetNDCPositionInWorld(const float& x, const float& y, const float& t) const
{
    glm::vec3 ref = m_glCamera.eye + t * m_glCamera.look;
    float len = glm::length(t * m_glCamera.look);

    float alpha = glm::radians(m_glCamera.fovy / 2.f);

    glm::vec3 V = len * glm::tan(alpha) * m_glCamera.up;
    glm::vec3 H = len * glm::tan(alpha) * m_glCamera.aspect * m_glCamera.right;

    return ref + x * H + y * V;
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAroundCenter(0, -amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAroundCenter(0, amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAroundCenter(amount, 0);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAroundCenter(-amount, 0);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    }
    m_CurrentMode->keyPressEvent(e);
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::mousePressEvent(QMouseEvent *event)
{
    m_CurrentMode->MousePressEvent(event);
}

void MyGL::mouseReleaseEvent(QMouseEvent *event)
{
    m_CurrentMode->MouseReleaseEvent(event);
}

void MyGL::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_CurrentMode->MouseDoubleClickEvent(event);
}

void MyGL::slot_ModeChanged(int modeId)
{
    ChangeMode(modeId);
}

void MyGL::slot_Skinning()
{
    if(m_Mesh->m_Vertices.size() > 0 && m_Skeleton->GetJointsCount() > 0)
    {
        m_SkeletalMesh->Bind(m_Mesh.get(), m_Skeleton.get());
        ui->ModeTabWidget->setCurrentIndex(SKELETON_MODE);
    }
}
