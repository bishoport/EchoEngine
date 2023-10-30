#pragma once
#include "ECS.h"



namespace ECS {

    class MeshFilter : public Component {

    public:
        GLCore::MeshData meshData;


        MeshFilter(){}

        void init() override {}

        void initMesh(const GLCore::MeshData& meshData)
        {
            this->meshData = meshData;
        }

        void draw() override
        {

        }


        void drawGUI_Inspector() override
        {
            ImGui::Text("MeshFilter");
            // Muestra los valores en la ventana usando etiquetas y valores
            ImGui::Text("VAO: %u", meshData.VAO);
            ImGui::Text("VBO: %u", meshData.VBO);
            ImGui::Text("EBO: %u", meshData.EBO);
            ImGui::Text("Index Count: %u", meshData.indexCount);
            ImGui::Text("Number of Triangles: %u", meshData.indexCount / 3);

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    };
}  // namespace ECS
