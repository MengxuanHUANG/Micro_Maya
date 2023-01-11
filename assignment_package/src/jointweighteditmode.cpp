#include "jointweighteditmode.h"

#include "scene/vertexdisplay.h"
#include "scene/skeletalmesh.h"
#include "scene/skeleton.h"
#include "mesh.h"

#include "mygl.h"
#include "vertex.h"

#include "vertexjointweighteditor.h"
#include "raycaster.h"

JointWeightEditMode::JointWeightEditMode(MyGL* mygl, Mode* parent)
    :Mode(mygl, parent),
     m_VertexDisplay(std::make_unique<VertexDisplay>(mygl))
{}

void JointWeightEditMode::Load()
{
    connect(m_VertexJointWeightEditor,
            SIGNAL(sig_JointChanged(const QString&, const QString&)),
            this,
            SLOT(slot_JointChanged(const QString&, const QString&)));

    connect(m_VertexJointWeightEditor,
            SIGNAL(sig_WeightChanged(const QString&, double)),
            this,
            SLOT(slot_WeightChanged(const QString&, double)));
}

void JointWeightEditMode::UnLoad()
{
    ClearSelection();
    disconnect(m_VertexJointWeightEditor,
               SIGNAL(sig_JointChanged(const QString&, const QString&)),
               this,
               SLOT(slot_JointChanged(const QString&, const QString&)));

    disconnect(m_VertexJointWeightEditor,
               SIGNAL(sig_WeightChanged(const QString&, double)),
               this,
               SLOT(slot_WeightChanged(const QString&, double)));
}

void JointWeightEditMode::Init()
{
    m_VertexDisplay->Init();
}

void JointWeightEditMode::DrawOnTop()
{
    if(m_SkeletalMesh->IsBinded())
    {
        m_progFlat->setViewProjMatrix(m_Camera->getViewProj());
        m_progFlat->setModelMatrix(glm::mat4(1.f));

        m_progFlat->draw(*static_cast<Drawable*>(m_VertexDisplay.get()));
    }
}

void JointWeightEditMode::ClearSelection()
{
    m_VertexDisplay->ClearSelection();
    m_VertexJointWeightEditor->SetDisabled(true);
    m_VertexJointWeightEditor->ClearValue();
}

void JointWeightEditMode::MousePressEvent(QMouseEvent *event)
{
    if(m_SkeletalMesh->IsBinded())
    {
        const QPoint& pos = event->pos();
        Vertex* selected = SelectAt(pos.x(), pos.y());
        VertexSelectionChanged(selected);
    }
}

Vertex* JointWeightEditMode::SelectAt(const unsigned int& x, const unsigned int& y)
{
    glm::vec3 posInWorld = m_MyGL->GetPxielInWorld(x, y, m_Camera->far_clip);
    glm::vec3 rayDir = glm::normalize(posInWorld - m_Camera->eye);

    glm::vec3& eye = m_Camera->eye;
    glm::vec3& look = m_Camera->look;

    Vertex* hitVertex = nullptr;
    float t = std::numeric_limits<float>::max();
    for(std::unique_ptr<Vertex>& vert : m_SkeletalMesh->GetMesh()->m_Vertices)
    {
        const glm::vec3& pos = vert->GetPosition();
        float distance = glm::dot(look, pos - eye);
        float radius = 0.08 * distance / 10.0;

        float result = RayCaster::HitSphere(eye, rayDir, pos, radius);
        if(result >=0 && result < t)
        {
            hitVertex = vert.get();
            t = result;
        }
    }

    return hitVertex;
}

void JointWeightEditMode::VertexSelectionChanged(Vertex* v)
{
    if(v != nullptr)
    {
        m_VertexDisplay->ClearSelection();
        m_VertexDisplay->SelectVertex(v);
        glm::ivec4 ids(-1);
        glm::vec4 weights(0);
        SkeletalMesh::GetFirstKWeight(m_SkeletalMesh->GetVertexWeights()[v->GetIndex()],
                                      4,
                                      weights,
                                      ids);
        m_VertexJointWeightEditor->SetDisabled(false);
        std::array<QString, 4> jointNames{
            ids[0] >= 0 ? m_SkeletalMesh->GetSkeleton()->GetJoints()[ids[0]]->GetName() : "None",
            ids[1] >= 0 ? m_SkeletalMesh->GetSkeleton()->GetJoints()[ids[1]]->GetName() : "None",
            ids[2] >= 0 ? m_SkeletalMesh->GetSkeleton()->GetJoints()[ids[2]]->GetName() : "None",
            ids[3] >= 0 ? m_SkeletalMesh->GetSkeleton()->GetJoints()[ids[3]]->GetName() : "None",
        };
        QStringList aviableJoints;
        for(int i = 0; i < m_SkeletalMesh->GetSkeleton()->GetJoints().size(); ++i)
        {
            if(i != ids[0] && i != ids[1] && i != ids[2] && i != ids[3])
            {
                aviableJoints.append(m_SkeletalMesh->GetSkeleton()->GetJoints()[i]->GetName());
            }
        }

        m_VertexJointWeightEditor->SetJointNames(aviableJoints);
        m_VertexJointWeightEditor->SetValue(jointNames,
                                            {weights[0], weights[1], weights[2], weights[3]},
                                            m_SkeletalMesh->GetSkeleton()->GetJointsCount(),
                                            true);
        m_VertexDisplay->create();
    }
    else
    {
        m_VertexDisplay->ClearSelection();
        m_VertexJointWeightEditor->SetDisabled(true);
        m_VertexJointWeightEditor->ClearValue();
    }
    m_VertexDisplay->create();
    m_MyGL->update();
}

void JointWeightEditMode::slot_JointChanged(const QString& preJoint, const QString& currentJoint)
{
    const Vertex* v = m_VertexDisplay->GetCurrentSelectedVertex();
    int pre = -1, cur = -1;
    for(int i = 0; i < m_SkeletalMesh->GetSkeleton()->GetJoints().size(); ++i)
    {
        if(m_SkeletalMesh->GetSkeleton()->GetJoints()[i]->GetName() == preJoint)
        {
            pre = i;
        }
        else if(m_SkeletalMesh->GetSkeleton()->GetJoints()[i]->GetName() == currentJoint)
        {
            cur = i;
        }
    }
    float w = 0.0;
    if(pre != -1)
    {
        w = m_SkeletalMesh->GetVertexWeights()[v->GetIndex()][pre];
        m_SkeletalMesh->GetVertexWeights()[v->GetIndex()][pre] = 0;
    }
    if(cur != -1)
    {
        m_SkeletalMesh->GetVertexWeights()[v->GetIndex()][cur] = w;
    }

    m_Parent->Update();
}

void JointWeightEditMode::slot_WeightChanged(const QString& currentJoint, double weight)
{
    const Vertex* v = m_VertexDisplay->GetCurrentSelectedVertex();
    for(int i = 0; i < m_SkeletalMesh->GetSkeleton()->GetJoints().size(); ++i)
    {
        if(m_SkeletalMesh->GetSkeleton()->GetJoints()[i]->GetName() == currentJoint)
        {
            m_SkeletalMesh->GetVertexWeights()[v->GetIndex()][i] = weight;
            m_Parent->Update();
            return;
        }
    }
}
