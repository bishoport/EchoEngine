#pragma once

#include "../glpch.h"
#include "../GLCore/DataStruct.h"
#include "Component.h"
#include "Entity.h"
#include "Transform.h"

#include "../GLCore/Render/PrimitivesHelper.h"
#include "../GLCore/Core/Scene.h"


namespace ECS
{
    class DirectionalLight : public ECS::Component
    {
    public:
        unsigned int lightID = 0;
        bool active = true;
        bool debug = true;

        glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

        //DirectionalLight values
        glm::vec3 direction = glm::vec3(0.0f, -10.0f, 0.0f);
        float currentSceneRadius = 0.0f;
        float sceneRadiusOffset = 0.0f;

        //Shadow values
        int shadowMapResolution = 1024;

        GLuint shadowFBO = 0;
        GLuint shadowTex = 0;
        GLuint shadowDepth = 0;


        glm::mat4 shadowMVP = glm::mat4(1.0f);
        bool drawShadows = false;
        float near_plane = 0.1f, far_plane = 100.0f;
        float shadowIntensity = 0.5f;
        bool usePoisonDisk = false;
        float orthoLeft = -10.0f;
        float orthoRight = 10.0f;
        float orthoBottom = -10.0f;
        float orthoTop = 10.0f;

        float orthoNear = 0.1f;
        float orthoNearOffset = 0.0f;
        float orthoFar = 100.0f;
        float orthoFarOffset = 0.0f;

        std::vector<ECS::Entity*> m_entitiesInScene;

        void init() override
        {
            entity->name = "DirectionalLight";
            entity->getComponent<ECS::Transform>().position = glm::vec3( - 2.0f, 4.0f, -1.0f);

            prepareShadows();
        } 

        ComponentID getTypeID() const
        {
            return getComponentTypeID<DirectionalLight>();
        }


        void onDestroy() override
        {
            // Liberar el mapa de sombras y los recursos asociados.
            if (shadowTex != 0) {
                glDeleteTextures(1, &shadowTex);
                shadowTex = 0;
            }

            if (shadowDepth != 0) {
                glDeleteTextures(1, &shadowDepth);
                shadowDepth = 0;
            }

            if (shadowFBO != 0) {
                glDeleteFramebuffers(1, &shadowFBO);
                shadowFBO = 0;
            }
        }


        void prepareShadows()
        {
            GLCore::Render::FBOManager::CreateShadowMapFBO(&shadowFBO, &shadowDepth, &shadowTex, shadowMapResolution);
        }

        

        void shadowMappingProjection(std::vector<ECS::Entity*> entitiesInScene)
        {
            m_entitiesInScene = entitiesInScene;


            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
            glViewport(0, 0, shadowMapResolution, shadowMapResolution);
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            glm::mat4 shadowProjMat = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
            glm::mat4 shadowViewMat = glm::lookAt(entity->getComponent<ECS::Transform>().position, direction, glm::vec3(0, 1, 0));
            
            shadowMVP = shadowProjMat * shadowViewMat;

            GLCore::Render::ShaderManager::Get("direct_light_depth_shadows")->use();
            for (int i = 0; i < entitiesInScene.size(); i++)
            {
                if (entitiesInScene[i]->hascomponent<MeshRenderer>())
                {
                    if (entitiesInScene[i]->getComponent<MeshRenderer>().dropShadow)
                    {
                        glm::mat4 entityShadowMVP = shadowMVP * entitiesInScene[i]->getComponent<MeshRenderer>().model_transform_matrix;
                        GLCore::Render::ShaderManager::Get("direct_light_depth_shadows")->setMat4("shadowMVP", entityShadowMVP);
                        entitiesInScene[i]->getComponent<ECS::MeshRenderer>().bindVAO();
                    }
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void update(GLCore::Timestep timestamp) override
        {
            auto [sceneCenter, sceneRadius] = GLCore::Scene::SceneBounds;

            currentSceneRadius = sceneRadius;

            if (sceneRadius > 0.0f)
            {
                sceneRadius += sceneRadiusOffset;

                orthoLeft = -sceneRadius;
                orthoRight = sceneRadius;
                orthoBottom = -sceneRadius;
                orthoTop = sceneRadius;
                orthoNear = -sceneRadius - orthoNearOffset;
                orthoFar = (2 * sceneRadius) + orthoFarOffset;

                // Calcula la posición de la luz basada en los ángulos y la distancia al centro de la escena
                Transform& lightTransform = entity->getComponent<ECS::Transform>();
                lightTransform.position.x = sceneCenter.x + sceneRadius * sin(angleX) * cos(angleY);
                lightTransform.position.y = sceneCenter.y + sceneRadius * cos(angleX);
                lightTransform.position.z = sceneCenter.z + sceneRadius * sin(angleX) * sin(angleY);

                // Actualiza la dirección de la luz
                direction = glm::normalize(sceneCenter - lightTransform.position);
            }
        }

        void draw() override
        {
            for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
            {
                GLCore::Render::ShaderManager::Get(name.c_str())->use();
                GLCore::Render::ShaderManager::Get(name.c_str())->setBool("dirLight.isActive",  active);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.direction", direction);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.position",  entity->getComponent<ECS::Transform>().position);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.ambient",   ambient);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.diffuse",   diffuse);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.specular",  specular);
                GLCore::Render::ShaderManager::Get(name.c_str())->setBool("dirLight.drawShadows", drawShadows);

                if (drawShadows)
                {
                    //Texture shadow
                    glActiveTexture(GL_TEXTURE10);
                    glBindTexture(GL_TEXTURE_2D, shadowTex);
                    GLCore::Render::ShaderManager::Get(name.c_str())->setInt("dirLight.shadowMap", 10);
                    GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("dirLight.shadowBiasMVP", shadowBias * shadowMVP);
                    GLCore::Render::ShaderManager::Get(name.c_str())->setFloat("dirLight.shadowIntensity", shadowIntensity);
                    GLCore::Render::ShaderManager::Get(name.c_str())->setBool("dirLight.usePoisonDisk", usePoisonDisk);
                }
                else
                {
                    glActiveTexture(GL_TEXTURE10);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }
            if (debug)
            {
                drawDebug();
            }
        }

        void drawGUI_Inspector() override
        {
            ImGui::Text("PointLight");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("Active", &active);
            ImGui::Checkbox("Debug", &debug);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderFloat("Orbit X", &angleX, 0.0f, 6.28319f);
            ImGui::SliderFloat("Orbit Y", &angleY, 0.0f, 6.28319f);
            ImGui::SliderFloat("Scene Radius Offset", &sceneRadiusOffset, -(currentSceneRadius * 10.0f), (currentSceneRadius * 10.0f), "%.1f");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(specular));


            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("Draw Shadows", &drawShadows);

            if (drawShadows == true)
            {
                ImGui::SliderFloat("Shadow Intensity", &shadowIntensity, 0.0f, 1.0f);
                ImGui::Checkbox("Use Poison Disk", &usePoisonDisk);

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Image((void*)(intptr_t)shadowTex, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::SliderFloat("Left", &orthoLeft, -100.0f, 100.0f);
                ImGui::SliderFloat("Right", &orthoRight, -100.0f, 100.0f);
                ImGui::SliderFloat("Top", &orthoTop, -100.0f, 100.0f);
                ImGui::SliderFloat("Bottom", &orthoBottom, -100.0f, 100.0f);

                ImGui::SliderFloat("Near", &orthoNear, 0.0f, 100.0f);
                ImGui::SliderFloat("Near Offset", &orthoNearOffset, -100.0f, 100.0f);
                ImGui::SliderFloat("Far", &orthoFar, 0.0f, 500.0f);
                ImGui::SliderFloat("Far Offset", &orthoFarOffset, -100.0f, 100.0f);

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                if (ImGui::CollapsingHeader("Shadow Bias")) {
                    ImGui::InputFloat("00", &shadowBias[0][0], 0.001f);
                    ImGui::InputFloat("01", &shadowBias[0][1], 0.001f);
                    ImGui::InputFloat("02", &shadowBias[0][2], 0.001f);
                    ImGui::InputFloat("03", &shadowBias[0][3], 0.001f);

                    ImGui::InputFloat("10", &shadowBias[1][0], 0.001f);
                    ImGui::InputFloat("11", &shadowBias[1][1], 0.001f);
                    ImGui::InputFloat("12", &shadowBias[1][2], 0.001f);
                    ImGui::InputFloat("13", &shadowBias[1][3], 0.001f);

                    ImGui::InputFloat("20", &shadowBias[2][0], 0.001f);
                    ImGui::InputFloat("21", &shadowBias[2][1], 0.001f);
                    ImGui::InputFloat("22", &shadowBias[2][2], 0.001f);
                    ImGui::InputFloat("23", &shadowBias[2][3], 0.001f);

                    ImGui::InputFloat("30", &shadowBias[3][0], 0.001f);
                    ImGui::InputFloat("31", &shadowBias[3][1], 0.001f);
                    ImGui::InputFloat("32", &shadowBias[3][2], 0.001f);
                    ImGui::InputFloat("33", &shadowBias[3][3], 0.001f);

                    if (ImGui::Button("Reset")) {
                        shadowBias = glm::mat4(
                            0.5, 0.0, 0.0, 0.0,
                            0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0,
                            0.5, 0.5, 0.5, 1.0
                        );
                    }
                }
            }

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }

    private:
        float angleX = 0.0f;
        float angleY = 0.0f;

        glm::mat4 shadowBias = glm::mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );

        //------------------------DEBUG---------------------------------
        void drawDebug()
        {
            // Crea un array con los puntos de la línea
            glm::vec3 rotationVector = direction;

            glm::vec3 linePoints[] = {
                entity->getComponent<ECS::Transform>().position,
                entity->getComponent<ECS::Transform>().position + rotationVector * 10.0f  // Aquí asumimos que la longitud de la línea es 10
            };

            // Crea el VBO y el VAO para la línea
            GLuint lineVBO, lineVAO;
            glGenBuffers(1, &lineVBO);
            glGenVertexArrays(1, &lineVAO);

            // Rellena el VBO con los puntos de la línea
            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(linePoints), linePoints, GL_STATIC_DRAW);

            // Enlaza el VBO al VAO
            glBindVertexArray(lineVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            // Dibuja la línea
            GLCore::Render::ShaderManager::Get("debug")->use();
            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", glm::mat4(1.0f));
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(ambient.r, ambient.g, ambient.b, 1.0));
            glLineWidth(2);

            glBindVertexArray(lineVAO);
            glDrawArrays(GL_LINES, 0, 2);

            // Elimina el VBO y el VAO de la línea
            glDeleteBuffers(1, &lineVBO);
            glDeleteVertexArrays(1, &lineVAO);
        }
    };
}



