#include "joint.h"

Joint::Joint(const QString& name,
      Joint* parent,
      const glm::vec3& position,
      const glm::quat& orientation)
    :m_Name(name), m_Parent(parent), m_LocalTransform(position, orientation)
{
    setText(0, name);
}

Joint::Joint(const QString& name)
    :Joint(name, nullptr, glm::vec3(0), glm::quat())
{}

void Joint::UpdateTransform()
{
    if(m_Parent != nullptr)
    {
        m_GlobalTransform = m_Parent->GetLocal2Global() * m_LocalTransform;
    }
    else
    {
        m_GlobalTransform = m_LocalTransform;
    }

    for(std::unique_ptr<Joint>& child : m_Children)
    {
        child->UpdateTransform();
    }
}

Joint* Joint::EmplaceChildJoint(const QString& name)
{
    std::unique_ptr<Joint> child = std::make_unique<Joint>(name);
    return Joint::AddChildJoint(child);
}

Joint* Joint::AddChildJoint(std::unique_ptr<Joint>& joint)
{
    Joint* j = joint.get();
    m_Children.emplace_back(std::move(joint));
    QTreeWidgetItem::addChild(j);
    return j;
}
