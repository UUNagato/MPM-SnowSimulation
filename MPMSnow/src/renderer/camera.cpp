#include "renderer/camera.h"

G_NAMESPACE_BEGIN

Camera::Camera() : cameraPos(0.0f, 0.0f, 3.0f), cameraYaw(-90.0f), cameraPitch(0.0f), cameraSpeed(2.5f),
    cameraZoom(45.0f)
{
    // calculate right axis
    // lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    UpdateCameraVectors();
}

void Camera::setPosition(const glm::vec3 &pos)
{
    cameraPos = pos;
}

void Camera::setPosition(float x, float y, float z)
{
    cameraPos = glm::vec3(x, y, z);
}

void Camera::setZoom(float z)
{
    cameraZoom = z;
    if (cameraZoom < 1.0f)
        cameraZoom = 1.0f;
    if (cameraZoom > 45.0f)
        cameraZoom = 45.0f;
}

void Camera::lookAt(const glm::vec3 &pos)
{
    /* for now, it does nothing, since yaw and pitch are not updated.
    cameraDirection = glm::normalize(pos - cameraPos);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    cameraRight = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
    */
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

    cameraDirection = glm::normalize(front);
    cameraRight = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

}

void Camera::setYawandPitch(float yaw, float pitch)
{
    cameraYaw = yaw;
    if (pitch > 89.0f)
        cameraPitch = 89.0f;
    else if (pitch < -89.0f)
        cameraPitch = -89.0f;
    else
        cameraPitch = pitch;
    UpdateCameraVectors();
}
G_NAMESPACE_END