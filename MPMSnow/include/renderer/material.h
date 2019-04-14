#pragma once
#include <glm/glm.hpp>
#include "common/math.h"

G_NAMESPACE_BEGIN

struct Material
{
    glm::vec3 albedo;
    float roughness;
    float metallic;
    float ao;

    Material(const glm::vec3 &alb, float r, float m, float a) : albedo(alb),
        roughness(r), metallic(m), ao(a) {};
};

G_NAMESPACE_END