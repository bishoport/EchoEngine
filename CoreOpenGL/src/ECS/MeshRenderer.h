#pragma once
#include "ECS.h"
#include "../GLCore/Render/PrimitivesHelper.h"

namespace ECS {

    class MeshRenderer : public Component {

    public:
        std::string currentShaderName = "pbr_ibl";

        bool visibleModel = true;
        bool drawLocalBB = false;

        GLCore::MeshData meshData;
        bool dropShadow = true;

        glm::mat4 model_transform_matrix{ glm::mat4(1.0f) };


        MeshRenderer() {}

        void onDestroy() override
        {
            // Liberar VAO
            glDeleteVertexArrays(1, &meshData.VAO);
            // Si hay otros VBOs, tambi�n deber�as liberarlos aqu�.
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
            entity->getComponent<Transform>().position = meshData.meshLocalPosition;

            //-----------------------------------PREPARE BOUNDING BOX LOCAL-----------------------------------
            meshData.PrepareAABB();
            //-------------------------------------------------------------------------------------------------

            // Generar lista de nombres de shaders
            for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
            {
                shaderNames.push_back(name.c_str());
            }
        }

        void update() override
        {
            // Recuperamos el componente Transform de la entidad
            Transform& transform = entity->getComponent<Transform>();

            // Ahora, directamente podemos obtener la matriz de transformaci�n del objeto
            model_transform_matrix = transform.getLocalModelMatrix();

            // Si hay un padre, combinamos nuestras transformaciones con las de �l.
            if (entity->getComponent<Transform>().parent != nullptr) {
                model_transform_matrix = entity->getComponent<Transform>().parent->getComponent<Transform>().getLocalModelMatrix() * model_transform_matrix;
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
            if (ImGui::Button("Delete Mesh Renderer")) {
                entity->removeComponent<MeshRenderer>();
                return;
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("Visible Model", &visibleModel);
            ImGui::Checkbox("Draw Local Bounding Box", &drawLocalBB);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Text("Current Shader: %s", currentShaderName.c_str());

            static int selectedItem = -1;  // El �ndice del elemento seleccionado, -1 significa que ning�n elemento est� seleccionado
            const char* comboPreviewContent = selectedItem >= 0 ? shaderNames[selectedItem] : "Select a shader...";  // Lo que se mostrar� cuando no se haya desplegado la lista

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

    private:
        //PARA LA GESTION DEL SHADER
        std::vector<const char*> shaderNames;
    };

}  // namespace ECS












//
//
//GLfloat vertices_AABB[72] = {
//    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  // L�nea 1
//     0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  // L�nea 2
//     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  // L�nea 3
//    -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  // L�nea 4
//    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  // L�nea 5
//     0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  // L�nea 6
//     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  // L�nea 7
//    -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  // L�nea 8
//    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f,  // L�nea 9
//     0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  // L�nea 10
//     0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  // L�nea 11
//    -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f   // L�nea 12
//};
//
//// Definimos los �ndices de los v�rtices del bounding box que forman las l�neas
//GLuint indices_AABB[24] = {
//    0, 1, 1, 2, 2, 3, 3, 0,  // L�neas de la cara frontal
//    4, 5, 5, 6, 6, 7, 7, 4,  // L�neas de la cara trasera
//    0, 4, 1, 5, 2, 6, 3, 7   // L�neas diagonales
//};