#include "wireframesphere.h"

#include <vector>

WireframeSphere::WireframeSphere(OpenGLContext* context)
    :Drawable(context)
{}

void WireframeSphere::create()
{
    std::vector<glm::vec4> positions{
        // x-axis
        { 0.f,  0.500,  0.000, 1.f},
        { 0.f,  0.433, -0.250, 1.f},
        { 0.f,  0.250, -0.433, 1.f},
        { 0.f,  0.000, -0.500, 1.f},
        { 0.f, -0.250, -0.433, 1.f},
        { 0.f, -0.433, -0.250, 1.f},
        { 0.f, -0.500,  0.000, 1.f},
        { 0.f, -0.433,  0.250, 1.f},
        { 0.f, -0.250,  0.433, 1.f},
        { 0.f,  0.000,  0.500, 1.f},
        { 0.f,  0.250,  0.433, 1.f},
        { 0.f,  0.433,  0.250, 1.f},

        // y-axis
        {  0.500, 0.f,  0.000, 1.f},
        {  0.433, 0.f, -0.250, 1.f},
        {  0.250, 0.f, -0.433, 1.f},
        {  0.000, 0.f, -0.500, 1.f},
        { -0.250, 0.f, -0.433, 1.f},
        { -0.433, 0.f, -0.250, 1.f},
        { -0.500, 0.f,  0.000, 1.f},
        { -0.433, 0.f,  0.250, 1.f},
        { -0.250, 0.f,  0.433, 1.f},
        {  0.000, 0.f,  0.500, 1.f},
        {  0.250, 0.f,  0.433, 1.f},
        {  0.433, 0.f,  0.250, 1.f},
        // z-axis
        {  0.500,  0.000, 0.f, 1.f},
        {  0.433, -0.250, 0.f, 1.f},
        {  0.250, -0.433, 0.f, 1.f},
        {  0.000, -0.500, 0.f, 1.f},
        { -0.250, -0.433, 0.f, 1.f},
        { -0.433, -0.250, 0.f, 1.f},
        { -0.500,  0.000, 0.f, 1.f},
        { -0.433,  0.250, 0.f, 1.f},
        { -0.250,  0.433, 0.f, 1.f},
        {  0.000,  0.500, 0.f, 1.f},
        {  0.250,  0.433, 0.f, 1.f},
        {  0.433,  0.250, 0.f, 1.f}
    };
    std::vector<glm::vec4> colors{
        // x-axis
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        { 1.f, 0.f, 0.f, 1.f},
        // y-axis
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        { 0.f, 1.f, 0.f, 1.f},
        // z-axis
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f},
        { 0.f, 0.f, 1.f, 1.f}
    };
    std::vector<GLuint> indices{
        0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 0, // x-axis
        12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 12, // y-axis
        24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 24  // z-axis
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

GLenum WireframeSphere::drawMode()
{
    return GL_LINES;
}
