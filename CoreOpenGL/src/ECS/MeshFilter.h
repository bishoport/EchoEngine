#pragma once
#include "ECS.h"
#include "../GLCore/DataStruct.h"



namespace ECS {

    class MeshFilter : public Component {

    public:
        GLCore::MeshData meshData;

        GLCore::MODEL_TYPES modelType = GLCore::MODEL_TYPES::NONE;
        std::string modelPath = "none";


        MeshFilter(){}

        void init() override {}

        void initMesh(const GLCore::MeshData& meshData)
        {
            this->meshData = meshData;
        }

        void draw() override{}

        void onDestroy() override
        {

            //// Liberar VAO
            //glDeleteVertexArrays(1, &meshData.VAO);

            //// Liberar VBO
            //if (meshData.VBO) {
            //    glDeleteBuffers(1, &meshData.VBO);
            //}

            //// Liberar EBO
            //if (meshData.EBO) {
            //    glDeleteBuffers(1, &meshData.EBO);
            //}

            std::cout << "MeshFilter resources released." << std::endl;
            //markForDeletion();
        }

        void drawGUI_Inspector() override
        {            
            //Common Delete
            ImGui::Text("MeshFilter");

            // Muestra los valores en la ventana usando etiquetas y valores
            ImGui::Text("Model type: %s", ModelTypeToString(modelType).c_str());
            if (modelType == GLCore::MODEL_TYPES::EXTERNAL_FILE)
            {
                ImGui::Text("Model path: %s", modelPath.c_str());
            }
            
            ImGui::Text("Index Count: %u", meshData.indexCount);
            ImGui::Text("Number of Triangles: %u", meshData.indexCount / 3);

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }





        // Método para serializar el MeshFilter a YAML
        void serialize(YAML::Emitter& out) const {
            out << YAML::BeginMap;
            out << YAML::Key << "modelType" << YAML::Value << static_cast<int>(modelType);
            out << YAML::Key << "modelPath" << YAML::Value << modelPath;
            // Agrega aquí la serialización de cualquier otro miembro que necesites
            out << YAML::EndMap;
        }

        // Método para deserializar el MeshFilter desde YAML
        void deserialize(const YAML::Node& node) {
            modelType = static_cast<GLCore::MODEL_TYPES>(node["modelType"].as<int>());
            modelPath = node["modelPath"].as<std::string>();
            // Agrega aquí la deserialización de cualquier otro miembro
        }
    };
}  // namespace ECS
