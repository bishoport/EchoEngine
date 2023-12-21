#pragma once
#include "../../glpch.h"
#include "../Util/IMGLoader.h"


namespace GLCore::Render 
{
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
        Ref<MaterialData> addMaterial(const std::string& key, Ref<MaterialData> materialData) {
            auto result = materials.emplace(key, materialData);

            if (result.second) {
                // El material fue insertado correctamente.
                return materialData;
            }
            else {
                // El material ya existía, devuelve el material existente.
                return result.first->second;
            }
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


        //DRAW IMGUI
        void drawMaterialsPanel() {
            ImGui::Begin("Materials In Scene"); // Comienza el panel de materiales

            if (ImGui::TreeNode("Materials")) {
                for (auto& pair : materials) {
                    std::string key = pair.first;
                    Ref<MaterialData> material = pair.second;

                    if (ImGui::TreeNode(key.c_str())) {
                        drawMaterial(material); // Dibuja la interfaz para este material específico
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }

            ImGui::End(); // Termina el panel de materiales
        }

        void drawMaterial(Ref<MaterialData> materialData) {
            // Mostrar y editar el nombre del material
            char buf[128];
            // Suponiendo que 'buf' es tu buffer destino y 'sizeof(buf)' es su tamaño
            strncpy_s(buf, sizeof(buf), materialData->materialName.c_str(), _TRUNCATE);

            if (ImGui::InputText("Material Name", buf, sizeof(buf))) {
                materialData->materialName = buf;
            }

            if (ImGui::Button("Reset Values")) {
                materialData->ResetToDefaultValues();
            }

            // Mostrar y editar el color albedo
            ImGui::ColorEdit3("Color", glm::value_ptr(materialData->color));

            // Mostrar y editar las propiedades del material
            ImGui::SliderFloat("Shininess", &materialData->shininess, 0.0f, 128.0f);
            ImGui::SliderFloat("HDR Multiply", &materialData->hdrMultiply, 0.0f, 10.0f);
            ImGui::SliderFloat("HDR Intensity", &materialData->hdrIntensity, 0.0f, 10.0f);
            ImGui::SliderFloat("Exposure", &materialData->exposure, 0.1f, 10.0f);
            ImGui::SliderFloat("Gamma", &materialData->gamma, 0.1f, 3.0f);
            ImGui::SliderFloat("Max Reflection LOD", &materialData->max_reflection_lod, 0.0f, 10.0f);
            ImGui::SliderFloat("IBL Intensity", &materialData->iblIntensity, 0.0f, 10.0f);
            ImGui::SliderFloat("Normal Intensity", &materialData->normalIntensity, 0.0f, 10.0f);
            ImGui::SliderFloat("Metallic Value", &materialData->metallicValue, 0.0f, 1.0f);
            ImGui::SliderFloat("Roughness Value", &materialData->roughnessValue, 0.0f, 1.0f);
            ImGui::SliderFloat("Reflectance Value", &materialData->reflectanceValue, 0.0f, 1.0f);
            ImGui::SliderFloat("Fresnel Coefficient", &materialData->fresnelCoefValue, 0.1f, 10.0f);

            // Mostrar y posiblemente cambiar las texturas
            // Suponiendo que tienes una función que devuelve una representación de ImGui de la textura
            showTexture("Albedo Map", materialData->albedoMap);
            showTexture("Normal Map", materialData->normalMap);
            showTexture("Metallic Map", materialData->metallicMap);
            showTexture("Roughness Map", materialData->rougnessMap);
            showTexture("AO Map", materialData->aOMap);
        }

        void showTexture(const char* label, Ref<Texture> texture) {
            if (texture && texture->hasMap) {
                ImGui::Text("%s: %s", label, texture->image->path.c_str());
                // Aquí podrías añadir una imagen de ImGui si tienes el identificador de la textura de OpenGL
                ImGui::Image((void*)(intptr_t)texture->textureID, ImVec2(64, 64));
                // Podrías añadir también un botón para cargar una nueva textura
                if (ImGui::Button("Load New Texture")) {
                    // Lógica para cargar una nueva textura
                }
            }
        }


    private:
        MaterialManager() {} // Constructor privado
        std::unordered_map<std::string, Ref<MaterialData>> materials;
        int nextMaterialID = 0; // Un contador simple para generar IDs únicos para los materiales
    };
}
