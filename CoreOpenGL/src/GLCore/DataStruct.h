#pragma once
#include "../glpch.h"

namespace GLCore 
{
    enum TEXTURE_TYPES {
        ALBEDO,
        NORMAL,
        METALLIC,
        ROUGHNESS,
        AO
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
        Image image;
        bool hasMap = false;
    };

    struct Material {

        float shininess;
        float hdrMultiply = 0.0f;
        float hdrIntensity = 0.3f;
        float exposure = 1.0f;
        float gamma = 2.2f;
        //-------------------------------------------------------

        std::vector<Texture*> textures;

        Texture albedoMap;
        Texture normalMap;
        Texture metallicMap;
        Texture rougnessMap;
        Texture aOMap;

        //VALUES
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float normalIntensity = 0.5f;
        float metallicValue = 0.0f;
        float roughnessValue = 0.05f;
        float reflectanceValue = 0.04f;
        float fresnelCoefValue = 5.0f;
        //-------------------------------------------------------


        void loadTextureData(Texture& map) {
            if (map.hasMap) {

                if (map.textureID != 0) {
                    glDeleteTextures(1, &map.textureID);
                }

                glGenTextures(1, &map.textureID);
                glBindTexture(GL_TEXTURE_2D, map.textureID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format{};
                if (map.image.channels == 1)
                    format = GL_RED;
                else if (map.image.channels == 3)
                    format = GL_RGB;
                else if (map.image.channels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, map.image.width, map.image.height, 0, format, GL_UNSIGNED_BYTE, map.image.pixels);

                glGenerateMipmap(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }


        void prepare_PBRMaterials()
        {
            if (textures.size() == 0)
            {
                albedoMap.typeString = "ALBEDO";
                normalMap.typeString = "NORMAL";
                metallicMap.typeString = "METALLIC";
                rougnessMap.typeString = "ROUGHNESS";
                aOMap.typeString = "AO";

                albedoMap.type = TEXTURE_TYPES::ALBEDO;
                normalMap.type = TEXTURE_TYPES::NORMAL;
                metallicMap.type = TEXTURE_TYPES::METALLIC;
                rougnessMap.type = TEXTURE_TYPES::ROUGHNESS;
                aOMap.type = TEXTURE_TYPES::AO;

                textures.push_back(&albedoMap);
                textures.push_back(&normalMap);
                textures.push_back(&metallicMap);
                textures.push_back(&rougnessMap);
                textures.push_back(&aOMap);
            }
            

            loadTextureData(albedoMap);
            loadTextureData(normalMap);
            loadTextureData(metallicMap);
            loadTextureData(rougnessMap);
            loadTextureData(aOMap);
        }

        void bindTextures(const std::string shaderName)
        {
            GLCore::Render::ShaderManager::Get(shaderName)->use();
            int textureChannelInit = 4;

            // Nombres de los mapas
            std::vector<std::string> mapNames = { "albedo", "normal", "metallic", "roughness", "ao" };

            // Iteramos sobre cada mapa en el vector 'textures'
            for (size_t i = 0; i < textures.size(); i++) {
                if (textures[i]->hasMap) {
                    glActiveTexture(GL_TEXTURE0 + textureChannelInit);
                    glBindTexture(GL_TEXTURE_2D, textures[i]->textureID);
                    GLCore::Render::ShaderManager::Get(shaderName)->setInt("material." + mapNames[i] + "Map", textureChannelInit);
                    GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.has" + std::string(1, toupper(mapNames[i][0])) + mapNames[i].substr(1) + "Map", true);
                }
                else {
                    GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.has" + std::string(1, toupper(mapNames[i][0])) + mapNames[i].substr(1) + "Map", false);
                    glActiveTexture(GL_TEXTURE0 + textureChannelInit);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                textureChannelInit++;
            }

            // Establece los valores comunes
            GLCore::Render::ShaderManager::Get(shaderName)->setVec3("material.albedo", color);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.hdrMultiply", hdrMultiply);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.metallic", metallicValue);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.roughness", roughnessValue);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.normalIntensity", normalIntensity);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.reflectance", reflectanceValue);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.fresnelCoef", fresnelCoefValue);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.hdrIntensity", hdrIntensity);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.exposure", exposure);
            GLCore::Render::ShaderManager::Get(shaderName)->setFloat("material.gamma", gamma);
        }
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

        MeshData meshData;
        Material model_material;
    };

    struct ModelParent
    {
        std::string name;
        std::vector<ModelInfo> modelInfos;
    };
}

