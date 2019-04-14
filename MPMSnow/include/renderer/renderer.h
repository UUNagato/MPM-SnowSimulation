#pragma once

#include <glad/glad.h>
#include "../common/util.h"
#include "../common/math.h"

G_NAMESPACE_BEGIN

class Renderer
{
public:
    Renderer();                 // default constructor
    Renderer(const char* vertex, const char* fragment); // given path constructor

    virtual ~Renderer();        // destructor
    bool virtual useShader(const char* vertex, const char* fragment);

protected:
    uint m_program;             // shader program
};

G_NAMESPACE_END