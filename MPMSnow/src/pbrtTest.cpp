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
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwSwapInterval(1);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        Shader pbrt("./Shader/pbrVertex.glsl", "./Shader/iblpbrFragment.glsl");
        pbrt.use();
        pbrt.setInt("irradianceMap", 0);
        SphereShape sphere;
        Camera cam;
        cam.setPosition(glm::vec3(0.f, 0.f, 3.f));
        cam.lookAt(glm::vec3(0.f));
        Material mat(glm::vec3(0.0f, 0.5f, 0.0f), 0.8f, 0.5f, 1.0f);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = cam.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.f / 768.f, 0.1f, 100.0f);

        Skybox cubemap("./resources/snow_env.hdr", window);

        Shader background("./Shader/background.vs", "./Shader/background.fs");
        background.use();
        background.setInt("environmentMap", 0);
        
        CubeShape cube;

        while (!glfwWindowShouldClose(window)) {
            glViewport(0, 0, 1024, 768);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            // std::cout << "Advanced one step!" << std::endl;

            background.use();
            background.setMat4("projection", projection);
            background.setMat4("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getCubeMap());

            cube.draw();

            pbrt.use();
            // set light
            pbrt.setVec3("lightColor", glm::vec3(300.f, 300.f, 300.f));
            pbrt.setVec3("lightDirection", glm::vec3(1.0f, 1.0f, 1.0f));
            pbrt.setMat4("projection", projection);
            pbrt.setMat4("view", view);
            pbrt.setMat4("model", model);
            pbrt.setVec3("camPos", cam.getPosition());
            pbrt.setMaterial(mat);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getCubeMap());

            sphere.draw();

            // renderer.renderParticles(mpmsolver.getParticles());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    return 0;
}