#ifndef DEBUGPOINT_H
#define DEBUGPOINT_H

#include <drawable.h>

class DebugPoint : public Drawable
{
public:
    DebugPoint(OpenGLContext* context);

public:
    virtual void create() override;
    virtual GLenum drawMode() override;

public:
    bool enabled;
    glm::vec3 color;
    glm::vec3 position;
};

#endif // DEBUGPOINT_H
