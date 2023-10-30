#include "Skybox.h"
#include "IMGLoader.h"

namespace GLCore::Utils
{
    Skybox::Skybox(const std::vector<const char*> faces)
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

    Skybox::~Skybox()
    {
    }

    void Skybox::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
    {
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        GLCore::Render::ShaderManager::Get("skybox")->use();

        // Elimina la traslación de la matriz de vista
        viewMatrix = glm::mat4(glm::mat3(viewMatrix)); // Elimina la traslación, manteniendo solo la rotación

        // Escala la matriz de vista para hacer el skybox más grande
        float scale = 10.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
        viewMatrix = glm::scale(viewMatrix, glm::vec3(scale, scale, scale));

        GLCore::Render::ShaderManager::Get("skybox")->setMat4("view", viewMatrix);
        GLCore::Render::ShaderManager::Get("skybox")->setMat4("projection", projectionMatrix);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }
}