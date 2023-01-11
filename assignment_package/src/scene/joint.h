#ifndef JOINT_H
#define JOINT_H

#include <QTreeWidgetItem>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "transform.h"

class Joint : public QTreeWidgetItem
{
public:
    Joint(const QString&, Joint*, const glm::vec3&, const glm::quat&);
    Joint(const QString&);

    void UpdateTransform();

    Joint* EmplaceChildJoint(const QString& name);
    Joint* AddChildJoint(std::unique_ptr<Joint>& joint);

public:
    inline const QString& GetName() const { return m_Name; }
    inline const Transform& getLocalTransformation() const { return m_LocalTransform; }
    inline const Transform& getOverallTransformation() const { return m_GlobalTransform; }

    inline void SetID(const unsigned int& id) { m_ID = id; }
    inline unsigned int& GetID() { return m_ID; }

    inline void SetParent(Joint* joint) { m_Parent = joint; }
    inline Joint* GetParent() const { return m_Parent; }

    inline void SetLocalTranslation(const glm::vec3& translate) { m_LocalTransform.SetTranslate(translate); }
    inline const glm::vec3& GetLocalTranslation() const { return m_LocalTransform.GetTranslation(); }

    inline void SetLocalRotation(const glm::quat& rotate) { m_LocalTransform.SetRotate(rotate); }
    inline const glm::quat& GetLocalRotation() const { return m_LocalTransform.GetRotationQuat(); }

    inline void SetBindMat4(const glm::mat4& bind) { m_BindMat = bind; }
    inline const glm::mat4& GetBindMat4() const { return m_BindMat; }

    inline const Transform& GetLocal2Global() const { return m_GlobalTransform; }

    inline const Transform& GetLocal2Parent() const { return m_LocalTransform; }

    inline std::vector<std::unique_ptr<Joint>>& GetChildren() { return m_Children; }
protected:
    unsigned int m_ID;

    QString m_Name;
    Joint* m_Parent;
    Transform m_LocalTransform;
    Transform m_GlobalTransform;

    glm::mat4 m_BindMat;

    std::vector<std::unique_ptr<Joint>> m_Children;
};

#endif // JOINT_H
