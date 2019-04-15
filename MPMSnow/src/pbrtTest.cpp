//===========================================================
// This file is written to familier with Eigen and further experiment MPM
//===========================================================

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "common/util.h"
#include "common/math.h"

#include "renderer/shader.h"
#include "renderer/sphere.h"
#include "renderer/cube.h"
#include "renderer/material.h"
#include "renderer/camera.h"
#include "renderer/skybox.h"

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

        GLFWwindow* window = glfwCreateWindow(1024, 768, "PBRT", NULL, NULL);
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

        // pbrt.setInt("irradianceMap", 0);
        SphereShape sphere;
        cam.setPosition(glm::vec3(0.f, 0.f, 20.f));

        // 4 lights
        glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
        };
        glm::vec3 lightColors[] = {
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f)
        };

        // the square of balls
        int rows = 7;
        int cols = 7;
        float spacing = 2.5f;

        Skybox cubemap("./resources/newport_loft.hdr", window);
        Shader background("./Shader/background.vs", "./Shader/background.fs");
        background.use();
        background.setInt("environmentMap", 0);
        // pbr shader
        Shader pbrt("./Shader/pbrVertex.glsl", "./Shader/pbrFullFragment.glsl");
        pbrt.use();
        pbrt.setInt("irradianceMap", 0);
        pbrt.setInt("prefilterMap", 1);
        pbrt.setInt("brdfLUT", 2);
        //pbrt.setVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
        //pbrt.setFloat("ao", 1.0f);
        
        CubeShape cube;

        // ====================================================
        // render loop
        // ====================================================
        while (!glfwWindowShouldClose(window)) {
            float current = glfwGetTime();
            deltaTime = current - lastTime;
            lastTime = current;

            processInput(window);

            glViewport(0, 0, 1024, 768);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            // std::cout << "Advanced one step!" << std::endl;

            glm::mat4 view = cam.getViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(cam.getZoom()), 1024.f / 768.f, 0.1f, 100.0f);

            background.use();
            background.setMat4("projection", projection);
            background.setMat4("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getCubeMap());

            cube.draw();

            pbrt.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getIrradianceMap());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getPrefilterMap());
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cubemap.getBRDFLUT());
            // light moving
            for (uint i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
                glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
                // newPos = lightPositions[i];
                pbrt.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
                pbrt.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
            }

            pbrt.setMat4("projection", projection);
            pbrt.setMat4("view", view);
            pbrt.setVec3("camPos", cam.getPosition());

            for (int r = 0; r < rows; ++r) {
                //pbrt.setFloat("metallic", (float)r / (float)rows);
                for (int c = 0; c < cols; ++c) {
                    //pbrt.setFloat("roughness", glm::clamp((float)c / (float)cols, 0.05f, 1.0f));
                    Material mat(glm::vec3(1.0f, 0.71f, 0.0f), glm::clamp((float)c / (float)cols, 0.05f, 1.0f),
                        ((float)r / (float)rows), 1.0f);

                    pbrt.setMaterial(mat);

                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(
                        (c - (cols / 2.0f)) * spacing,
                        (r - (rows / 2.0f)) * spacing,
                        0.0f
                    ));
                    pbrt.setMat4("model", model);
                    // set lights
                    //glActiveTexture(GL_TEXTURE0);
                    //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getCubeMap());

                    sphere.draw();

                }
            }

            // renderer.renderParticles(mpmsolver.getParticles());
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