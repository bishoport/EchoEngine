#pragma once
#include "ECS.h"
#include "../GLCore/Util/IMGLoader.h"

namespace ECS {

    class Material : public Component {

    public:

        Material() {}

        void init() override 
        {
            material = std::make_unique<GLCore::Material>();
            material->albedoColor = glm::vec3(1.0f,1.0f,1.0f);
            material->shininess = 32.0f;
        }

		void setMaterial(GLCore::Material& mat)
		{
			*material = mat;
			material->prepare_PBRMaterials();
		}


        void draw() override 
        {
            currentShaderName = entity->getComponent<MeshRenderer>().currentShaderName;

            GLCore::Render::ShaderManager::Get(currentShaderName)->use();
			GLCore::Render::ShaderManager::Get(currentShaderName)->setVec2("repetitionFactor", repetitionFactor);
            material->bindTextures(currentShaderName); 
        }



        void drawGUI_Inspector() override
        {
            ImGui::Text("Material");
			ImGui::SliderFloat("HDR INTENSITY", &material->hdrIntensity, 0.0f, 1.0f, "%.001f");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderInt2("Texture Repetition", glm::value_ptr(repetitionFactor), 1, 20);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
			DropCheckerPBR();
            
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }

    private:
        std::unique_ptr<GLCore::Material> material = nullptr;
        std::string currentShaderName = "pbr";
        glm::ivec2 repetitionFactor = glm::ivec2(1, 1);


        void DropCheckerPBR()
        {
			//---ALBEDO
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material->albedoColor));
			ImGui::InputFloat("hdr Multiply", &material->hdrMultiply, 0.001f, 0.001f);
			if (material->hasAlbedoMap)
				ImGui::Text(material->albedoMap.image.path.c_str());
			else
				ImGui::Text("Drop in black box");
			ImGui::Image((void*)(intptr_t)material->albedoMap.textureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));


			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_DRAG"))
				{
					if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
					{
						std::string dropped_fpath = (const char*)payload->Data;

						/*if (material->hasAlbedoMap)
							GLCore::Utils::ImageLoader::freeImage(material->albedoMap.image);*/

						material->albedoMap.image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
						material->prepare_PBRMaterials();
						
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Dummy(ImVec2(10.0f, 10.0f));
			//------------------------------------------------------------------------ 



			//---NORMAL
			ImGui::Text("Normal");

			ImGui::SliderFloat("Normal Intensity", &material->normalIntensity, 0.0f, 1.0f);
			if (material->hasNormalMap)
				ImGui::Text(material->normalMap.image.path.c_str());
			else
				ImGui::Text("Drop in black box");

			ImGui::Image((void*)(intptr_t)material->normalMap.textureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_DRAG"))
				{
					if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
					{
						std::string dropped_fpath = (const char*)payload->Data;

						/*if (material->hasNormalMap)
							GLCore::Utils::ImageLoader::freeImage(material->normalMap.image);*/

						material->normalMap.image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
						material->prepare_PBRMaterials();

					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Dummy(ImVec2(10.0f, 10.0f));
			//------------------------------------------------------------------------ 

			//---METALLIC
			ImGui::Text("Metallic");
			ImGui::SliderFloat("Metallic Value", &material->metallicValue, 0.0f, 1.0f);
			ImGui::SliderFloat("F0", &material->reflectanceValue, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Fresnel Co", &material->fresnelCoefValue, 0.0f, 10.0f, "%.2f");
			if (material->hasMetallicMap)
				ImGui::Text(material->metallicMap.image.path.c_str());
			else
				ImGui::Text("Drop in black box");

			ImGui::Image((void*)(intptr_t)material->metallicMap.textureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_DRAG"))
				{
					if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
					{
						std::string dropped_fpath = (const char*)payload->Data;

						/*if (material->hasMetallicMap)
							GLCore::Utils::ImageLoader::freeImage(material->metallicMap.image);*/

						material->metallicMap.image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
						material->prepare_PBRMaterials();

					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Dummy(ImVec2(10.0f, 10.0f));
			//------------------------------------------------------------------------ 



			//---ROUGHNESS
			ImGui::Text("Roughness");
			ImGui::SliderFloat("Roughtness Value", &material->roughnessValue, 0.05f, 1.0f);
			if (material->hasRougnessMap)
				ImGui::Text(material->rougnessMap.image.path.c_str());
			else
				ImGui::Text("Drop in black box");

			ImGui::Image((void*)(intptr_t)material->rougnessMap.textureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_DRAG"))
				{
					if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
					{
						std::string dropped_fpath = (const char*)payload->Data;

						/*if (material->hasRougnessMap)
							GLCore::Utils::ImageLoader::freeImage(material->rougnessMap.image);*/

						material->rougnessMap.image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
						material->prepare_PBRMaterials();
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Dummy(ImVec2(10.0f, 10.0f));
			//------------------------------------------------------------------------ 



			//---AO
			ImGui::Text("AO");

			if (material->hasAoMap)
				ImGui::Text(material->aOMap.image.path.c_str());
			else
				ImGui::Text("Drop in black box");

			ImGui::Image((void*)(intptr_t)material->aOMap.textureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_DRAG"))
				{
					if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
					{
						std::string dropped_fpath = (const char*)payload->Data;

						if (material->hasAoMap)
							GLCore::Utils::ImageLoader::freeImage(material->aOMap.image);

						material->aOMap.image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
						material->prepare_PBRMaterials();
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Dummy(ImVec2(10.0f, 10.0f));
			//------------------------------------------------------------------------ 
        }
    };

}  // namespace ECS
