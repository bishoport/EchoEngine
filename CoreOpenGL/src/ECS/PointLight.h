#pragma once
#include "../glpch.h"
#include "../GLCore/DataStruct.h"
#include "Entity.h"
#include "Transform.h"


namespace ECS {

    class PointLight : public Component {

    public:

        unsigned int lightID = 0;
        bool active = true;
        bool debug = true;
        
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);

        int currentAttenuation = 0; // 0 = Sin atenuación, 1 = Atenuación lineal, 2 = Atenuación cuadrática
        float constant = 1.0f;
        float linear = 1.0f;
        float quadratic = 1.0f;


        //-LightPoint properties
        float strength = 10.0f;
        float range = 1.0f;
        float lightSmoothness = 0.8f;
        float specularPower = 1.0f;
        bool blinn = true;

        float hdrMultiply = 1.0f;

        //-Shadows
        bool drawShadows = false;
        int shadowMapResolution = 1024;
        float shadowIntensity = 0.8f;
        int usePoisonDisk;
        GLuint shadowTex = 0;
        GLuint shadowFBO = 0;


        void init() override 
        {
            sphereDebugMeshData = GLCore::Render::PrimitivesHelper::CreateSphere(1, 6, 6);

            GLCore::Render::PrimitivesHelper::GenerateBuffers(sphereDebugMeshData);
            GLCore::Render::PrimitivesHelper::SetupMeshAttributes(sphereDebugMeshData);
        }

        ComponentID getTypeID() const
        {
            return getComponentTypeID<PointLight>();
        }

        void setId(int _lightID)
        {
            this->lightID = _lightID;

            entity->name = "PointLight_" + std::to_string(lightID);

            //GET LOCATIONS
            activeL          << "pointLights[" << lightID << "].isActive";
            ambientL         << "pointLights[" << lightID << "].ambient";     
            diffuseL         << "pointLights[" << lightID << "].diffuse";     
            specularL        << "pointLights[" << lightID << "].specular";    
            positionL        << "pointLights[" << lightID << "].position";     
            strengthL        << "pointLights[" << lightID << "].strength";	     
            constantL        << "pointLights[" << lightID << "].constant";     
            linearL          << "pointLights[" << lightID << "].linear";    
            quadraticL       << "pointLights[" << lightID << "].quadratic";	     
            rangeL           << "pointLights[" << lightID << "].range";
            lightSmoothnessL << "pointLights[" << lightID << "].lightSmoothness";
            specularPowerL   << "pointLights[" << lightID << "].specularPower";

            drawShadowsL << "pointLights[" << lightID << "].drawShadows";
            shadowMapL       << "pointLights[" << lightID << "].shadowMap";
            shadowIntensityL << "pointLights[" << lightID << "].shadowIntensity";

            //prepareShadows();
        }

        void prepareShadows()
        {
            // configure depth map FBO
            // -----------------------
            glGenFramebuffers(1, &shadowFBO);
            // create depth cubemap texture

            glGenTextures(1, &shadowTex);
            glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTex);
            for (unsigned int i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            // attach depth texture as FBO's depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //unbind texture
        }

        void shadowMappingProjection(std::vector<ECS::Entity*> entitiesInScene)
        {
            // move light position over time
            //entity->getComponent<ECS::Transform>().position.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);

            // render
            // ------
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 0. create depth cubemap transformation matrices
            // -----------------------------------------------
            float near_plane = 0.0f;
            float far_plane = range;
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)shadowMapResolution / (float)shadowMapResolution, near_plane, far_plane);
            std::vector<glm::mat4> shadowTransforms;

            glm::vec3 lightPos = entity->getComponent<ECS::Transform>().position;

            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));



            // 1. render scene to depth cubemap
            // --------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
            glViewport(0, 0, shadowMapResolution, shadowMapResolution);

            //glClear(GL_DEPTH_BUFFER_BIT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GLCore::Render::ShaderManager::Get("pointLight_depth_shadows")->use();

            for (unsigned int i = 0; i < 6; ++i)
                GLCore::Render::ShaderManager::Get("pointLight_depth_shadows")->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

            GLCore::Render::ShaderManager::Get("pointLight_depth_shadows")->setFloat("far_plane[" + std::to_string(lightID) + "]", far_plane);
            GLCore::Render::ShaderManager::Get("pointLight_depth_shadows")->setVec3("lightPos[" + std::to_string(lightID) + "]", lightPos);

            //renderScene(simpleDepthShader);
            for (int i = 0; i < entitiesInScene.size(); i++)
            {
                if (entitiesInScene[i]->hascomponent<MeshRenderer>())
                {
                    if (entitiesInScene[i]->getComponent<MeshRenderer>().dropShadow)
                    {
                        GLCore::Render::ShaderManager::Get("pointLight_depth_shadows")->setMat4("model", entitiesInScene[i]->getComponent<MeshRenderer>().model_transform_matrix);
                        entitiesInScene[i]->getComponent<MeshRenderer>().bindVAO();
                    }
                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void draw() override 
        {
            for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
            {
                GLCore::Render::ShaderManager::Get(name.c_str())->use();
                GLCore::Render::ShaderManager::Get(name.c_str())->setBool(activeL.str().c_str(), active);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(positionL.str().c_str(), entity->getComponent<ECS::Transform>().position);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(ambientL.str().c_str(), ambient * hdrMultiply);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(diffuseL.str().c_str(), diffuse);
                GLCore::Render::ShaderManager::Get(name.c_str())->setVec3(specularL.str().c_str(), specular);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(strengthL.str().c_str(), strength);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(constantL.str().c_str(), constant);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(linearL.str().c_str(), linear);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(quadraticL.str().c_str(), quadratic);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(rangeL.str().c_str(), range);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(lightSmoothnessL.str().c_str(), lightSmoothness);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(specularPowerL.str().c_str(), specularPower);

                GLCore::Render::ShaderManager::Get(name.c_str())->setBool(drawShadowsL.str(), drawShadows);

                if (drawShadows)
                {
                    //Texture shadow
                    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //unbind texture
                    glActiveTexture(GL_TEXTURE0);              // donde SHADOW_SLOT es un índice que no se solape con tus otras texturas.
                    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTex);   // 'shadowTextureID' es el ID de la textura de sombra que creaste.
                    GLCore::Render::ShaderManager::Get(name.c_str())->setInt(shadowMapL.str(), 0);
                    GLCore::Render::ShaderManager::Get(name.c_str())->setFloat(shadowIntensityL.str().c_str(), shadowIntensity);
                }
                else
                {
                    glActiveTexture(GL_TEXTURE_CUBE_MAP);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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
            ImGui::Text("Component ID: %i", getTypeID());
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("Active", &active);
            ImGui::Checkbox("Debug", &debug);


            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
            ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(specular));
            
            
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderFloat("Specular Power", &specularPower, 0.0f, 100.0f);
            ImGui::SliderFloat("Strength", &strength, 0.0f, 30.0f);
            ImGui::InputFloat("hdr Multiply", &hdrMultiply, 0.1f, 0.1f);
            
            
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderFloat("Range", &range, 0.0f, 100.0f);
            ImGui::SliderFloat("Smoothness", &lightSmoothness, 0.0f, 1.0f);

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
            ImGui::Checkbox("Draw shadows", &drawShadows);
            if (drawShadows)
            {
                ImGui::SliderFloat("Shadow Intensity", &shadowIntensity, 0.0f, 1.0f);
                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Image((void*)(intptr_t)shadowFBO, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
            }

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }

    private:
        GLCore::MeshData sphereDebugMeshData;
        std::string currentShaderName = "pbr";

        

        //--Locations
        std::stringstream location;
        
        std::stringstream activeL;
        std::stringstream ambientL;
        std::stringstream diffuseL;
        std::stringstream specularL;
        std::stringstream positionL;
        std::stringstream strengthL;
        std::stringstream constantL;
        std::stringstream linearL;
        std::stringstream quadraticL;
        std::stringstream rangeL;
        std::stringstream lightSmoothnessL;
        std::stringstream specularPowerL;

        std::stringstream drawShadowsL;
        std::stringstream shadowIntensityL;
        std::stringstream shadowMapL;
        //----------------------------------------------




        void drawDebug()
        {
            glBindVertexArray(sphereDebugMeshData.VAO);

            glm::mat4 modelMatrix = glm::mat4(1.0f);

            float radiusValue = 1.0f;
            //if (showRange) radiusValue = range;

            glm::vec3 size = glm::vec3(radiusValue, radiusValue, radiusValue);
            modelMatrix = glm::translate(modelMatrix, entity->getComponent<ECS::Transform>().position) * glm::scale(glm::mat4(1), size);

            GLCore::Render::ShaderManager::Get("debug")->use();
            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", modelMatrix);
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(ambient.r, ambient.g, ambient.b, 1.0));


            // Dibujar la esfera
            glLineWidth(2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, sphereDebugMeshData.indexCount, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    };

}  // namespace ECS
