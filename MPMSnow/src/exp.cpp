//===========================================================
// This file is written to familier with Eigen and further experiment MPM
//===========================================================

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "common/util.h"
#include "common/math.h"

#include "mpm/mpm.h"
#include "renderer/renderer.h"

using namespace G_NAMESPACE_NAME;
int main()
{
    MPM mpmsolver(80);
    // mpmsolver.addObject(Vector3(0.5, 0.5, 0.55));
    mpmsolver.addObject(Vector3(0.55, 0.5, 0.25));

    // create mpm solver
    // MPM mpm(100);
    if (glfwInit())
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        GLFWwindow* window = glfwCreateWindow(1024, 768, "My Title", NULL, NULL);
        if (!window) {
            // create failed
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwSwapInterval(1);

        Renderer renderer;

        int render_dt = -1;

        while (!glfwWindowShouldClose(window)) {
            mpmsolver.advance();
            //render_dt = (render_dt + 1) % 10;
            //if (render_dt != 0)
            //    continue;

            glViewport(0, 0, 1024, 768);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            // std::cout << "Advanced one step!" << std::endl;

            // renderer.renderParticles(mpmsolver.getParticles());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    return 0;
}