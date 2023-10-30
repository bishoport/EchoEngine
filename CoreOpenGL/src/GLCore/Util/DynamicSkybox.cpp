#include "DynamicSkybox.h"
#include "IMGLoader.h"


namespace GLCore::Utils
{
    DynamicSkybox::DynamicSkybox(const std::vector<const char*> faces)  : m_SunPosition(0.0f, 1.0f, 0.0f)
    {
        // cube VAO
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        // skybox VAO
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        cubemapTexture = GLCore::Utils::ImageLoader::loadImagesForCubemap(faces);

    }



    DynamicSkybox::~DynamicSkybox() {
        glDeleteVertexArrays(1, &skyboxVAO);
        glDeleteBuffers(1, &skyboxVBO);
    }


    void DynamicSkybox::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 sunPosition) 
    {
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->use();

        // Elimina la traslación de la matriz de vista
        viewMatrix = glm::mat4(glm::mat3(viewMatrix)); // Elimina la traslación, manteniendo solo la rotación

        // Escala la matriz de vista para hacer el skybox más grande
        float scale = 1.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
        viewMatrix = glm::scale(viewMatrix, glm::vec3(scale, scale, scale));

        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setMat4("view", viewMatrix);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setMat4("projection", projectionMatrix);

        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setVec3("sunPosition", sunPosition);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setVec3("sunDiskSize", m_sunDiskSize);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setFloat("gradientIntensity", m_gradientIntensity);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setFloat("uAuraSize", auraSize);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setFloat("uAuraIntensity", auraIntensity);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setFloat("uEdgeSoftness", edgeSoftness);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setVec3("uDayLightColor", dayLightColor);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setVec3("uSunsetColor", sunsetColor);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setVec3("uDayNightColor", dayNightColor);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setVec3("uGroundColor", groundColor);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setFloat("uUpperBound", upperBound);
        GLCore::Render::ShaderManager::Get("dynamicSkybox")->setFloat("uLowerBound", lowerBound);
        


        // skybox cube
        //glDepthMask(GL_FALSE);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS); // set depth function back to default
        //glDepthMask(GL_TRUE);
    }

    


}
