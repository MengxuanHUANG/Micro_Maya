#include "skeletalmeshshaderprogram.h"

#include "scene/skeleton.h"
#include "scene/joint.h"
#include <iostream>

SkeletalMeshShaderProgram::SkeletalMeshShaderProgram(OpenGLContext* context)
    :ShaderProgram(context),
      attrJointID(-1),
      attrJointWeight(-1),
      unifBind(-1),
      unifOverall(-1)
{}

void SkeletalMeshShaderProgram::create(const char *vertfile, const char *fragfile)
{
    ShaderProgram::create(vertfile, fragfile);
    attrJointWeight = context->glGetAttribLocation(prog, "vs_Weight");
    attrJointID = context->glGetAttribLocation(prog, "vs_ID");

    unifBind = context->glGetUniformLocation(prog, "u_Bind");
    unifOverall = context->glGetUniformLocation(prog, "u_Overall");
}

void SkeletalMeshShaderProgram::draw(SkeletalMesh &skeletalMesh)
{
    if(skeletalMesh.elemCount() < 0) {
        throw std::invalid_argument(
        "Attempting to draw a Drawable that has not initialized its count variable! Remember to set it to the length of your index array in create()."
        );
    }
    useMe();

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

        // Remember, by calling bindPos(), we call
        // glBindBuffer on the Drawable's VBO for vertex position,
        // meaning that glVertexAttribPointer associates vs_Pos
        // (referred to by attrPos) with that VBO
    if (attrPos != -1 && skeletalMesh.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, nullptr);
    }

    if (attrNor != -1 && skeletalMesh.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, nullptr);
    }

    if (attrCol != -1 && skeletalMesh.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, nullptr);
    }
    if (attrJointID != -1 && skeletalMesh.bindID()) {
        context->glEnableVertexAttribArray(attrJointID);
        context->glVertexAttribIPointer(attrJointID, 4, GL_INT, 0, nullptr);
    }
    if (attrJointWeight != -1 && skeletalMesh.bindWeight()) {
        context->glEnableVertexAttribArray(attrJointWeight);
        context->glVertexAttribPointer(attrJointWeight, 4, GL_FLOAT, false, 0, nullptr);
    }
    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    skeletalMesh.bindIdx();
    context->glDrawElements(skeletalMesh.drawMode(), skeletalMesh.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrJointWeight != -1) context->glDisableVertexAttribArray(attrJointWeight);
    if (attrJointID != -1) context->glDisableVertexAttribArray(attrJointID);
    context->printGLErrorLog();
}

void SkeletalMeshShaderProgram::SetJointBind(Skeleton* skeleton)
{
    useMe();

    if (unifBind != -1)
    {
        std::vector<glm::mat4> binds;
        for(Joint* j : skeleton->GetJoints())
        {
            binds.emplace_back(j->GetBindMat4());
        }

        context->glUniformMatrix4fv(unifBind,
                                    skeleton->GetJointsCount(),
                                    GL_FALSE,
                                    &(binds[0][0][0]));
    }
}

void SkeletalMeshShaderProgram::SetJointOverall(Skeleton* skeleton)
{
    useMe();

    if (unifOverall != -1 && skeleton->GetJointsCount() > 0)
    {
        std::vector<glm::mat4> overalls;
        for(Joint* j : skeleton->GetJoints())
        {
            overalls.emplace_back(j->getOverallTransformation().GetTransformMat4());
        }
        context->glUniformMatrix4fv(unifOverall,
                                    skeleton->GetJointsCount(),
                                    GL_FALSE,
                                    &(overalls[0][0][0]));
    }
}
