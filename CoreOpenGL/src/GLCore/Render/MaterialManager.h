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
        void operator=(MaterialManager const&)  = delete;


        void CreateEmptyMaterial(const std::string& matName)
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

            materialData->materialName = matName;

            addMaterial(matName, materialData);
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

            // Campo de texto para el nombre del material
            ImGui::InputText("Material Name", materialNameBuffer, IM_ARRAYSIZE(materialNameBuffer));

            // Botón para crear un nuevo material
            if (ImGui::Button("Create Material")) {
                std::string matName(materialNameBuffer);

                // Si el campo de texto está vacío, genera un nombre único
                if (matName.empty()) {
                    auto now = std::chrono::system_clock::now();
                    auto now_c = std::chrono::system_clock::to_time_t(now);
                    matName = "Material_" + std::to_string(now_c);
                }

                CreateEmptyMaterial(matName);

                // Limpiar el buffer para el siguiente nombre de material
                memset(materialNameBuffer, 0, sizeof(materialNameBuffer));
            }

            ImGui::Separator();

            if (ImGui::TreeNode("Materials")) {
                for (auto& pair : materials) {
                    std::string key = pair.first;
                    Ref<MaterialData> material = pair.second;

                    // Haz que cada nodo de material sea una fuente de arrastre
                    if (ImGui::TreeNodeEx(key.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth)) {
                        // Iniciar una fuente de arrastre
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                            // Establecer la carga útil para el arrastre, que es el nombre del material
                            ImGui::SetDragDropPayload("MATERIAL_NAME", key.c_str(), key.size() + 1);

                            // Muestra una vista previa (opcional)
                            ImGui::Text("Assign %s", key.c_str());
                            ImGui::EndDragDropSource();
                        }
                        drawMaterial(material); // Dibuja la interfaz para este material específico
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }

            ImGui::End();

        }
        void drawMaterial(Ref<MaterialData> materialData) {

            //Editable Material Name
            char buf[128];
            strncpy_s(buf, sizeof(buf), materialData->materialName.c_str(), _TRUNCATE);
            if (ImGui::InputText("Material Name", buf, sizeof(buf))) {
                materialData->materialName = buf;
            }

            // Asegúrate de comenzar con una línea nueva limpia.
            //ImGui::NewLine();
            ImGui::Separator();

            // Asegúrate de que no haya elementos previos que puedan afectar la alineación vertical.
            ImGui::Columns(2);

            // Calcular el ancho de las columnas en proporción al ancho del panel
            float windowWidth = ImGui::GetWindowSize().x; // Obtener el ancho de la ventana
            float columnWidthLeft = windowWidth * 0.4f; // 40% para la columna izquierda
            float columnWidthRight = windowWidth * 0.6f; // 60% para la columna derecha

            ImGui::SetColumnWidth(0, columnWidthLeft); // Establecer el ancho de la columna izquierda
            ImGui::ColorEdit3("Color", glm::value_ptr(materialData->color));
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
            if (ImGui::Button("Reset Values")) {
                materialData->ResetToDefaultValues();
            }
            ImGui::NextColumn(); // Pasar a la columna de la derecha
            ImGui::SetColumnWidth(1, columnWidthRight); // Establecer el ancho de la columna derecha

            // Comienza un child window para las previsualizaciones de texturas
            if (ImGui::BeginChild("TexturePreviews", ImVec2(-1, -1), true)) {
                ImGui::Columns(2); // Establece dos columnas para el grid

                // Llama a showTexture para cada textura en el grid de 2 columnas
                showTexture("Albedo Map", materialData->albedoMap);
                ImGui::NextColumn(); // Mueve a la siguiente columna
                showTexture("Normal Map", materialData->normalMap);
                ImGui::NextColumn(); // Vuelve a la primera columna para la siguiente fila

                showTexture("Metallic Map", materialData->metallicMap);
                ImGui::NextColumn();
                showTexture("Roughness Map", materialData->rougnessMap);
                ImGui::NextColumn();

                showTexture("AO Map", materialData->aOMap);
                // Si hay más texturas, continuaría llamando a showTexture y NextColumn aquí

                ImGui::Columns(1); // Restablece a una columna antes de cerrar el child window
                ImGui::EndChild();
            }

            ImGui::Columns(1);   
        }

        void HandleDragDropForTexture(Ref<Texture> texture, const char* payloadType) {
            // Crear un área de drop. Si algo está siendo arrastrado sobre este área (que cumple con payloadType), resáltalo
            if (ImGui::BeginDragDropTarget()) {
                // Aceptar una carga útil de tipo payloadType. La carga útil necesita contener el identificador de la textura.
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType);
                if (payload != nullptr && payload->IsDataType(payloadType)) {

                    if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
                    {
                        std::string dropped_fpath = (const char*)payload->Data;

                        texture->image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
                        texture->Bind();
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }

        void showTexture(const char* label, Ref<Texture> texture) 
        {
            // Muestra la textura
            ImGui::Image((void*)(intptr_t)texture->textureID, ImVec2(128, 128)); // Ajustar según el tamaño deseado
            HandleDragDropForTexture(texture, "ASSET_DRAG");
            if (texture && texture->hasMap) {
               
                ImGui::Text("%s", label); // Muestra el nombre de la textura
                ImGui::Text("Path: %s", texture->image->path.c_str()); // Muestra la ruta de la textura
            }
            else {
                ImGui::Text("%s: None", label); // Para texturas que no existen
            }
        }

    private:
        MaterialManager() {} // Constructor privado
        std::unordered_map<std::string, Ref<MaterialData>> materials;
        static char materialNameBuffer[128];
    };
}
