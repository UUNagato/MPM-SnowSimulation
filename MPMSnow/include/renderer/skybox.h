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
    uint getIrradianceMap() const { return irradianceMap; }
    uint getPrefilterMap() const { return prefilterMap; }
    uint getBRDFLUT() const { return brdfLUTTexture; }

private:
    uint envCubemap;
    uint irradianceMap;
    uint prefilterMap;
    uint brdfLUTTexture;

    uint maxMipLevel;

    void createCubeMap(int width, int height, uint &texture);
};

G_NAMESPACE_END