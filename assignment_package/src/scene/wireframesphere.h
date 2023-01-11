#ifndef WIREFRAMESPHERE_H
#define WIREFRAMESPHERE_H

#include <drawable.h>

class WireframeSphere : public Drawable
{
public:
    WireframeSphere(OpenGLContext* context);

public:
    virtual void create() override;
    virtual GLenum drawMode() override;

};

#endif // WIREFRAMESPHERE_H
