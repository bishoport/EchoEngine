#pragma once

#include "../../glpch.h"

#include <string>
#include "../Core/Components.h"

namespace GLCore::Render {

	static void ActiveTextures(MaterialComponent* materialComponent,int slotTextureCounter)
    {
        int textureChannelInit = slotTextureCounter;

        // Nombres de los mapas
        std::vector<std::string> mapNames = { "albedo", "normal", "metallic", "roughness", "ao" };

        std::vector<Ref<Texture>> textures;

        textures.push_back(materialComponent->materialData->albedoMap);
        textures.push_back(materialComponent->materialData->normalMap);
        textures.push_back(materialComponent->materialData->metallicMap);
        textures.push_back(materialComponent->materialData->rougnessMap);
        textures.push_back(materialComponent->materialData->aOMap);

        // Iteramos sobre cada mapa en el vector 'textures'
        for (size_t i = 0; i < textures.size(); i++) 
        {

            glActiveTexture(GL_TEXTURE0 + textureChannelInit);
            glBindTexture(GL_TEXTURE_2D, textures[i]->textureID);
            GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->use();
            GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setInt("material." + mapNames[i] + "Map", textureChannelInit);
            GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setBool("material.has" + std::string(1, toupper(mapNames[i][0])) + mapNames[i].substr(1) + "Map", true);
            textureChannelInit++;
        }

        // Establece los valores comunes
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->use();
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setVec3("material.albedo",              materialComponent->materialData->color);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.hdrMultiply",        materialComponent->materialData->hdrMultiply);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.metallic",           materialComponent->materialData->metallicValue);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.roughness",          materialComponent->materialData->roughnessValue);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.normalIntensity",    materialComponent->materialData->normalIntensity);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.reflectance",        materialComponent->materialData->reflectanceValue);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.fresnelCoef",        materialComponent->materialData->fresnelCoefValue);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.hdrIntensity",       materialComponent->materialData->hdrIntensity);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.exposure",           materialComponent->materialData->exposure);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.gamma",              materialComponent->materialData->gamma);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.max_reflection_lod", materialComponent->materialData->max_reflection_lod);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setFloat("material.iblIntensity",       materialComponent->materialData->iblIntensity);
        GLCore::Render::ShaderManager::Get(materialComponent->currentShaderName)->setVec2("repetitionFactor",             materialComponent->repetitionFactor);
    }
    static void DrawDirectionalLight(DirectionalLightComponent* directionalLightComponent, TransformComponent* transformComponent)
    {

        if (directionalLightComponent->drawShadows)
        {
            //Texture shadow
            glActiveTexture(GL_TEXTURE0 + 15);
            glBindTexture(GL_TEXTURE_2D, directionalLightComponent->shadowTex);
        }
        else
        {
            glActiveTexture(GL_TEXTURE0 + 15);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
        {
            GLCore::Render::ShaderManager::Get(name.c_str())->use();
            GLCore::Render::ShaderManager::Get(name.c_str())->setBool("dirLight.isActive",  directionalLightComponent->active);
            GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.direction", directionalLightComponent->direction);
            GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.position",  transformComponent->position);
            GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.ambient",   directionalLightComponent->ambient);
            GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.diffuse", directionalLightComponent->diffuse);
            GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("dirLight.specular", directionalLightComponent->specular);
            GLCore::Render::ShaderManager::Get(name.c_str())->setBool("dirLight.drawShadows", directionalLightComponent->drawShadows);

            if (directionalLightComponent->drawShadows)
            {
                GLCore::Render::ShaderManager::Get(name.c_str())->setInt("dirLight.shadowMap", 15);
                GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("dirLight.shadowBiasMVP", directionalLightComponent->shadowBias * directionalLightComponent->shadowMVP);
                GLCore::Render::ShaderManager::Get(name.c_str())->setFloat("dirLight.shadowIntensity", directionalLightComponent->shadowIntensity);
                GLCore::Render::ShaderManager::Get(name.c_str())->setBool("dirLight.usePoisonDisk", directionalLightComponent->usePoisonDisk);
            }
            
        }

        //--DEBUG LIGHT
        if (directionalLightComponent->debug)
        {
            // Crea un array con los puntos de la línea
            glm::vec3 rotationVector = directionalLightComponent->direction;

            glm::vec3 linePoints[] = {
                transformComponent->position,
                transformComponent->position + rotationVector * 10.0f  // Aquí asumimos que la longitud de la línea es 10
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
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(directionalLightComponent->ambient.r, directionalLightComponent->ambient.g, directionalLightComponent->ambient.b, 1.0));
            glLineWidth(2);

            glBindVertexArray(lineVAO);
            glDrawArrays(GL_LINES, 0, 2);

            // Elimina el VBO y el VAO de la línea
            glDeleteBuffers(1, &lineVBO);
            glDeleteVertexArrays(1, &lineVAO);
        }
    }
    static void DrawBoundingBox(MeshRendererComponent* meshRendererComponent)
    {
        // Configura y usa un shader simple para dibujar la Bounding Box
        GLCore::Render::ShaderManager::Get("debug")->use();
        GLCore::Render::ShaderManager::Get("debug")->setMat4("model", meshRendererComponent->model_transform_matrix);
        GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(1.0, 0.5, 0.2, 1.0));

        if (meshRendererComponent->drawLocalBB)
        {
            // Dibuja la Bounding Box
            glLineWidth(5);
            glBindVertexArray(meshRendererComponent->meshData->VAO_BB);
            glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}
