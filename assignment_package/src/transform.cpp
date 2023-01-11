#include "transform.h"
#include <memory>

Transform::Transform(const glm::vec3& translate, const glm::quat& rotate)
    :m_Translation(translate), m_Rotation(rotate)
{}
Transform::Transform()
    :Transform(glm::vec3(), glm::quat())
{}

Transform::Transform(const Transform& T)
    :Transform(T.m_Translation, T.m_Rotation)
{}

Transform::Transform(Transform&& T)
    :m_Translation(std::move(T.m_Translation)),
     m_Rotation(std::move(T.m_Rotation))
{}

Transform& Transform::operator=(const Transform& T)
{
    this->m_Translation = T.m_Translation;
    this->m_Rotation = T.m_Rotation;
}

Transform Transform::operator*(const Transform& T) const
{
    glm::mat3 R1 = this->GetRotationMat3();
    glm::mat3 R2 = T.GetRotationMat3();
    return {T.m_Translation * R1 + this->m_Translation, glm::toQuat(R2 * R1)};
}

glm::vec3 Transform::operator*(const glm::vec3& v) const
{
    return  v * GetRotationMat3() + m_Translation;
}
