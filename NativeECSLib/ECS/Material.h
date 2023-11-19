#pragma once
#include "../src/glpch.h"
#include "../src/GLCore/DataStruct.h"
#include "Entity.h"
#include "Component.h"
#include "Transform.h"

#include "../src/GLCore/Util/IMGLoader.h"
#include "MeshRenderer.h"

namespace ECS {

    class Material : public Component {

    public:

        Material() {}

        void init() override 
        {
            material = std::make_unique<GLCore::Material>();
            material->color = glm::vec3(1.0f,1.0f,1.0f);
            material->shininess = 32.0f;
        }

		ComponentID getComponentID() const
		{
			return getComponentTypeID<Material>();
		}

		void onDestroy() override
		{
			Component::onDestroy();

			// Libera las texturas asociadas con el material.
			for (auto& texture : material->textures) {
				if (texture->hasMap) {
					// Liberar la textura de la GPU si se ha cargado.
					glDeleteTextures(1, &texture->textureID);
					texture->hasMap = false;
				}
			}

			// En este punto, puedes también asegurarte de que todos los punteros inteligentes
			// sean reseteados o cualquier otro recurso que requiera una limpieza específica.
			material.reset();

			//markForDeletion();
		}

		void setDafaultMaterial()
		{
			material->albedoMap.image = GLCore::Utils::ImageLoader::loadImage("assets/default/default_white.jpg");
			material->albedoMap.image.path = "assets/default/default_white.jpg";
			material->albedoMap.hasMap = true;

			material->normalMap.image = GLCore::Utils::ImageLoader::loadImage("assets/default/default_normal.jpg");
			material->normalMap.image.path = "assets/default/default_normal.jpg";
			material->normalMap.hasMap = true;

			material->metallicMap.image = GLCore::Utils::ImageLoader::loadImage("assets/default/default_black.jpg");
			material->metallicMap.image.path = "assets/default/default_black.jpg";
			material->metallicMap.hasMap = true;

			material->rougnessMap.image = GLCore::Utils::ImageLoader::loadImage("assets/default/default_black.jpg");
			material->rougnessMap.image.path = "assets/default/default_black.jpg";
			material->rougnessMap.hasMap = true;

			material->aOMap.image = GLCore::Utils::ImageLoader::loadImage("assets/default/default_white.jpg");
			material->aOMap.image.path = "assets/default/default_white.jpg";
			material->aOMap.hasMap = true;

			material->prepare_PBRMaterials();
		}

		void setMaterial(GLCore::Material& mat)
		{
			*material = mat;
			material->prepare_PBRMaterials();
		}

        void draw() override 
        {
			if (entity->hascomponent<MeshRenderer>())
			{

				currentShaderName = entity->getComponent<MeshRenderer>().currentShaderName;

				GLCore::Render::ShaderManager::Get(currentShaderName)->use();
				GLCore::Render::ShaderManager::Get(currentShaderName)->setVec2("repetitionFactor", repetitionFactor);
				material->bindTextures(currentShaderName);

			}
			
        }

        void drawGUI_Inspector() override
        {
            ImGui::Text("Material");
			ImGui::Text("Component ID: %i", getComponentID());
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			if (ImGui::Button("Delete Material")) {
				entity->removeComponent<Material>();
				return;
			}

			ImGui::SliderFloat("HDR INTENSITY", &material->hdrIntensity, 0.0f, 1.0f, "%.3f");
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			
			ImGui::SliderFloat("HDR Exposure", &material->exposure, 0.0f, 10.0f, "%.3f");
			ImGui::SliderFloat("HDR Gamma", &material->gamma, 0.0f, 10.0f, "%.3f");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderInt2("Texture Repetition", glm::value_ptr(repetitionFactor), 1, 20);
			ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
			drawTextureProperties();
        }


		// Método para serializar el MeshFilter a YAML
		void serialize(YAML::Emitter& out) const {
			out << YAML::BeginMap;

			// Serializar propiedades simples
			out << YAML::Key << "shininess" << YAML::Value << material->shininess;
			out << YAML::Key << "hdrIntensity" << YAML::Value << material->hdrIntensity;
			out << YAML::Key << "exposure" << YAML::Value << material->exposure;
			out << YAML::Key << "gamma" << YAML::Value << material->gamma;
			out << YAML::Key << "tiling" << YAML::Value << repetitionFactor;

			//Albedo
			out << YAML::Key << "albedoMap"   << YAML::Value << material->albedoMap.image.path;
			out << YAML::Key << "color" << YAML::Flow << YAML::BeginSeq << material->color.r << material->color.g << material->color.b << YAML::EndSeq;
			out << YAML::Key << "hdrMultiply" << YAML::Value << material->hdrMultiply;

			//Normal
			out << YAML::Key << "normalMap"   << YAML::Value << material->normalMap.image.path;
			out << YAML::Key << "normalIntensiry"   << YAML::Value << material->normalIntensity;

			//Metallic
			out << YAML::Key << "metallicMap" << YAML::Value << material->metallicMap.image.path;
			out << YAML::Key << "metallicValue" << YAML::Value << material->metallicValue;
			out << YAML::Key << "reflectanceValue" << YAML::Value << material->reflectanceValue;

			//Rougness
			out << YAML::Key << "rougnessMap" << YAML::Value << material->rougnessMap.image.path;
			out << YAML::Key << "rougnessValue" << YAML::Value << material->roughnessValue;

			//AO
			out << YAML::Key << "aOMap" << YAML::Value << material->aOMap.image.path;

			out << YAML::EndMap;
		}




		// Método para deserializar el MeshFilter desde YAML
		void deserialize(const YAML::Node& node) {
			/*modelType = static_cast<GLCore::MODEL_TYPES>(node["modelType"].as<int>());
			modelPath = node["modelPath"].as<std::string>();*/
			// Agrega aquí la deserialización de cualquier otro miembro
		}



    private:
        std::unique_ptr<GLCore::Material> material = nullptr;
        std::string currentShaderName = "pbr_ibl";
        glm::ivec2 repetitionFactor = glm::ivec2(1, 1);

		void drawTextureProperties()
		{
			ImGui::Dummy(ImVec2(10.0f, 20.0f));

			for (size_t i = 0; i < material->textures.size(); i++) {

				auto texture = material->textures[i];

				ImGui::SetWindowFontScale(1.3); // 150% del tamaño original
				ImGui::Text("%s", texture->typeString.c_str());
				ImGui::SetWindowFontScale(1.0); // Vuelve al tamaño original

				//--drop place
				ImGui::Image((void*)(intptr_t)texture->textureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));
				HandleDragDropForTexture(*texture, "ASSET_DRAG");

				if (texture->hasMap)
				{
					std::string treeNodeTitle = "Details: " + texture->typeString;
					if (ImGui::TreeNode(treeNodeTitle.c_str())) {
						ImGui::Text("File: %s", texture->image.path.c_str());
						ImGui::Text("Width: %i", texture->image.width);
						ImGui::Text("Height: %i", texture->image.height);
						ImGui::Text("Channels: %i", texture->image.channels);

						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::Text("Drop in black box");
				}
					


				if (texture->type == GLCore::TEXTURE_TYPES::ALBEDO)
				{
					ImGui::ColorEdit3("Color", glm::value_ptr(material->color));
					ImGui::InputFloat("hdr Multiply", &material->hdrMultiply, 0.001f, 0.001f);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::NORMAL)
				{
					ImGui::SliderFloat("Normal Intensity", &material->normalIntensity, 0.0f, 1.0f);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::METALLIC)
				{
					ImGui::SliderFloat("Metallic Value", &material->metallicValue, 0.0f, 1.0f);
					ImGui::SliderFloat("Reflectance", &material->reflectanceValue, 0.0f, 1.0f, "%.2f");
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::ROUGHNESS)
				{
					ImGui::SliderFloat("Roughtness Value", &material->roughnessValue, 0.05f, 1.0f);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::AO)
				{
					ImGui::Separator();
				}
			}
		}

		void HandleDragDropForTexture(GLCore::Texture& matTex, const char* assetTag)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(assetTag))
				{
					if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
					{
						std::string dropped_fpath = (const char*)payload->Data;

						// Nota: puedes agregar una condición aquí para liberar la imagen si es necesario
						/*if (matTex.hasTexture)
							GLCore::Utils::ImageLoader::freeImage(matTex.image);*/

						matTex.image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
						material->prepare_PBRMaterials();
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
    };

}  // namespace ECS
