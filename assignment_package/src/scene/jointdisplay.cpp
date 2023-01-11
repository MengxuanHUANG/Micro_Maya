#include "jointdisplay.h"

#include "joint.h"
#include <iostream>

JointDisplay::JointDisplay(OpenGLContext* context)
    :Drawable(context), m_SelectedJoint(nullptr)
{}

void JointDisplay::create()
{
    if(m_SelectedJoint != nullptr)
    {
        std::vector<glm::vec4> positions{
            {   0.f,  0.25,   0.0, 1.f},
            {   0.f,   0.0, -0.25, 1.f},
            {   0.f, -0.25,   0.0, 1.f},
            {   0.f,   0.0,  0.25, 1.f},
            {  0.25,   0.0,   0.0, 1.f},
            { -0.25,   0.0,   0.0, 1.f},
            {   0.0,   0.0,  0.25, 1.f}
        };
        std::vector<glm::vec4> colors{
            { 1.f, 1.f, 1.f, 1.f},
            { 1.f, 1.f, 1.f, 1.f},
            { 1.f, 1.f, 1.f, 1.f},
            { 1.f, 1.f, 1.f, 1.f},
            { 1.f, 1.f, 1.f, 1.f},
            { 1.f, 1.f, 1.f, 1.f}
        };
        std::vector<GLuint> indices{
            0, 1, 1, 2, 2, 3, 3, 0, // x-axis
            4, 1, 1, 5, 5, 3, 3, 4, // y-axis
            4, 2, 2, 5, 5, 0, 0, 4 // z-axis
        };

        generateIdx();
        bindIdx();
        mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        generatePos();
        bindPos();
        mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

        generateCol();
        bindCol();
        mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

        count = indices.size();
    }
    else
    {
        count = 0;
    }
}

GLenum JointDisplay::drawMode()
{
    return GL_LINES;
}
