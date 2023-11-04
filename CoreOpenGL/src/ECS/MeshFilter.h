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

            ImGui::Text("Index Count: %u", meshData.indexCount);
            ImGui::Text("Number of Triangles: %u", meshData.indexCount / 3);

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    };
}  // namespace ECS
