#pragma once
#include "../../glpch.h"
#include "../Util/IMGLoader.h"


namespace GLCore::Render {

    //enum TEX_TYPES {
    //    ALBEDO,
    //    NORMAL,
    //    METALLIC,
    //    ROUGHNESS,
    //    AO
    //};

    ////LA VERSION EN RAM
    //struct ImgData {
    //    unsigned char* pixels;
    //    int width, height, channels;
    //    std::string path = "NONE";
    //};

    ////LA VERSION EN GPU
    //struct TexData {
    //    GLuint textureID = 0;
    //    TEX_TYPES type;
    //    std::string typeString;
    //    Ref<ImgData> image;
    //    bool hasMap = false;

    //    void Bind() {
    //        if (hasMap) {

    //            if (textureID != 0) {
    //                glDeleteTextures(1, &textureID);
    //            }

    //            glGenTextures(1, &textureID);
    //            glBindTexture(GL_TEXTURE_2D, textureID);

    //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //            GLenum format{};
    //            if (image->channels == 1)
    //                format = GL_RED;
    //            else if (image->channels == 3)
    //                format = GL_RGB;
    //            else if (image->channels == 4)
    //                format = GL_RGBA;

    //            glTexImage2D(GL_TEXTURE_2D, 0, format, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->pixels);
    //            glGenerateMipmap(GL_TEXTURE_2D);
    //            glBindTexture(GL_TEXTURE_2D, 0);
    //        }
    //    }
    //};


    //struct MatData
    //{
    //    //TEXTURES
    //    Ref<TexData> albedoMap;
    //    Ref<TexData> normalMap;
    //    Ref<TexData> metallicMap;
    //    Ref<TexData> rougnessMap;
    //    Ref<TexData> aOMap;
    //    //-------------------------------------------------------

    //    //VALUES
    //    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    //    float shininess;
    //    float hdrMultiply = 0.0f;
    //    float hdrIntensity = 0.3f;
    //    float exposure = 1.0f;
    //    float gamma = 2.2f;
    //    float max_reflection_lod = 4.0;
    //    float iblIntensity = 0.0;
    //    float normalIntensity = 0.5f;
    //    float metallicValue = 0.0f;
    //    float roughnessValue = 0.05f;
    //    float reflectanceValue = 0.04f;
    //    float fresnelCoefValue = 5.0f;
    //    //-------------------------------------------------------
    //};


    class MaterialManager 
    {

    public:
        // Obtiene la instancia única de TextureManager
        static MaterialManager& getInstance() {
            static MaterialManager instance;
            return instance;
        }

        // Elimina los métodos de copia y asignación
        MaterialManager(MaterialManager const&) = delete;
        void operator=(MaterialManager const&) = delete;


        std::string CreateDefaultMaterial()
        {
            std::string defaultPathTexture_white = "assets/default/default_white.jpg";
            std::string defaultPathTexture_normal = "assets/default/default_normal.jpg";
            std::string defaultPathTexture_black = "assets/default/default_black.jpg";

            Ref <MaterialData> materialData  = std::make_shared<GLCore::MaterialData>();

            //--ALBEDO
            Ref<Texture> textureAlbedo = std::make_shared<GLCore::Texture>();
            textureAlbedo->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_white);
            textureAlbedo->image->path = defaultPathTexture_white;
            textureAlbedo->hasMap = true;
            materialData->albedoMap = textureAlbedo;
            //---------------------------------------------------------------------------------------------------

            //--NORMAL
            Ref<Texture> textureNormal = std::make_shared<GLCore::Texture>();
            textureNormal->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_normal);
            textureNormal->image->path = defaultPathTexture_normal;
            textureNormal->hasMap = true;
            materialData->normalMap = textureNormal;
            //---------------------------------------------------------------------------------------------------

            //--METALLIC
            Ref<Texture> textureMetallic = std::make_shared<GLCore::Texture>();
            textureMetallic->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_black);
            textureMetallic->image->path = defaultPathTexture_black;
            textureMetallic->hasMap = true;
            materialData->metallicMap = textureMetallic;
            //---------------------------------------------------------------------------------------------------

            //--ROUGHNESS
            Ref<Texture> textureRoughness = std::make_shared<GLCore::Texture>();
            textureRoughness->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_black);
            textureRoughness->image->path = defaultPathTexture_black;
            textureRoughness->hasMap = true;
            materialData->rougnessMap = textureRoughness;
            //---------------------------------------------------------------------------------------------------

            //--AO
            Ref<Texture> textureAO = std::make_shared<GLCore::Texture>();
            textureAO->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_white);
            textureAO->image->path = defaultPathTexture_white;
            textureAO->hasMap = true;
            materialData->aOMap = textureAO;
            //---------------------------------------------------------------------------------------------------

            materialData->albedoMap->typeString = "ALBEDO";
            materialData->normalMap->typeString = "NORMAL";
            materialData->metallicMap->typeString = "METALLIC";
            materialData->rougnessMap->typeString = "ROUGHNESS";
            materialData->aOMap->typeString = "AO";

            materialData->albedoMap->type = TEXTURE_TYPES::ALBEDO;
            materialData->normalMap->type = TEXTURE_TYPES::NORMAL;
            materialData->metallicMap->type = TEXTURE_TYPES::METALLIC;
            materialData->rougnessMap->type = TEXTURE_TYPES::ROUGHNESS;
            materialData->aOMap->type = TEXTURE_TYPES::AO;

            materialData->albedoMap->Bind();
			materialData->normalMap->Bind();
			materialData->metallicMap->Bind();
			materialData->rougnessMap->Bind();
			materialData->aOMap->Bind();

            addMaterial("default", materialData);

            return "default";
        }

        // Agrega un nuevo material al manager
        bool addMaterial(const std::string& key, Ref <MaterialData> materialData) {
            auto result = materials.emplace(key, materialData);
            return result.second; // Retorna true si se insertó, false si ya existía
        }

        // Obtiene un material por su clave
        Ref<MaterialData> getMaterial(const std::string& key) {
            auto it = materials.find(key);
            if (it != materials.end()) {
                return it->second;
            }
            std::cout << key << " material does exist" << std::endl;
            return nullptr; // O manejar el error como prefieras
        }

        // Elimina un material por su clave
        bool removeMaterial(const std::string& key) {
            return materials.erase(key) > 0;
        }

    private:
        MaterialManager() {} // Constructor privado
        std::unordered_map<std::string, Ref<MaterialData>> materials;
        int nextMaterialID = 0; // Un contador simple para generar IDs únicos para los materiales
    };
}
