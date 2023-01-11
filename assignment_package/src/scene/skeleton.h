#ifndef SKELETON_H
#define SKELETON_H

#include <drawable.h>
#include <vector>
#include "joint.h"


class Skeleton : public Drawable
{
public:
    Skeleton(OpenGLContext* context);

    void ComputeBind();

    inline void Update() { m_RootJoint->UpdateTransform(); }
    inline Joint* GetRootJoint() const { return m_RootJoint.get(); }
    inline const std::vector<Joint*>& GetJoints() { return m_Joints;}
    inline unsigned int GetJointsCount() const { return m_Joints.size(); }
public:
    virtual void create() override;
    virtual GLenum drawMode() override;

public:
    static void LoadSkeleton(Skeleton* skeleton, const QString& path);
    static void LoadJoint(Joint* joint,
                          const QJsonObject& objct,
                          std::vector<Joint*>& joints);

    static void HelpComputeBind(Joint* joint, glm::mat4 F);

    static void HelpCreate(Joint*,
                           std::vector<glm::vec4>&,
                           std::vector<glm::vec4>&,
                           std::vector<GLuint>&);
protected:
    std::unique_ptr<Joint> m_RootJoint;

    std::vector<Joint*> m_Joints;
};

#endif // SKELETON_H
