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
    Camera(glm::vec3 &pos, glm::vec3 &direction);

    void lookAt(const glm::vec3 &pos);

protected:
    glm::vec3 cameraPos;
    glm::vec3 cameraDirection;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
};

G_NAMESPACE_END