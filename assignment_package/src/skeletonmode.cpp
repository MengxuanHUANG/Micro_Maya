#include "skeletonmode.h"
#include "mygl.h"

#include "guivec3editor.h"

#include "jointeditmode.h"
#include "jointweighteditmode.h"

#include <QTreeWidget>
#include <QPushButton>
#include <QTabWidget>
#include <numeric>

#include "scene/joint.h"
#include "scene/skeleton.h"

SkeletonMode::SkeletonMode(MyGL* mygl, Mode* parent)
    :Mode(mygl, parent),
     m_JointDisplay(std::make_unique<JointDisplay>(mygl)),
     m_progSkeletalMesh(std::make_unique<SkeletalMeshShaderProgram>(mygl))
{}

void SkeletonMode::Load()
{
    currentEditMode->Load();

    connect(m_JointTree,
            SIGNAL(itemSelectionChanged()),
            this,
            SLOT(slot_SelectedJointChanged()));

    connect(BN_UnBind,
            SIGNAL(clicked()),
            this,
            SLOT(slot_UnBindSkeletalMesh()));

    connect(m_EditModeTab,
            SIGNAL(currentChanged(int)),
            this,
            SLOT(slot_EidtModeChanged(int)));

    Update();
}

void SkeletonMode::UnLoad()
{
    currentEditMode->UnLoad();

    ClearSelection();
    disconnect(m_JointTree,
                SIGNAL(itemSelectionChanged()),
                this,
                SLOT(slot_SelectedJointChanged()));

    disconnect(BN_UnBind,
                SIGNAL(clicked()),
                this,
                SLOT(slot_UnBindSkeletalMesh()));
    disconnect(m_EditModeTab,
            SIGNAL(currentChanged(int)),
            this,
            SLOT(slot_EidtModeChanged(int)));
}

void SkeletonMode::ClearSelection()
{
    m_JointTree->clearSelection();
}

void SkeletonMode::Init()
{    
    std::unique_ptr<JointWeightEditMode> jointWeightMode = std::make_unique<JointWeightEditMode>(m_MyGL, this);
    jointWeightMode->m_Camera = m_Camera;
    jointWeightMode->m_SkeletalMesh = m_SkeletalMesh;
    jointWeightMode->m_progFlat = m_progFlat;
    jointWeightMode->m_VertexJointWeightEditor = m_VertexJointWeightEditor;
    jointWeightMode->Init();

    std::unique_ptr<JointEditMode> jointEditMode = std::make_unique<JointEditMode>(m_MyGL, this);
    jointEditMode->m_Camera = m_Camera;
    jointEditMode->m_Skeleton = m_Skeleton;
    jointEditMode->m_JointDisplay = m_JointDisplay.get();
    jointEditMode->m_JointPosEditor = m_JointPosEditor;
    jointEditMode->m_JointRotEditor = m_JointRotEditor;
    jointEditMode->m_JointTree = m_JointTree;

    currentEditMode = jointEditMode.get();

    m_EditModes.emplace_back(std::move(jointEditMode));
    m_EditModes.emplace_back(std::move(jointWeightMode));

    m_progSkeletalMesh->create(":/glsl/skeletalMesh.vert.glsl", ":/glsl/skeletalMesh.frag.glsl");
    m_JointDisplay->Init();
}

void SkeletonMode::Update()
{
    m_Skeleton->create();
    if(m_SkeletalMesh->IsBinded())
    {
        m_SkeletalMesh->CreateWithSelectedJoint(m_JointDisplay->GetSelectedJoint());

        m_progSkeletalMesh->SetJointBind(m_SkeletalMesh->GetSkeleton());
        m_progSkeletalMesh->SetJointOverall(m_SkeletalMesh->GetSkeleton());
    }
    m_JointDisplay->create();
    m_MyGL->update();
}

void SkeletonMode::Draw()
{
    m_progSkeletalMesh->setViewProjMatrix(m_Camera->getViewProj());
    m_progSkeletalMesh->setCamPos(m_Camera->eye);

    m_progSkeletalMesh->setModelMatrix(glm::mat4(1));
    m_progSkeletalMesh->draw(*m_SkeletalMesh);
}

void SkeletonMode::DrawOnTop()
{
    if(m_JointDisplay->GetSelectedJoint() != nullptr)
    {
        m_progFlat->setViewProjMatrix(m_Camera->getViewProj());
        glm::mat4 M = m_JointDisplay->GetSelectedJoint()->getOverallTransformation().GetTransformMat4();
        m_progFlat->setModelMatrix(glm::transpose(M));
        m_progFlat->draw(*static_cast<Drawable*>(m_JointDisplay.get()));
    }
    currentEditMode->DrawOnTop();
}

void SkeletonMode::keyPressEvent(QKeyEvent *e)
{

}

void SkeletonMode::MousePressEvent(QMouseEvent *event)
{
    currentEditMode->MousePressEvent(event);
}

void SkeletonMode::MouseDoubleClickEvent(QMouseEvent *event)
{

}

void SkeletonMode::slot_SelectedJointChanged()
{
    m_JointDisplay->ClearSelection();
    for(QTreeWidgetItem* item : m_JointTree->selectedItems())
    {
        m_JointDisplay->SelectJoint(reinterpret_cast<Joint*>(item));
    }
    if(m_JointTree->selectedItems().size() > 0)
    {
        m_JointPosEditor->SetEnabled(true);
        m_JointPosEditor->SetValue(m_JointDisplay->GetSelectedJoint()->GetLocalTranslation(), true);

        m_JointRotEditor->SetEnabled(true);
        m_JointRotEditor->SetValue(glm::vec3(0), true);

        Update();
    }
    else
    {
        m_JointPosEditor->SetEnabled(false);
        m_JointPosEditor->SetValue(glm::vec3(0), true);

        m_JointRotEditor->SetEnabled(false);
        m_JointRotEditor->SetValue(glm::vec3(0), true);

        Update();
    }
}

void SkeletonMode::slot_UnBindSkeletalMesh()
{
    m_SkeletalMesh->UnBind();
    m_ModeTab->setCurrentIndex(MESH_MODE);
}

void SkeletonMode::slot_EidtModeChanged(int id)
{
    currentEditMode->UnLoad();
    currentEditMode = m_EditModes[id].get();
    currentEditMode->Load();
    m_MyGL->update();
}
