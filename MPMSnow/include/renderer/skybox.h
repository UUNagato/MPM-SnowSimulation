#pragma once
/*
    Skybox
    read HDR file and convert it into a cubemap
    used for image based lighting.
*/

#include <glfw/glfw3.h>
#include "common/math.h"

G_NAMESPACE_BEGIN

class Skybox
{
public:
    Skybox(const char* path, GLFWwindow *window);
    ~Skybox();

    uint getCubeMap() const { return envCubemap; }

private:
    uint envCubemap;
};

G_NAMESPACE_END