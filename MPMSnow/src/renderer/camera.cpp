#include "renderer/camera.h"

G_NAMESPACE_BEGIN

Camera::Camera() : cameraPos(0.0f, 0.0f, 3.0f)
{
    // calculate right axis
}

void Camera::lookAt(const glm::vec3 &pos)
{
    cameraDirection = glm::normalize(pos - cameraPos);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
}
G_NAMESPACE_END