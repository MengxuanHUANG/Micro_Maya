#ifndef DEBUGLINE_H
#define DEBUGLINE_H

#include "drawable.h"

#include <array>
#include <glm/glm.hpp>

class DebugLine : public Drawable
{
public:
    DebugLine(OpenGLContext* context);

    virtual void create() override;
    virtual GLenum drawMode() override;

public:
    bool enabled;
    glm::vec3 color;
    std::array<glm::vec3, 2> positions;
};

#endif // DEBUGLINE_H
