#pragma once

#include "../../glpch.h"

namespace GLCore::Utils
{

    class DynamicSkybox {
    public:
        DynamicSkybox(const std::vector<const char*> faces);
        ~DynamicSkybox();

        void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 sunPosition);

        float m_gradientIntensity = 1.0f;
        glm::vec3 m_sunDiskSize = glm::vec3(0.15f, 0.15f, 0.15f);

        float auraSize = 0.02f;
        float auraIntensity =0.5f;
        float edgeSoftness =0.5f;

        glm::vec3 dayLightColor = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 sunsetColor = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 dayNightColor = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 groundColor = glm::vec3(0.0f, 0.0f, 0.0f);

        float lowerBound = -0.05;
        float upperBound = 0.05;

    private:
        unsigned int cubeVAO, cubeVBO;
        GLuint skyboxVAO, skyboxVBO;
        glm::vec3 m_SunPosition;

        GLuint cubemapTexture;
        
        float cubeSize = 100.0f;
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        std::array<float, 180> cubeVertices = {
            // positions               // texture Coords
            -cubeSize, -cubeSize, -cubeSize,       0.0f, 0.0f,
             cubeSize, -cubeSize, -cubeSize,       1.0f, 0.0f,
             cubeSize,  cubeSize, -cubeSize,       1.0f, 1.0f,
             cubeSize,  cubeSize, -cubeSize,       1.0f, 1.0f,
            -cubeSize,  cubeSize, -cubeSize,       0.0f, 1.0f,
            -cubeSize, -cubeSize, -cubeSize,       0.0f, 0.0f,
                                       
            -cubeSize, -cubeSize,  cubeSize,       0.0f, 0.0f,
             cubeSize, -cubeSize,  cubeSize,       1.0f, 0.0f,
             cubeSize,  cubeSize,  cubeSize,       1.0f, 1.0f,
             cubeSize,  cubeSize,  cubeSize,       1.0f, 1.0f,
            -cubeSize,  cubeSize,  cubeSize,       0.0f, 1.0f,
            -cubeSize, -cubeSize,  cubeSize,       0.0f, 0.0f,
                                       
            -cubeSize,  cubeSize,  cubeSize,       1.0f, 0.0f,
            -cubeSize,  cubeSize, -cubeSize,       1.0f, 1.0f,
            -cubeSize, -cubeSize, -cubeSize,       0.0f, 1.0f,
            -cubeSize, -cubeSize, -cubeSize,       0.0f, 1.0f,
            -cubeSize, -cubeSize,  cubeSize,       0.0f, 0.0f,
            -cubeSize,  cubeSize,  cubeSize,       1.0f, 0.0f,
                                       
             cubeSize,  cubeSize,  cubeSize,       1.0f, 0.0f,
             cubeSize,  cubeSize, -cubeSize,       1.0f, 1.0f,
             cubeSize, -cubeSize, -cubeSize,       0.0f, 1.0f,
             cubeSize, -cubeSize, -cubeSize,       0.0f, 1.0f,
             cubeSize, -cubeSize,  cubeSize,       0.0f, 0.0f,
             cubeSize,  cubeSize,  cubeSize,       1.0f, 0.0f,
                                       
            -cubeSize, -cubeSize, -cubeSize,       0.0f, 1.0f,
             cubeSize, -cubeSize, -cubeSize,       1.0f, 1.0f,
             cubeSize, -cubeSize,  cubeSize,       1.0f, 0.0f,
             cubeSize, -cubeSize,  cubeSize,       1.0f, 0.0f,
            -cubeSize, -cubeSize,  cubeSize,       0.0f, 0.0f,
            -cubeSize, -cubeSize, -cubeSize,       0.0f, 1.0f,
                                       
            -cubeSize,  cubeSize, -cubeSize,       0.0f, 1.0f,
             cubeSize,  cubeSize, -cubeSize,       1.0f, 1.0f,
             cubeSize,  cubeSize,  cubeSize,       1.0f, 0.0f,
             cubeSize,  cubeSize,  cubeSize,       1.0f, 0.0f,
            -cubeSize,  cubeSize,  cubeSize,       0.0f, 0.0f,
            -cubeSize,  cubeSize, -cubeSize,       0.0f, 1.0f
        };

        std::array<float, 108> skyboxVertices = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
    };
}