#include "debugpoint.h"

DebugPoint::DebugPoint(OpenGLContext* context)
    :Drawable(context), enabled(false), position(0.0), color(1)
{}

void DebugPoint::create()
{
    destroy();
    generateIdx();
    bindIdx();
    GLuint i = 0;
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), &i, GL_STATIC_DRAW);

    generatePos();
    bindPos();
    glm::vec4 p{position, 1.0};
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), &p, GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), &color, GL_STATIC_DRAW);

    count = 1;
}

GLenum DebugPoint::drawMode()
{
    return GL_POINTS;
}
