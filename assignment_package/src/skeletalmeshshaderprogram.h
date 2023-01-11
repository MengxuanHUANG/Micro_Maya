#ifndef SKELETALMESHSHADERPROGRAM_H
#define SKELETALMESHSHADERPROGRAM_H

#include "shaderprogram.h"
#include "scene/skeletalmesh.h"

class SkeletalMeshShaderProgram : public ShaderProgram
{
public:
    int attrJointID;
    int attrJointWeight;

    int unifBind; // A handle for the "uniform" mat4 array representing bind matrix in the vertex shader
    int unifOverall; // A handle for the "uniform" mat4 array representing overall matrix in the vertex shader

public:
    SkeletalMeshShaderProgram(OpenGLContext* context);

    bool bindId();
    bool bindWeight();

    void SetJointBind(Skeleton* skeleton);
    void SetJointOverall(Skeleton* skeleton);

    void create(const char *vertfile, const char *fragfile);
    void draw(SkeletalMesh &d);
};

#endif // SKELETALMESHSHADERPROGRAM_H
