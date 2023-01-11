#include "jointeditmode.h"

#include <QTreeWidget>
#include "guivec3editor.h"
#include "mygl.h"

#include "scene/joint.h"
#include "scene/jointdisplay.h"
#include "scene/skeleton.h"
#include "raycaster.h"
#include "camera.h"


JointEditMode::JointEditMode(MyGL* mygl, Mode* parent)
    :Mode(mygl, parent)
{}

void JointEditMode::Load()
{
    connect(m_JointPosEditor,
            SIGNAL(sig_Vec3Changed(const glm::vec3&)),
            this,
            SLOT(slot_JointPosChanged(const glm::vec3&)));

    connect(m_JointRotEditor,
            SIGNAL(sig_Vec3Changed(const glm::vec3&)),
            this,
            SLOT(slot_JointRotChanged(const glm::vec3&)));
}

void JointEditMode::UnLoad()
{
    disconnect(m_JointPosEditor,
                SIGNAL(sig_Vec3Changed(const glm::vec3&)),
                this,
                SLOT(slot_JointPosChanged(const glm::vec3&)));

    disconnect(m_JointRotEditor,
                SIGNAL(sig_Vec3Changed(const glm::vec3&)),
                this,
                SLOT(slot_JointRotChanged(const glm::vec3&)));
}

void JointEditMode::MousePressEvent(QMouseEvent *event)
{
    const QPoint& pos = event->pos();
    Joint* selected = SelectAt(pos.x(), pos.y());
    if(selected != nullptr)
    {
        m_JointTree->setCurrentItem(selected);
    }
    else
    {
        m_JointTree->clearSelection();
    }
}

Joint* JointEditMode::SelectAt(const unsigned int& x, const unsigned int& y)
{
    glm::vec3 posInWorld = m_MyGL->GetPxielInWorld(x, y, m_Camera->far_clip);
    glm::vec3 rayDir = glm::normalize(posInWorld - m_Camera->eye);

    glm::vec3& eye = m_Camera->eye;
    glm::vec3& look = m_Camera->look;

    Joint* hitJoint = nullptr;
    float t = std::numeric_limits<float>::max();
    for(Joint* joint : m_Skeleton->GetJoints())
    {
        const glm::vec3& pos = joint->getOverallTransformation().GetTranslation();
        float distance = glm::dot(look, pos - eye);
        float radius = 0.4 * distance / 10.0;

        float result = RayCaster::HitSphere(eye, rayDir, pos, radius);
        if(result >=0 && result < t)
        {
            hitJoint = joint;
            t = result;
        }
    }

    return hitJoint;
}

void JointEditMode::slot_JointPosChanged(const glm::vec3& value)
{
    m_JointDisplay->GetSelectedJoint()->SetLocalTranslation(value);
    m_JointDisplay->GetSelectedJoint()->UpdateTransform();
    m_Parent->Update();
}

void JointEditMode::slot_JointRotChanged(const glm::vec3& value)
{
    glm::mat4 R;
    for(int i = 0; i < 3; ++i)
    {
        if(value[i] != 0)
        {
            glm::vec3 axis(0);
            axis[i] = 1;
            R = glm::rotate(glm::mat4(1), value[i], axis);
            break;
        }
    }
    const glm::quat& q1 = m_JointDisplay->GetSelectedJoint()->GetLocalRotation();
    glm::quat q = glm::toQuat(R);
    m_JointDisplay->GetSelectedJoint()->SetLocalRotation(q1 * q);
    m_JointDisplay->GetSelectedJoint()->UpdateTransform();
    m_JointRotEditor->SetValue(glm::vec3(0), true);

    m_Parent->Update();
}
