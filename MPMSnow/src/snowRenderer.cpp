//===========================================================
// This file is written to familier with Eigen and further experiment MPM
//===========================================================

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "common/util.h"
#include "common/math.h"

#include "renderer/shader.h"
#include <renderer/quad.h>
#include "renderer/cube.h"
#include "renderer/material.h"
#include "renderer/camera.h"
#include "renderer/skybox.h"
#include <renderer/volumerenderer.h>
#include <mpm/mpm.h>

using namespace G_NAMESPACE_NAME;

// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// camera is global in order to access it from this whole script
Camera cam;
// time is used everywhere
float deltaTime;
float lastTime;

int main()
{
    // pbrt tester
    if (glfwInit())
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        GLFWwindow* window = glfwCreateWindow(1024, 768, "SnowRender", NULL, NULL);
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
        // depth test and set lequal to draw cubemap box.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // seamless cubemap
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        cam.setPosition(glm::vec3(0.f, 0.f, 20.f));

        VolumeRenderer vr(1024, 768);
        MPM mpm(200);
        mpm.addObject(Vector3(0.1, 0.1, 0.1));
        Quad quad;
        CubeShape cube;

        Shader billboard("./Shader/billboard.vs", "./Shader/billboard.fs");
        billboard.use();
        billboard.setInt("startTexture", 0);
        billboard.setInt("endTexture", 1);

        // ====================================================
        // render loop
        // ====================================================
        while (!glfwWindowShouldClose(window)) {
            float current = glfwGetTime();
            deltaTime = current - lastTime;
            lastTime = current;

            processInput(window);

            glViewport(0, 0, 1024, 768);
            glClearColor(1.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            vr.RenderSnow(mpm.getParticles(), cam);

            glm::mat4 view = cam.getViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(cam.getZoom()), 1024.f / 768.f, 0.1f, 100.0f);
            billboard.use();
            billboard.setMat4("view", view);
            billboard.setMat4("projection", projection);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
            billboard.setMat4("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, vr.getStartMap());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, vr.getEndMap());
            quad.draw();

            glfwSwapBuffers(window);
            glfwPollEvents();
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
}