#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0))
{
    theta = 0;
    phi = 0;
    zoom = 12;
}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp),
    theta(0),
    phi(0),
    zoom(12)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H),
    theta(c.theta),
    phi(c.phi),
    zoom(c.zoom)
{}


void Camera::RecomputeAttributes()
{
    float x = glm::radians(phi);
    float y = glm::radians(theta);

    float sx = glm::sin(x);
    float cx = glm::cos(x);
    float sy = glm::sin(y);
    float cy = glm::cos(y);

    glm::mat4 Ref{
        {1, 0, 0, ref.x},
        {0, 1, 0, ref.y},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    glm::mat4 Z{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, zoom},
        {0, 0, 0, 1}
    };
    glm::mat4 R1{
        {1,   0,  0, 0},
        {0,  cx, sx, 0},
        {0, -sx, cx, 0},
        {0,   0,  0, 1}
    };
    glm::mat4 R2{
        {cy, 0, -sy, 0},
        { 0, 1,   0, 0},
        {sy, 0,  cy, 0},
        { 0, 0,   0, 1}
    };

    // from camera space to world space
    glm::mat4 F = Z * R1 * R2 * Ref;

    eye = glm::vec3(glm::vec4(0, 0, 0, 1) * F);
    look = glm::vec3(glm::vec4(0, 0, -1, 0) * F);
    right = glm::vec3(glm::vec4(1, 0, 0, 0) * F);
    up = glm::vec3(glm::vec4(0, 1, 0, 0) * F);

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width / static_cast<float>(height);
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    zoom -= amt;
}

void Camera::TranslateAlongRight(float amt)
{
    ref += amt * right;
}
void Camera::TranslateAlongUp(float amt)
{
    ref += amt * up;
}

void Camera::RotateAroundCenter(const float& x_amt, const float& y_amt)
{
    theta += y_amt;
    phi += x_amt;
}
