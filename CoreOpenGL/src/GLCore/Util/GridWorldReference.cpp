#include "GridWorldReference.h"

namespace GLCore::Utils
{

    GridWorldReference::GridWorldReference()
    {
        //-----------------------------------------------GRID----------------------------------------
        std::vector<glm::vec3> gridVertices;
        std::vector<glm::uvec4> gridIndices;

        //Vertical lines
        for (int j = 0; j <= slices; ++j) {
            for (int i = 0; i <= slices; ++i) {
                float x = (float)i / (float)slices * sizeMultiplicator;
                float y = 0;
                float z = (float)j / (float)slices * sizeMultiplicator;
                gridVertices.push_back(glm::vec3(x - sizeMultiplicator / 2, y, z - sizeMultiplicator / 2));
            }
        }

        //Horizontal lines
        for (int j = 0; j < slices; ++j) {
            for (int i = 0; i < slices; ++i) {

                int row1 = j * (slices + 1);
                int row2 = (j + 1) * (slices + 1);

                gridIndices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
                gridIndices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));
            }
        }

        glGenVertexArrays(1, &gridVAO);
        glBindVertexArray(gridVAO);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(glm::vec3), glm::value_ptr(gridVertices[0]), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        GLuint ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices.size() * sizeof(glm::uvec4), glm::value_ptr(gridIndices[0]), GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        lenght = (GLuint)gridIndices.size() * 4;
        //-------------------------------------------------------------------------------------------------------



        //-----------------------------------------------WORLD AXES----------------------------------------
        // Ahora generamos los ejes
        std::vector<float> axesVertices = {
            // Posiciones            // Colores
            0.0f, 0.0f, 0.0f,        1.0f, 0.0f, 0.0f, // Origen hacia X (rojo)
            1.0f, 0.0f, 0.0f,        1.0f, 0.0f, 0.0f, // Eje X
            0.0f, 0.0f, 0.0f,        0.0f, 1.0f, 0.0f, // Origen hacia Y (verde)
            0.0f, 1.0f, 0.0f,        0.0f, 1.0f, 0.0f, // Eje Y
            0.0f, 0.0f, 0.0f,        0.0f, 0.0f, 1.0f, // Origen hacia Z (azul)
            0.0f, 0.0f, 1.0f,        0.0f, 0.0f, 1.0f  // Eje Z
        };

        glGenVertexArrays(1, &axesVAO);
        glBindVertexArray(axesVAO);

        GLuint axesVBO;
        glGenBuffers(1, &axesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
        glBufferData(GL_ARRAY_BUFFER, axesVertices.size() * sizeof(float), &axesVertices[0], GL_STATIC_DRAW);

        // Posiciones
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        // Colores
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //-------------------------------------------------------------------------------------------------------
    }

    GridWorldReference::~GridWorldReference()
    {
        glDeleteVertexArrays(1, &gridVAO);
        glDeleteVertexArrays(1, &axesVAO);
    }

    void GridWorldReference::Render()
    {
        if (showGrid)
        {
            //-----------------------------------------------GRID----------------------------------------
            glBindVertexArray(gridVAO);

            glm::mat4 transform{ glm::mat4(1.0f) };

            /* Apply object's transformation matrix */

            GLCore::Render::ShaderManager::Get("debug")->use();
            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", transform);
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(0.8, 0.8, 0.8, 1.0));


            glLineWidth(3);
            glDrawElements(GL_LINES, lenght, GL_UNSIGNED_INT, NULL);
            glBindVertexArray(0);
            //-------------------------------------------------------------------------------------------------------



            //-----------------------------------------------WORLD AXIS----------------------------------------
            glBindVertexArray(axesVAO);

            glm::mat4 transformAxes = glm::translate(glm::mat4(1.0f), glm::vec3(-sizeMultiplicator / 2, 1.0f, -sizeMultiplicator / 2)); // traslación a la esquina de la rejilla

            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", transformAxes);
            glLineWidth(5);

            // Dibuja eje X (rojo)
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(1.0, 0.0, 0.0, 1.0)); // Rojo
            glDrawArrays(GL_LINES, 0, 2); // Solo dibuja las primeras 2 posiciones (Origen -> X)

            // Dibuja eje Y (verde)
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(0.0, 1.0, 0.0, 1.0)); // Verde
            glDrawArrays(GL_LINES, 2, 2); // Dibuja las siguientes 2 posiciones (Origen -> Y)

            // Dibuja eje Z (azul)
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(0.0, 0.0, 1.0, 1.0)); // Azul
            glDrawArrays(GL_LINES, 4, 2); // Dibuja las últimas 2 posiciones (Origen -> Z)

            glBindVertexArray(0);
            //-------------------------------------------------------------------------------------------------------
        }
    }
}
