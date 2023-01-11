#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
    Transform();
    Transform(const glm::vec3& translate, const glm::quat& rotate);
    Transform(const Transform& T); // deep copy constructor
    Transform(Transform&& T); // move constructor

    Transform& operator=(const Transform& T);
    Transform operator*(const Transform& T) const;
    glm::vec3 operator*(const glm::vec3& v) const;

public:
    inline void SetTranslate(const glm::vec3& translate) { m_Translation = translate; }
    inline void SetRotate(const glm::quat& rotate) { m_Rotation = rotate; }

    inline glm::mat4 GetTransformMat4() const
    {
        glm::mat4 F = glm::toMat4(m_Rotation);
        F[0][3] = m_Translation.x;
        F[1][3] = m_Translation.y;
        F[2][3] = m_Translation.z;
        return F;
    }

    inline const glm::vec3& GetTranslation() const { return m_Translation; }
    inline const glm::quat& GetRotationQuat() const { return m_Rotation; }
    inline glm::mat3 GetRotationMat3() const { return glm::toMat3(m_Rotation); }

    inline glm::vec3 RotateVec3(const glm::vec3& v) const { return v * GetRotationMat3();}
    inline glm::vec3 TranslateVec3(const glm::vec3& v) const { return v + m_Translation;}

protected:
    glm::vec3 m_Translation;
    glm::quat m_Rotation;
};

#endif // TRANSFORM_H
