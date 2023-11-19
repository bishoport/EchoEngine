#pragma once
#include "../src/glpch.h"
#include "../src/GLCore/DataStruct.h"
#include "Entity.h"
#include "Component.h"
#include "Transform.h"
#include "MeshFilter.h"

#include "../src/GLCore/Render/PrimitivesHelper.h"

namespace ECS {

    class MeshRenderer : public Component {

    public:
        std::string currentShaderName = "pbr_ibl";

        bool visibleModel = true;
        bool drawLocalBB = false;
        bool dropShadow = true;


        GLCore::MeshData meshData;
        glm::mat4 model_transform_matrix{ glm::mat4(1.0f) };


        MeshRenderer() {}

        void onDestroy() override
        {
            // Liberar VAO
            glDeleteVertexArrays(1, &meshData.VAO);
            // Si hay otros VBOs, también deberías liberarlos aquí.
            glDeleteBuffers(1, &meshData.VBO);  // Si tienes un VBO
            glDeleteBuffers(1, &meshData.EBO);  // Si tienes un EBO
            std::cout << "MeshRenderer resources released." << std::endl;
        }

        void init() override 
        {
            this->meshData = entity->getComponent<MeshFilter>().meshData;

            GLCore::Render::PrimitivesHelper::GenerateBuffers(meshData);
            GLCore::Render::PrimitivesHelper::SetupMeshAttributes(meshData);

            //Keep original Position
            entity->getComponent<ECS::Transform>().position = meshData.meshLocalPosition;

            //-----------------------------------PREPARE BOUNDING BOX LOCAL-----------------------------------
            meshData.PrepareAABB();
            //-------------------------------------------------------------------------------------------------

            // Generar lista de nombres de shaders
            for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
            {
                shaderNames.push_back(name.c_str());
            }
        }

        ComponentID getComponentID() const
        {
            return getComponentTypeID<MeshRenderer>();
        }

        void update(GLCore::Timestep timestamp) override
        {
            // Recuperamos el componente Transform de la entidad
            Transform& transform = entity->getComponent<ECS::Transform>();

            // Ahora, directamente podemos obtener la matriz de transformación del objeto
            model_transform_matrix = transform.getLocalModelMatrix();

            // Si hay un padre, combinamos nuestras transformaciones con las de él.
            if (entity->getComponent<ECS::Transform>().parent != nullptr) {
                model_transform_matrix = entity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().getLocalModelMatrix() * model_transform_matrix;
            }
        }

        void bindVAO()
        {
            glBindVertexArray(meshData.VAO);
            glDrawElements(GL_TRIANGLES, meshData.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        void draw() override 
        {
            GLCore::Render::ShaderManager::Get(currentShaderName)->use();
            GLCore::Render::ShaderManager::Get(currentShaderName)->setMat4("model", model_transform_matrix);

            bindVAO();


            // Configura y usa un shader simple para dibujar la Bounding Box
            GLCore::Render::ShaderManager::Get("debug")->use();
            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", model_transform_matrix);
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(1.0, 0.5, 0.2, 1.0));

            if (drawLocalBB)
            {
                // Dibuja la Bounding Box
                glLineWidth(5);
                glBindVertexArray(meshData.VAO_BB);
                glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        void drawGUI_Inspector() override
        {
            //Common Delete
            ImGui::Text("MeshRenderer");
            ImGui::Text("Component ID: %i", getComponentID());
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            if (ImGui::Button("Delete Mesh Renderer")) {
                entity->removeComponent<MeshRenderer>();
                return;
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("Visible Model", &visibleModel);
            ImGui::Checkbox("Draw Local Bounding Box", &drawLocalBB);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Text("Current Shader: %s", currentShaderName.c_str());

            static int selectedItem = -1;  // El índice del elemento seleccionado, -1 significa que ningún elemento está seleccionado
            const char* comboPreviewContent = selectedItem >= 0 ? shaderNames[selectedItem] : "Select a shader...";  // Lo que se mostrará cuando no se haya desplegado la lista

            if (ImGui::BeginCombo("Shaders", comboPreviewContent))
            {
                for (int i = 0; i < shaderNames.size(); i++)
                {
                    bool isSelected = selectedItem == i;
                    if (ImGui::Selectable(shaderNames[i], isSelected))
                    {
                        selectedItem = i;
                        currentShaderName = shaderNames[i];
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }

        // Método para serializar el MeshFilter a YAML
        void serialize(YAML::Emitter& out) const {
            out << YAML::BeginMap;
            out << YAML::Key << "ShaderName" << YAML::Value << currentShaderName;
            out << YAML::Key << "visibleModel" << YAML::Value << visibleModel;
            out << YAML::Key << "dropShadow" << YAML::Value << dropShadow;
            out << YAML::EndMap;
        }

        // Método para deserializar el MeshFilter desde YAML
        void deserialize(const YAML::Node& node) {

        }

    private:
        //PARA LA GESTION DEL SHADER
        std::vector<const char*> shaderNames;
    };

}  // namespace ECS



////GLfloat vertices_AABB[72] = {
////    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  // Línea 1
////     0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  // Línea 2
////     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  // Línea 3
////    -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  // Línea 4
////    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  // Línea 5
////     0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  // Línea 6
////     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  // Línea 7
////    -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  // Línea 8
////    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f,  // Línea 9
////     0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  // Línea 10
////     0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  // Línea 11
////    -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f   // Línea 12
////};
////
////// Definimos los índices de los vértices del bounding box que forman las líneas
////GLuint indices_AABB[24] = {
////    0, 1, 1, 2, 2, 3, 3, 0,  // Líneas de la cara frontal
////    4, 5, 5, 6, 6, 7, 7, 4,  // Líneas de la cara trasera
////    0, 4, 1, 5, 2, 6, 3, 7   // Líneas diagonales
////};