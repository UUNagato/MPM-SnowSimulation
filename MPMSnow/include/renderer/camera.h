#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/util.h"

G_NAMESPACE_BEGIN

class Camera
{
public:
    Camera();
    // Camera(glm::vec3 &pos);

    void lookAt(const glm::vec3 &pos);
    void setPosition(const glm::vec3 &pos);
    void setPosition(float x, float y, float z);

    glm::vec3 getPosition() { return cameraPos; }

    glm::mat4 getViewMatrix();

protected:
    glm::vec3 cameraPos;
    glm::vec3 cameraDirection;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
};

G_NAMESPACE_END