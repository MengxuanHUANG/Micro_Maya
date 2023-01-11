#include "skeleton.h"

#include "joint.h"
#include <glm/glm.hpp>
#include <queue>
#include <stack>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <iostream>

#define COLOR_MAGENTA glm::vec4{1, 0, 1, 1}
#define COLOR_YELLOW  glm::vec4{1, 1, 0, 1}
#define ORIGIN glm::vec3{0, 0, 0}

void Skeleton::LoadSkeleton(Skeleton* skeleton, const QString& path)
{
    QFile file(path);

    skeleton->m_Joints.clear();
    skeleton->m_RootJoint.release();
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cout << "Can't open \n" << path.toStdString() << std::endl;
    }
    else
    {
        QByteArray data = file.readAll();
        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonObject root = json.object().value("root").toObject();
        QString name = root.value("name").toString();
        skeleton->m_RootJoint = std::make_unique<Joint>(name);
        LoadJoint(skeleton->m_RootJoint.get(), root, skeleton->m_Joints);

        skeleton->Update();
    }
}

void Skeleton::LoadJoint(Joint* joint,
                         const QJsonObject& objct,
                         std::vector<Joint*>& joints)
{
    joint->SetID(joints.size());
    joints.emplace_back(joint);

    const QJsonValue& posValue = objct.value("pos");
    const QJsonValue& rotValue = objct.value("rot");
    glm::vec3 pos{posValue[0].toDouble(),
                  posValue[1].toDouble(),
                  posValue[2].toDouble()};
    glm::vec4 rot{rotValue[0].toDouble(),
                  rotValue[1].toDouble(),
                  rotValue[2].toDouble(),
                  rotValue[3].toDouble()};

    glm::mat4 R = glm::rotate(glm::mat4(1), rot[0], {rot[1], rot[2], rot[3]});
    joint->SetLocalTranslation(pos);
    joint->SetLocalRotation(glm::toQuat(glm::transpose(R)));
    QJsonArray children = objct.value("children").toArray();
    for(const auto& ref : children)
    {
        QJsonObject obj = ref.toObject();
        QString name = obj.value("name").toString();
        Joint* child = joint->EmplaceChildJoint(name);
        child->SetParent(joint);

        LoadJoint(child, obj, joints);
    }
}

void Skeleton::HelpComputeBind(Joint* joint, glm::mat4 F)
{
    F = joint->getLocalTransformation().GetTransformMat4() * F;
    joint->SetBindMat4(glm::inverse(F));
    for(std::unique_ptr<Joint>& child : joint->GetChildren())
    {
        HelpComputeBind(child.get(), F);
    }
}

void Skeleton::HelpCreate(Joint* joint,
                          std::vector<glm::vec4>& pos,
                          std::vector<glm::vec4>& col,
                          std::vector<GLuint>& idx)
{
    glm::vec4 p = glm::vec4(joint->getOverallTransformation().GetTranslation(), 1.0);
    // second point of a line
    if(joint->GetParent() != nullptr)
    {
        pos.emplace_back(p);
        col.emplace_back(COLOR_YELLOW);
    }

    for(std::unique_ptr<Joint>& child : joint->GetChildren())
    {
        // first point of a line
        pos.emplace_back(p);
        col.emplace_back(COLOR_MAGENTA);
        HelpCreate(child.get(), pos, col, idx);
    }
}

Skeleton::Skeleton(OpenGLContext* context)
    :Drawable(context)
{}

void Skeleton::ComputeBind()
{
    HelpComputeBind(m_RootJoint.get(), glm::mat4(1));
}

void Skeleton::create()
{
    if(m_RootJoint && m_RootJoint->GetChildren().size() > 0)
    {
        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> colors;
        std::vector<GLuint> indices;

        HelpCreate(m_RootJoint.get(), positions, colors, indices);

        for(GLuint i = 0; i < positions.size(); ++i)
        {
            indices.emplace_back(i);
        }
        count = indices.size();

        bindIdx();
        mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        bindPos();
        mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

        bindCol();
        mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
    }
    else
    {
        count = 0;
    }
}

GLenum Skeleton::drawMode()
{
    return GL_LINES;
}
