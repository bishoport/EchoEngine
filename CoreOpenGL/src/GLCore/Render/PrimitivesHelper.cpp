// PrimitivesHelper.cpp
#include "PrimitivesHelper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GLCore::Render {
    #define M_PI 3.14159265358979323846

    void PrimitivesHelper::GenerateBuffers(MeshData& meshData) {
        glCreateVertexArrays(1, &meshData.VAO);

        glCreateBuffers(1, &meshData.VBO);
        glNamedBufferStorage(meshData.VBO, meshData.vertexBuffer.size() * sizeof(GLfloat), meshData.vertexBuffer.data(), GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &meshData.EBO);
        glNamedBufferStorage(meshData.EBO, meshData.indices.size() * sizeof(GLuint), meshData.indices.data(), GL_DYNAMIC_STORAGE_BIT);  
    }

    void PrimitivesHelper::SetupMeshAttributes(MeshData& meshData)
    {
        //DSA
        //Definiendo los atributos del VAO
        glEnableVertexArrayAttrib(meshData.VAO, 0);
        glVertexArrayAttribFormat(meshData.VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(meshData.VAO, 0, 0);

        glEnableVertexArrayAttrib(meshData.VAO, 1);
        glVertexArrayAttribFormat(meshData.VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
        glVertexArrayAttribBinding(meshData.VAO, 1, 0);

        glEnableVertexArrayAttrib(meshData.VAO, 2);
        glVertexArrayAttribFormat(meshData.VAO, 2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat));
        glVertexArrayAttribBinding(meshData.VAO, 2, 0);

        // Enlazando el búfer de vértices al VAO
        glVertexArrayVertexBuffer(meshData.VAO, 0, meshData.VBO, 0, 8 * sizeof(GLfloat));
        glVertexArrayElementBuffer(meshData.VAO, meshData.EBO);



        //// Generando y enlazando VAO, VBO, EBO  OLD METHOD
        //glBindVertexArray(meshData.VAO);

        //glBindBuffer(GL_ARRAY_BUFFER, meshData.VBO);
        //glBufferData(GL_ARRAY_BUFFER, meshData.vertexBuffer.size() * sizeof(GLfloat), meshData.vertexBuffer.data(), GL_STATIC_DRAW);

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indices.size() * sizeof(GLuint), meshData.indices.data(), GL_STATIC_DRAW);

        //// Posición
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        //glEnableVertexAttribArray(0);

        //// Coordenadas de textura
        //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        //glEnableVertexAttribArray(1);

        //// Normales
        //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
        //glEnableVertexAttribArray(2);

        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArray(0);
    }

    GLCore::Render::PrimitivesHelper::BoundingBox PrimitivesHelper::CalculateBoundingBox(const std::vector<float>& vertices)
    {
        BoundingBox bbox;

        // Inicializamos min y max con valores extremos
        bbox.min = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        bbox.max = { -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };

        // Asumimos que cada vértice tiene 8 componentes (3 para posición, 2 para textura, 3 para normal)
        const int stride = 8;

        // Iteramos sobre cada vértice
        for (size_t i = 0; i < vertices.size(); i += stride) {
            glm::vec3 vertex = { vertices[i], vertices[i + 1], vertices[i + 2] };

            // Actualizamos el mínimo y máximo
            bbox.min.x = std::min(bbox.min.x, vertex.x);
            bbox.min.y = std::min(bbox.min.y, vertex.y);
            bbox.min.z = std::min(bbox.min.z, vertex.z);

            bbox.max.x = std::max(bbox.max.x, vertex.x);
            bbox.max.y = std::max(bbox.max.y, vertex.y);
            bbox.max.z = std::max(bbox.max.z, vertex.z);
        }

        return bbox;
    }

    GLCore::MeshData PrimitivesHelper::CreateQuad() {

        GLCore::MeshData meshData;

        std::vector<GLfloat> vertexBuffer = {
             // positions        // texture Coords      // normals
            -0.5f,  0.5f, 0.0f,     0.0f, 0.0f,       0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,       0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.0f,     1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f,     1.0f, 0.0f,       0.0f, 0.0f, 1.0f,
        };

        std::vector<GLuint> indices = {
            0, 1, 2,
            1, 3, 2
        };

        meshData.vertexBuffer = vertexBuffer;
        meshData.indices = indices;

        meshData.indexCount = indices.size();

        //--------------------CALCULATE AABB
        std::vector<float> posiciones;
        for (size_t i = 0; i < meshData.vertexBuffer.size(); i += 8) {
            posiciones.push_back(meshData.vertexBuffer[i]);
            posiciones.push_back(meshData.vertexBuffer[i + 1]);
            posiciones.push_back(meshData.vertexBuffer[i + 2]);
        }
        BoundingBox bbox = CalculateBoundingBox(posiciones);

        //bbox.min.x = -0.01f; // Establecer una altura mínima
        //bbox.max.y = 0.01f;  // Establecer una altura máxima

        meshData.minBounds = bbox.min;
        meshData.maxBounds = bbox.max;
        //-------------------------------------------------------------------


        return meshData;
    }

    GLCore::MeshData PrimitivesHelper::CreatePlane()
    {
        GLCore::MeshData meshData;

        const int divisions = 10;
        const float size = 10.0f; // tamaño del plano completo
        const float step = size / divisions;
        const float halfSize = size * 0.5f;

        for (int i = 0; i <= divisions; i++) {
            for (int j = 0; j <= divisions; j++) {
                // Posiciones
                meshData.vertexBuffer.push_back(j * step - halfSize); // x
                meshData.vertexBuffer.push_back(0.0f);                // y
                meshData.vertexBuffer.push_back(i * step - halfSize); // z

                // Coordenadas de textura
                meshData.vertexBuffer.push_back((float)j / divisions);  // u
                meshData.vertexBuffer.push_back((float)i / divisions);  // v

                // Normales
                meshData.vertexBuffer.push_back(0.0f);
                meshData.vertexBuffer.push_back(1.0f);
                meshData.vertexBuffer.push_back(0.0f);
            }
        }

        for (int i = 0; i < divisions; i++) {
            for (int j = 0; j < divisions; j++) {
                GLuint topLeft = i * (divisions + 1) + j;
                GLuint topRight = topLeft + 1;
                GLuint bottomLeft = (i + 1) * (divisions + 1) + j;
                GLuint bottomRight = bottomLeft + 1;

                meshData.indices.push_back(topLeft);
                meshData.indices.push_back(bottomLeft);
                meshData.indices.push_back(topRight);

                meshData.indices.push_back(topRight);
                meshData.indices.push_back(bottomLeft);
                meshData.indices.push_back(bottomRight);
            }
        }

        meshData.indexCount = meshData.indices.size();


        //--------------------CALCULATE AABB
        std::vector<float> posiciones;
        for (size_t i = 0; i < meshData.vertexBuffer.size(); i += 8) {
            posiciones.push_back(meshData.vertexBuffer[i]);
            posiciones.push_back(meshData.vertexBuffer[i + 1]);
            posiciones.push_back(meshData.vertexBuffer[i + 2]);
        }
        BoundingBox bbox = CalculateBoundingBox(posiciones);

        bbox.min.y = -0.01f; // Establecer una altura mínima
        bbox.max.y = 0.01f;  // Establecer una altura máxima

        meshData.minBounds = bbox.min;
        meshData.maxBounds = bbox.max;
        //-------------------------------------------------------------------

        return meshData;
    }

    GLCore::MeshData PrimitivesHelper::CreateCube() {

        GLCore::MeshData meshData;

        // Definiendo los 8 vértices del cubo (posición, coordenada de textura y normales)
        std::vector<float> vertexBuffer = {
           // Posiciones      // Coordenadas de Textura        // Normales
            -0.5f,  0.5f, -0.5f,           0, 0,               0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,           0, 1,               0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,           1, 1,               0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,           1, 0,               0.0f,  0.0f, -1.0f,
                                                             
            -0.5f,  0.5f,  0.5f,           0, 0,               0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,           0, 1,               0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,           1, 1,               0.0f,  0.0f,  1.0f,
             0.5f,  0.5f,  0.5f,           1, 0,               0.0f,  0.0f,  1.0f,
                                                             
             0.5f,  0.5f, -0.5f,           1, 0,               1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,           1, 1,               1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,           0, 1,               1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,           0, 0,               1.0f,  0.0f,  0.0f,
                                                             
            -0.5f,  0.5f, -0.5f,           1, 0,              -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,           1, 1,              -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,           0, 1,              -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,           0, 0,              -1.0f,  0.0f,  0.0f,
                                                             
            -0.5f,  0.5f,  0.5f,           0, 0,               0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,           0, 1,               0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,           1, 1,               0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,           1, 0,               0.0f,  1.0f,  0.0f,
                                                             
            -0.5f, -0.5f,  0.5f,           0, 0,               0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,           0, 1,               0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,           1, 1,               0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,           1, 0,               0.0f, -1.0f,  0.0f
        };

        


        // Definiendo los índices de los triángulos
        std::vector<unsigned int> indices = {
             0,  1,  3,   3,  1,  2,
             4,  5,  7,   7,  5,  6,
             8,  9, 11,  11,  9, 10,
            12, 13, 15,  15, 13, 14,
            16, 17, 19,  19, 17, 18,
            20, 21, 23,  23, 21, 22
        };
        meshData.vertexBuffer = vertexBuffer;
        meshData.indices = indices;

        meshData.indexCount = indices.size();

        //--------------------CALCULATE AABB
        std::vector<float> posiciones;
        for (size_t i = 0; i < vertexBuffer.size(); i += 8) {
            posiciones.push_back(vertexBuffer[i]);
            posiciones.push_back(vertexBuffer[i + 1]);
            posiciones.push_back(vertexBuffer[i + 2]);
        }
        BoundingBox bbox = CalculateBoundingBox(posiciones);

        meshData.minBounds = bbox.min;
        meshData.maxBounds = bbox.max;
        //-------------------------------------------------------------------

        return meshData;
    }

    GLCore::MeshData PrimitivesHelper::CreateSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {

        GLCore::MeshData meshData;

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;

        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= sectorCount; ++x)
        {
            for (unsigned int y = 0; y <= stackCount; ++y)
            {
                float xSegment = (float)x / (float)sectorCount;
                float ySegment = (float)y / (float)stackCount;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(radius * glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        for (unsigned int y = 0; y < stackCount; ++y)
        {
            for (unsigned int x = 0; x <= sectorCount; ++x)
            {
                meshData.indices.push_back(y * (sectorCount + 1) + x);
                meshData.indices.push_back((y + 1) * (sectorCount + 1) + x);
                meshData.indices.push_back((y + 1) * (sectorCount + 1) + x + 1);

                meshData.indices.push_back(y * (sectorCount + 1) + x);
                meshData.indices.push_back((y + 1) * (sectorCount + 1) + x + 1);
                meshData.indices.push_back(y * (sectorCount + 1) + x + 1);
            }
        }

        meshData.indexCount = meshData.indices.size();

        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            meshData.vertexBuffer.push_back(positions[i].x);
            meshData.vertexBuffer.push_back(positions[i].y);
            meshData.vertexBuffer.push_back(positions[i].z);

            if (uv.size() > 0)
            {
                meshData.vertexBuffer.push_back(uv[i].x);
                meshData.vertexBuffer.push_back(uv[i].y);
            }

            if (normals.size() > 0)
            {
                meshData.vertexBuffer.push_back(normals[i].x);
                meshData.vertexBuffer.push_back(normals[i].y);
                meshData.vertexBuffer.push_back(normals[i].z);
            }
        }


        //--------------------CALCULATE AABB
        std::vector<float> posiciones;
        for (size_t i = 0; i < meshData.vertexBuffer.size(); i += 8) {
            posiciones.push_back(meshData.vertexBuffer[i]);
            posiciones.push_back(meshData.vertexBuffer[i + 1]);
            posiciones.push_back(meshData.vertexBuffer[i + 2]);
        }
        BoundingBox bbox = CalculateBoundingBox(posiciones);

        meshData.minBounds = bbox.min;
        meshData.maxBounds = bbox.max;
        //-------------------------------------------------------------------

        return meshData;
    }

}