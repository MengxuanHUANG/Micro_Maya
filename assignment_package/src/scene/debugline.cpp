#include "debugline.h"

DebugLine::DebugLine(OpenGLContext* context)
    :Drawable(context), enabled(false), color(1, 1, 1)
{}

void DebugLine::create()
{
    destroy();
    generateIdx();
    bindIdx();
    std::array<GLuint, 2> indices = {0, 1};
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    std::array<glm::vec4, 2> pos {glm::vec4(positions[0], 1.0), glm::vec4(positions[1], 1.0)};
    mp_context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    std::array<glm::vec4, 2> col{glm::vec4(color, 1), glm::vec4(color, 1)};
    mp_context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);

    count = 2;
}

GLenum DebugLine::drawMode()
{
    return GL_LINES;
}
