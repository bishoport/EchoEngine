#pragma once
#include "../glpch.h"

namespace GLCore 
{
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
        std::string type;
        Image image;
        bool wasLoaded = false;
    };

    struct Material {
        float shininess;
        float hdrMultiply = 0.0f;
        float hdrIntensity = 0.3f;
        float exposure = 1.0f;
        float gamma = 2.2f;
        //-------------------------------------------------------

        //--PBR
        //ALBEDO
        glm::vec3 albedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
        Texture albedoMap;
        bool hasAlbedoMap = false;

        //NORMAL
        Texture normalMap;
        bool hasNormalMap = false;

        //METALLIC
        Texture metallicMap;
        bool hasMetallicMap = false;

        //ROUGHTNESS
        Texture rougnessMap;
        bool hasRougnessMap = false;

        //AMBIENT OCLUSSION
        Texture aOMap;
        bool hasAoMap = false;

        //VALUES
        float normalIntensity = 0.5f;
        float metallicValue = 0.0f;
        float roughnessValue = 0.05f;
        float reflectanceValue = 0.04f;
        float fresnelCoefValue = 5.0f;
        //-------------------------------------------------------



        void prepare_PBRMaterials()
        {
            //--------------------------------------ALBEDO MAP TEXTURE--------------------------------------
            if (albedoMap.image.pixels && albedoMap.image.width > 0) {
                
                glGenTextures(1, &albedoMap.textureID); //Aqui se genera por OpenGL un ID para la textura
                glBindTexture(GL_TEXTURE_2D, albedoMap.textureID); //Bind-> abrir la textura para poder usarla para parametrizarla

                //Flags 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format;
                if (albedoMap.image.channels == 1)
                    format = GL_RED;
                else if (albedoMap.image.channels == 3)
                    format = GL_RGB;
                else if (albedoMap.image.channels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, albedoMap.image.width, albedoMap.image.height, 0, format, GL_UNSIGNED_BYTE, albedoMap.image.pixels);

                glGenerateMipmap(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
                hasAlbedoMap = true;

                //GLCore::Utils::ImageLoader::freeImage(albedoMap.image);
            }


            //--------------------------------------NORMAL MAP TEXTURE--------------------------------------
            if (normalMap.image.pixels && normalMap.image.width > 0) {
                glGenTextures(1, &normalMap.textureID);
                glBindTexture(GL_TEXTURE_2D, normalMap.textureID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


                GLenum format;
                if (normalMap.image.channels == 1)
                    format = GL_RED;
                else if (normalMap.image.channels == 3)
                    format = GL_RGB;
                else if (normalMap.image.channels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, normalMap.image.width, normalMap.image.height, 0, format, GL_UNSIGNED_BYTE, normalMap.image.pixels);

                glGenerateMipmap(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
                hasNormalMap = true;
            }


            //--------------------------------------METALLIC MAP TEXTURE--------------------------------------
            if (metallicMap.image.pixels && metallicMap.image.width > 0) {
                glGenTextures(1, &metallicMap.textureID); //Aqui se genera por OpenGL un ID para la textura
                glBindTexture(GL_TEXTURE_2D, metallicMap.textureID); //Bind-> abrir la textura para poder usarla para parametrizarla

                //Flags 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format = GL_RED;
                if (metallicMap.image.channels == 1)
                    format = GL_RED;
                else if (metallicMap.image.channels == 3)
                    format = GL_RGB;
                else if (metallicMap.image.channels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, metallicMap.image.width, metallicMap.image.height, 0, format, GL_UNSIGNED_BYTE, metallicMap.image.pixels);

                glGenerateMipmap(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
                hasMetallicMap = true;
            }



            //--------------------------------------ROUGHNESS MAP TEXTURE--------------------------------------
            if (rougnessMap.image.pixels && rougnessMap.image.width > 0) {
                glGenTextures(1, &rougnessMap.textureID); //Aqui se genera por OpenGL un ID para la textura
                glBindTexture(GL_TEXTURE_2D, rougnessMap.textureID); //Bind-> abrir la textura para poder usarla para parametrizarla

                //Flags 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format;
                if (rougnessMap.image.channels == 1)
                    format = GL_RED;
                else if (rougnessMap.image.channels == 3)
                    format = GL_RGB;
                else if (rougnessMap.image.channels == 4)
                    format = GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, format, rougnessMap.image.width, rougnessMap.image.height, 0, format, GL_UNSIGNED_BYTE, rougnessMap.image.pixels);

                glGenerateMipmap(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
                hasRougnessMap = true;
            }


            //--------------------------------------AO MAP TEXTURE--------------------------------------
            if (aOMap.image.pixels && aOMap.image.width > 0) {

                glGenTextures(1, &aOMap.textureID);
                glBindTexture(GL_TEXTURE_2D, aOMap.textureID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format;
                if (aOMap.image.channels == 1)
                    format = GL_RED;
                else if (aOMap.image.channels == 3)
                    format = GL_RGB;
                else if (aOMap.image.channels == 4)
                    format = GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, format, aOMap.image.width, aOMap.image.height, 0, format, GL_UNSIGNED_BYTE, aOMap.image.pixels);

                glGenerateMipmap(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
                hasAoMap = true;
            }
        }

        

        void bindTextures(const std::string shaderName)
        {
            GLCore::Render::ShaderManager::Get(shaderName)->use();

            int textucheChannelInit = 4;

            //--Albedo Map
            if (hasAlbedoMap) { // si tienes un mapa difuso
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, albedoMap.textureID);
                GLCore::Render::ShaderManager::Get(shaderName)->setInt("material.albedoMap", textucheChannelInit);
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasAlbedoMap", true);
            }
            else
            {
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasAlbedoMap", false);
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            textucheChannelInit++;
            //--Normal Map
            if (hasNormalMap) { // si tienes un mapa normales
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, normalMap.textureID);
                GLCore::Render::ShaderManager::Get(shaderName)->setInt("material.normalMap", textucheChannelInit);
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasNormalMap", true);
            }
            else
            {
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasNormalMap", false);
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            textucheChannelInit++;

            //--METALLIC Map
            if (hasMetallicMap) { // si tienes un mapa normales
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, metallicMap.textureID);
                GLCore::Render::ShaderManager::Get(shaderName)->setInt("material.metallicMap", textucheChannelInit);
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasMetallicMap", true);
            }
            else
            {
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasMetallicMap", false);
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            textucheChannelInit++;


            //--ROUGHNESS Map
            if (hasRougnessMap) { // si tienes un mapa normales
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, rougnessMap.textureID);
                GLCore::Render::ShaderManager::Get(shaderName)->setInt("material.roughnessMap", textucheChannelInit);
            }
            else
            {
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasRougnessMap", false);
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            textucheChannelInit++;


            //--AO Map
            if (hasAoMap) { // si tienes un mapa especulares
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, aOMap.textureID);
                GLCore::Render::ShaderManager::Get(shaderName)->setInt("material.aoMap", textucheChannelInit);
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasAoMap", true);
            }
            else
            {
                GLCore::Render::ShaderManager::Get(shaderName)->setBool("material.hasAoMap", false);
                glActiveTexture(GL_TEXTURE0 + textucheChannelInit);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            //VALORES COMUNES
            GLCore::Render::ShaderManager::Get(shaderName)->setVec3("material.albedo", albedoColor);
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

