#pragma once
#include "ECS.h"
#include "../GLCore/Render/PrimitivesHelper.h"

namespace ECS
{
    class SpotLight : public ECS::Component
    {
    public:

        unsigned int lightID = 0;
        bool active = true;
        bool debug = true;

        glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

        int currentAttenuation = 0; // 0 = Sin atenuación, 1 = Atenuación lineal, 2 = Atenuación cuadrática
        float constant = 1.0f;
        float linear = 1.0f;
        float quadratic = 1.0f;


        //SpotLight values
        glm::vec3 direction = glm::vec3(0.0f, -10.0f, 0.0f);
        float cutOff = 1.0f;
        float outerCutOff = 2.0f;


        //SHADOWS
        bool drawShadows = false;
        glm::mat4 shadowMVP = glm::mat4(1.0f);
        int shadowMapResolution = 1024;

        std::vector<GLuint*> textureBuffers;
        GLuint shadowFBO = 0;
        GLuint shadowTex = 0;
        GLuint shadowDepth = 0;

        float near_plane = 1.0f;
        float far_plane = 100.0f;

        glm::mat4 lightProjection;
         
        void init() override
        {
            sphereDebugMeshData = GLCore::Render::PrimitivesHelper::CreateSphere(1, 6, 6);
            GLCore::Render::PrimitivesHelper::GenerateBuffers(sphereDebugMeshData);
            GLCore::Render::PrimitivesHelper::SetupMeshAttributes(sphereDebugMeshData);

            textureBuffers.push_back(&shadowTex);
            prepareShadows();
        }


        void prepareShadows()
        {
            GLCore::Render::FBOManager::CreateFBO_Color_RGBA16F(&shadowFBO, &shadowDepth, textureBuffers, shadowMapResolution, shadowMapResolution);
        }

        void shadowMappingProjection(std::vector<ECS::Entity*> entitiesInScene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
            glViewport(0, 0, 1024, 1024);
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 shadowProjMat = glm::perspective(glm::radians(outerCutOff * 2.0f), 1.0f, near_plane, far_plane);
            //glm::mat4 shadowProjMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

            glm::mat4 shadowViewMat = glm::lookAt(entity->getComponent<Transform>().position, direction, glm::vec3(0.0f, 1.0f, 0.0f));

            shadowMVP = shadowProjMat * shadowViewMat;

            // render scene from light's point of view
            GLCore::Render::ShaderManager::Get("spotLight_depth_shadows")->use();
            
            for (int i = 0; i < entitiesInScene.size(); i++)
            {
                if (entitiesInScene[i]->hascomponent<MeshRenderer>())
                {
                    if (entitiesInScene[i]->getComponent<MeshRenderer>().dropShadow)
                    {
                        glm::mat4 entityShadowMVP = shadowMVP * entitiesInScene[i]->getComponent<MeshRenderer>().model_transform_matrix;
                        GLCore::Render::ShaderManager::Get("spotLight_depth_shadows")->setMat4("shadowMVP", entityShadowMVP);
                        entitiesInScene[i]->getComponent<ECS::MeshRenderer>().bindVAO();
                    }
                }
            }
        }


        void setId(int _lightID)
        {
            this->lightID = lightID;

            entity->name = "SpotLight_" + std::to_string(lightID);

            //GET LOCATIONS
            activeL      << "spotLights[" << lightID << "].isActive";
            ambientL     << "spotLights[" << lightID << "].ambient";
            diffuseL     << "spotLights[" << lightID << "].diffuse";
            specularL    << "spotLights[" << lightID << "].specular";
            positionL    << "spotLights[" << lightID << "].position";
            directionL   << "spotLights[" << lightID << "].direction";
            constantL    << "spotLights[" << lightID << "].constant";
            linearL      << "spotLights[" << lightID << "].linear";
            quadraticL   << "spotLights[" << lightID << "].quadratic";
            cutOffL      << "spotLights[" << lightID << "].cutOff";
            outerCutOffL << "spotLights[" << lightID << "].outerCutOff";

            drawShadowsL << "spotLights[" << lightID << "].drawShadows";
            shadowMapL   << "spotLights[" << lightID << "].shadowMap";
            shadowMapBiasMVPL << "spotLights[" << lightID << "].shadowBiasMVP";
        }

        void draw() override
        {
            //glBindTexture(GL_TEXTURE_2D, 0);
            
            for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
            {
                GLCore::Render::ShaderManager::Get(name.c_str())->use();
                GLCore::Render::ShaderManager::Get(name.c_str())->setBool(activeL.str().c_str(), active);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(positionL.str().c_str(), entity->getComponent<Transform>().position);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(ambientL.str().c_str(), ambient);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(diffuseL.str().c_str(), diffuse);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(specularL.str().c_str(), specular);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(constantL.str().c_str(), constant);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(linearL.str().c_str(), linear);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(quadraticL.str().c_str(), quadratic);


                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(cutOffL.str(), glm::cos(glm::radians(cutOff)));
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(outerCutOffL.str(), glm::cos(glm::radians(outerCutOff)));
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(directionL.str(), direction);

                GLCore::Render::ShaderManager::Get(name.c_str())->setBool(drawShadowsL.str(), drawShadows);

                if (drawShadows)
                {
                    //Texture shadow
                    glActiveTexture(GL_TEXTURE7);
                    glBindTexture(GL_TEXTURE_2D, shadowTex);
                    GLCore::Render::ShaderManager::Get(name.c_str())->setInt(shadowMapL.str(), 7);

                    //MPV
                    GLCore::Render::ShaderManager::Get(name.c_str())->setMat4(shadowMapBiasMVPL.str(), shadowBias * shadowMVP);
                }
                else
                {
                    glActiveTexture(GL_TEXTURE7);
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
            ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(specular));


            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.1f);
            ImGui::SliderFloat("CutOff", &cutOff, 0.0f, 50.0f);
            ImGui::SliderFloat("OuterCutOff", &outerCutOff, 0.0f, 50.0f);


            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            const char* items[] = { "No Attenuation", "Linear", "Cuadratic" };
            if (ImGui::Combo("Attenuation Type", &currentAttenuation, items, IM_ARRAYSIZE(items)))
            {
                switch (currentAttenuation)
                {
                case 0: // Sin atenuación
                    constant = 1.0f;
                    linear = 0.0f;
                    quadratic = 0.0f;
                    break;

                case 1: // Atenuación lineal
                    constant = 1.0f;
                    linear = 0.09f;
                    quadratic = 0.032f;
                    break;

                case 2: // Atenuación cuadrática
                    constant = 1.0f;
                    linear = 0.07f;
                    quadratic = 0.017f;
                    break;
                }
            }

            ImGui::SliderFloat("Constant", &constant, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.0f);

            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::Checkbox("Draw Shadows", &drawShadows);

            if (drawShadows == true)
            {
               /* ImGui::SliderFloat("Shadow Intensity", &shadowIntensity, 0.0f, 1.0f);
                ImGui::Checkbox("Use Poison Disk", &usePoisonDisk);*/

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Image((void*)(intptr_t)shadowTex, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::SliderFloat("Near", &near_plane, 0.0f, 10.0f);
                ImGui::SliderFloat("Far", &far_plane, 0.0f, 200.0f);

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                if (ImGui::CollapsingHeader("Shadow Bias")) {
                    ImGui::InputFloat("00", &shadowBias[0][0], 0.01f);
                    ImGui::InputFloat("01", &shadowBias[0][1], 0.01f);
                    ImGui::InputFloat("02", &shadowBias[0][2], 0.01f);
                    ImGui::InputFloat("03", &shadowBias[0][3], 0.01f);

                    ImGui::InputFloat("10", &shadowBias[1][0], 0.01f);
                    ImGui::InputFloat("11", &shadowBias[1][1], 0.01f);
                    ImGui::InputFloat("12", &shadowBias[1][2], 0.01f);
                    ImGui::InputFloat("13", &shadowBias[1][3], 0.01f);

                    ImGui::InputFloat("20", &shadowBias[2][0], 0.01f);
                    ImGui::InputFloat("21", &shadowBias[2][1], 0.01f);
                    ImGui::InputFloat("22", &shadowBias[2][2], 0.01f);
                    ImGui::InputFloat("23", &shadowBias[2][3], 0.01f);

                    ImGui::InputFloat("30", &shadowBias[3][0], 0.01f);
                    ImGui::InputFloat("31", &shadowBias[3][1], 0.01f);
                    ImGui::InputFloat("32", &shadowBias[3][2], 0.01f);
                    ImGui::InputFloat("33", &shadowBias[3][3], 0.01f);

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

        void onDestroy() override {}

    private:
        
        std::string currentShaderName = "pbr";

        glm::mat4 shadowBias = glm::mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );

        //------------------------LOCATIONS---------------------------------
        std::stringstream location;
        std::stringstream activeL;
        std::stringstream ambientL;
        std::stringstream diffuseL;
        std::stringstream specularL;
        std::stringstream directionL;
        std::stringstream positionL;
        std::stringstream constantL;
        std::stringstream linearL;
        std::stringstream quadraticL;
        std::stringstream cutOffL;
        std::stringstream outerCutOffL;
        std::stringstream drawShadowsL;
        std::stringstream shadowMapL;
        std::stringstream shadowMapBiasMVPL;
        //--------------------------------------------------------


        //------------------------DEBUG---------------------------------
        GLCore::MeshData sphereDebugMeshData;

        void drawDebug()
        {
            glBindVertexArray(sphereDebugMeshData.VAO);

            glm::mat4 modelMatrix = glm::mat4(1.0f);

            float radiusValue = 1.0f;
            //if (showRange) radiusValue = range;

            glm::vec3 size = glm::vec3(radiusValue, radiusValue, radiusValue);
            modelMatrix = glm::translate(modelMatrix, entity->getComponent<Transform>().position) * glm::scale(glm::mat4(1), size);

            GLCore::Render::ShaderManager::Get("debug")->use();
            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", modelMatrix);
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(ambient.r, ambient.g, ambient.b, 1.0));


            // Dibujar la esfera
            glLineWidth(2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, sphereDebugMeshData.indexCount, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            drawLineDebug();
        }
        void drawLineDebug() {
            // Crea un array con los puntos de la línea
            glm::vec3 rotationVector = direction;

            glm::vec3 linePoints[] = {
                entity->getComponent<Transform>().position,
                entity->getComponent<Transform>().position + rotationVector * 10.0f  // Aquí asumimos que la longitud de la línea es 10
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
            glLineWidth(5);

            glBindVertexArray(lineVAO);
            glDrawArrays(GL_LINES, 0, 2);

            // Elimina el VBO y el VAO de la línea
            glDeleteBuffers(1, &lineVBO);
            glDeleteVertexArrays(1, &lineVAO);
        }
    };
}
