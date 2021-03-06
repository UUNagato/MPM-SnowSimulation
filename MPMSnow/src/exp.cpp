//===========================================================
// This file is written to familier with Eigen and further experiment MPM
//===========================================================

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "common/util.h"
#include "common/math.h"
#include <io/bgeoexporter.h>

#include <mpm/mpm.h>
#include <renderer/mpmrenderer.h>

using namespace G_NAMESPACE_NAME;

// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

Camera cam;
float deltaTime;
float lastTime;

bool pause = false;

int main()
{
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
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwSwapInterval(1);

        BgeoExporter exporter;
        MPM mpmsolver(100);
        // mpmsolver.addObject(Vector3(0.5, 0.5, 0.55));
        //mpmsolver.addCollideBody(new RigidSphere(0_f, 0_f), TSMatrix(Vector3(0.5_f, 0.25_f, 0.5_f), Vector3::Constant(0.1_f)));

        mpmsolver.addObject(Vector3(0.50_f, 0.50_f, 0.5_f));

        MPMRenderer renderer;
        cam.setPosition(glm::vec3(0.f, 0.f, 2.f));

        int render_dt = -1;
        int captured_frame = 0;

        while (!glfwWindowShouldClose(window)) {
            float current = glfwGetTime();
            deltaTime = current - lastTime;
            lastTime = current;
            processInput(window);

            if (!pause) {
                mpmsolver.advance();
                render_dt = (render_dt + 1) % 25;
                if (render_dt == 0) {
                    exporter.exportParticles(mpmsolver.getParticles());
                    captured_frame++;
                }
            }

            glViewport(0, 0, 1024, 768);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            // std::cout << "Advanced one step!" << std::endl;

            renderer.renderFrame(mpmsolver, cam);
            // renderer.renderParticles(mpmsolver.getParticles());
            glfwSwapBuffers(window);
            glfwPollEvents();

            if (captured_frame >= 168)
                break;
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    return 0;
}

// ==================================================
// callbacks
// ==================================================

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static double lastX = -1.0;
    static double lastY = -1.0;

    if (lastX < 0.f)
    {
        lastX = xpos;
        lastY = ypos;
    }

    float xoffset = (xpos - lastX) * 0.1f;
    float yoffset = (lastY - ypos) * 0.1f; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    cam.setYawandPitch(cam.getYaw() + xoffset, cam.getPitch() + yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.setZoom(cam.getZoom() + yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float speed = 3.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.setPosition(cam.getPosition() + cam.getFront() * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.setPosition(cam.getPosition() - cam.getFront() * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.setPosition(cam.getPosition() - cam.getRight() * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.setPosition(cam.getPosition() + cam.getRight() * speed);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        pause = true;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        pause = false;
}