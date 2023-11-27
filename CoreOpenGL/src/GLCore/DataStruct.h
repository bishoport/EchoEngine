#pragma once
#include "../glpch.h"

namespace GLCore 
{
    enum MODEL_TYPES
    {
        NONE,
        PRIMIVITE_PLANE,
        PRIMIVITE_CUBE,
        PRIMIVITE_SEGMENTED_CUBE,
        PRIMIVITE_SPHERE,
        PRIMIVITE_QUAD,
        EXTERNAL_FILE,
    };

    inline std::string ModelTypeToString(MODEL_TYPES type) {
        switch (type) {
        case NONE: return "NONE";
        case PRIMIVITE_PLANE: return "PRIMIVITE_PLANE";
        case PRIMIVITE_CUBE: return "PRIMIVITE_CUBE";
        case PRIMIVITE_SEGMENTED_CUBE: return "PRIMIVITE_SEGMENTED_CUBE";
        case PRIMIVITE_SPHERE: return "PRIMIVITE_SPHERE";
        case PRIMIVITE_QUAD: return "PRIMIVITE_QUAD";
        case EXTERNAL_FILE: return "EXTERNAL_FILE";
        default: return "Unknown Type";
        }
    }


    enum TEXTURE_TYPES {
        ALBEDO,
        NORMAL,
        METALLIC,
        ROUGHNESS,
        AO
    };

    struct FBO_Data {
        GLuint* FBO = nullptr;
        GLuint* depthBuffer = nullptr;
        std::vector<GLuint> colorBuffers;
        ImVec2 drawPos = ImVec2(0.0f, 0.0f);
        ImVec2 drawSize = ImVec2(640.0f, 480.0f);
    };

    // Definir un struct para las opciones de importación
    struct ImportOptions {
        std::string filePath;
        std::string fileName;
        int modelID;
        bool invertUV;
        bool rotate90;
    };

    //LA VERSION EN RAM
    struct Image {
        unsigned char* pixels;
        int width, height, channels;
        std::string path = "NONE";
    };




    //LA VERSION EN GPU
    struct Texture {
        GLuint textureID = 0;
        TEXTURE_TYPES type;
        std::string typeString;
        Ref<Image> image;
        bool hasMap = false;

        void Bind() {
            if (hasMap) {

                if (textureID != 0) {
                    glDeleteTextures(1, &textureID);
                }

                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_2D, textureID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format{};
                if (image->channels == 1)
                    format = GL_RED;
                else if (image->channels == 3)
                    format = GL_RGB;
                else if (image->channels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->pixels);
                glGenerateMipmap(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    };





    struct MaterialData
    {
        //ALBEDO COLOR
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

        //TEXTURES
        Ref<Texture> albedoMap;
        Ref<Texture> normalMap;
        Ref<Texture> metallicMap;
        Ref<Texture> rougnessMap;
        Ref<Texture> aOMap;
        //-------------------------------------------------------
    };





    struct MeshData {

        // Constructor predeterminado
        MeshData() = default;

        std::string meshName;

        std::vector<GLfloat> vertexBuffer;
        std::vector<GLfloat> texcoords;
        std::vector<GLfloat> normals;
        std::vector<GLfloat> tangents;
        std::vector<GLfloat> bitangents;

        std::vector<GLuint> indices;
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        unsigned int indexCount;

        glm::vec3 meshLocalPosition = glm::vec3(0.0f);;
        glm::vec3 meshPosition;
        glm::vec3 meshRotation; // En ángulos de Euler
        glm::vec3 meshScale;

        glm::vec3 minBounds; // Esquina inferior delantero izquierda de la bounding box
        glm::vec3 maxBounds; // Esquina superior trasero derecha de la bounding box
        GLuint VBO_BB, VAO_BB, EBO_BB;

        void PrepareAABB()
        {
            //-----------------------------------PREPARE BOUNDING BOX LOCAL-----------------------------------
            glCreateVertexArrays(1, &VAO_BB);
            glCreateBuffers(1, &VBO_BB);
            glCreateBuffers(1, &EBO_BB);  // Crear EBO

            glVertexArrayVertexBuffer(VAO_BB, 0, VBO_BB, 0, 3 * sizeof(GLfloat));

            glm::vec3 min = minBounds;
            glm::vec3 max = maxBounds;

            glm::vec3 vertices[8] = {
                {min.x, min.y, min.z}, {max.x, min.y, min.z},
                {max.x, max.y, min.z}, {min.x, max.y, min.z},
                {min.x, min.y, max.z}, {max.x, min.y, max.z},
                {max.x, max.y, max.z}, {min.x, max.y, max.z}
            };

            glNamedBufferStorage(VBO_BB, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);  // Llenar VBO

            // Definir índices para las líneas de la Bounding Box
            GLuint indices[24] = {
                0, 1, 1, 2, 2, 3, 3, 0,  // base inferior
                4, 5, 5, 6, 6, 7, 7, 4,  // base superior
                0, 4, 1, 5, 2, 6, 3, 7   // aristas laterales
            };

            glNamedBufferStorage(EBO_BB, sizeof(indices), indices, GL_DYNAMIC_STORAGE_BIT);  // Llenar EBO

            // Asociar EBO con VAO
            glVertexArrayElementBuffer(VAO_BB, EBO_BB);

            glEnableVertexArrayAttrib(VAO_BB, 0);
            glVertexArrayAttribFormat(VAO_BB, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(VAO_BB, 0, 0);
            //-------------------------------------------------------------------------------------------------
        }
        void UpdateAABB(const glm::mat4& modelMatrix)
        {
            glm::vec3 originalVertices[8] = {
                {minBounds.x, minBounds.y, minBounds.z}, {maxBounds.x, minBounds.y, minBounds.z},
                {maxBounds.x, maxBounds.y, minBounds.z}, {minBounds.x, maxBounds.y, minBounds.z},
                {minBounds.x, minBounds.y, maxBounds.z}, {maxBounds.x, minBounds.y, maxBounds.z},
                {maxBounds.x, maxBounds.y, maxBounds.z}, {minBounds.x, maxBounds.y, maxBounds.z}
            };

            glm::vec3 transformedVertices[8];

            for (int i = 0; i < 8; ++i) {
                glm::vec4 updatedVertex = modelMatrix * glm::vec4(originalVertices[i], 1.0f);
                transformedVertices[i] = glm::vec3(updatedVertex);
            }

            glNamedBufferSubData(VBO_BB, 0, sizeof(transformedVertices), transformedVertices);
        }

    };

    struct ModelInfo
    {
        // Constructor predeterminado
        ModelInfo() = default;

        Ref<GLCore::MeshData> meshData;
        Ref<GLCore::MaterialData> model_textures;

        MODEL_TYPES modelType;
    };

    struct ModelParent
    {
        std::string name;
        std::vector<ModelInfo> modelInfos;
    };
}

