#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "common/math.h"

#include <stb_image/stb_image.h>

#define SAFE_RELEASETEX(x) if(x>0) glDeleteTextures(1, &x)


G_NAMESPACE_BEGIN

struct Material
{
    glm::vec3 albedo;
    float roughness;
    float metallic;
    float ao;
    uint albedoMap;
    uint normalMap;
    uint roughnessMap;
    uint metallicMap;
    uint aoMap;

    Material(const glm::vec3 &alb, float r, float m, float a) : albedo(alb),
        roughness(r), metallic(m), ao(a), albedoMap(0), normalMap(0),
        roughnessMap(0), metallicMap(0), aoMap(0) {};

    Material(const char *albedoPath, const char *normalPath, const char *roughnessPath,
        const char *metallicPath, const char *aoPath) : albedo(0.f), roughness(1.0f), metallic(0.0f),
                ao(1.0f) 
    {
        LoadTexture(albedoMap, albedoPath);
        LoadTexture(normalMap, normalPath);
        LoadTexture(roughnessMap, roughnessPath);
        LoadTexture(metallicMap, metallicPath);
        LoadTexture(aoMap, aoPath);
    }

    ~Material() {
        SAFE_RELEASETEX(albedoMap);
        SAFE_RELEASETEX(normalMap);
        SAFE_RELEASETEX(roughnessMap);
        SAFE_RELEASETEX(metallicMap);
        SAFE_RELEASETEX(aoMap);
    }

private:
    void LoadTexture(uint &texture, const char *path)
    {
        glGenTextures(1, &texture);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
    }
};

G_NAMESPACE_END