#include "SceneHierarchyPanel.h"
#include "../../glpch.h"
#include "Entity.h"
#include "Components.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include "../Util/IMGLoader.h"


namespace GLCore {
	GLCore::Scene* m_SceneContext;
	std::vector<const char*> shaderNames;

	SceneHierarchyPanel::SceneHierarchyPanel(GLCore::Scene* SceneContext)
	{
		// Generar lista de nombres de shaders
		for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
		{
			shaderNames.push_back(name.c_str());
		}
	}



	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		RegistrySingleton::getRegistry().each([&](auto entityID)
		{
			Entity entity{ entityID , m_SceneContext };
			if (entity.HasComponent<TransformComponent>())
			{
				// Comprobar si la entidad no tiene un componente ParentComponent o si tiene uno y su parentEntity es nula
				if (!entity.HasComponent<ParentComponent>() ||
					RegistrySingleton::getRegistry().get<ParentComponent>(entityID).parentEntity == entt::null)
				{
					DrawEntityNode(entity);
				}
			}
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();


		//BORRADO de las entidades
		for (auto& entity : m_EntitiesToBeDeleted) {
			m_SceneContext->DestroyEntity(entity);
		}
		m_EntitiesToBeDeleted.clear(); // Limpia la lista después de eliminar las entidades

	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}


	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		// Establece los flags base para todos los nodos
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		// Comprueba si la entidad tiene el componente ChildrenComponent y si tiene hijos
		bool hasChildren = entity.HasComponent<ChildrenComponent>() && !entity.GetComponent<ChildrenComponent>().childEntities.empty();

		// Si la entidad no tiene hijos, añade el flag ImGuiTreeNodeFlags_Leaf
		ImGuiTreeNodeFlags flags = base_flags | (hasChildren ? 0 : ImGuiTreeNodeFlags_Leaf);

		// Si la entidad está seleccionada, añade el flag ImGuiTreeNodeFlags_Selected
		flags |= ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);

		// La variable 'opened' es verdadera si el nodo está realmente abierto.
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			if (m_SelectionContext)
			{
				if (m_SelectionContext.HasComponent<MeshRendererComponent>())
				{
					m_SelectionContext.GetComponent<MeshRendererComponent>().drawLocalBB = false;
				}			
			}
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		// Asegúrate de que solo llamas a TreePop si TreeNodeEx ha devuelto verdadero.
		if (opened)
		{
			// Solo dibuja los hijos si hasChildren es verdadero.
			if (hasChildren)
			{
				auto& childrenComponent = entity.GetComponent<ChildrenComponent>();
				for (auto child : childrenComponent.childEntities)
				{
					Entity childEntity{ child, m_SceneContext };
					DrawEntityNode(childEntity); // Llamada recursiva para dibujar el nodo del hijo
				}
			}
			// Asegúrate de llamar a TreePop para equilibrar la pila de ImGui.
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			if (entity.HasComponent<ChildrenComponent>())
			{
				auto& children = entity.GetComponent<ChildrenComponent>().childEntities;
				auto childrenCopy = children;
				for (auto child : childrenCopy)
				{
					Entity childEntity{ child, m_SceneContext };
					m_EntitiesToBeDeleted.push_back(childEntity);
				}
			}

			m_EntitiesToBeDeleted.push_back(entity);

			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	//--DRAW COMPNENTS
	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		// Slider para X
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X"))
			values.x = resetValue;
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		//ImGui::SameLine();

		// Slider para Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y"))
			values.y = resetValue;
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		//ImGui::SameLine();

		// Slider para Z
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z"))
			values.z = resetValue;
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}
	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = 10;// GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");



		if (ImGui::BeginPopup("AddComponent"))
		{
			ImGui::Text("New Component");
			DisplayAddComponentEntry<TransformComponent>("Transform");
			DisplayAddComponentEntry<MeshFilterComponent>("MeshFilter");
			DisplayAddComponentEntry<MeshRendererComponent>("Mesh Renderer");
			DisplayAddComponentEntry<MaterialComponent>("Material");

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();




		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{	
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			DrawVec3Control("Position", component.position);

			glm::vec3 eulerAnglesRadians = glm::eulerAngles(component.rotation); // Convierte el quaternion a un vector de Euler en radianes
			glm::vec3 rotationDegrees = glm::degrees(eulerAnglesRadians); // Convierte los radianes a grados

			DrawVec3Control("Rotation", rotationDegrees);
			eulerAnglesRadians = glm::radians(rotationDegrees); // Convierte grados a radianes

			// Crea un cuaternión a partir de los ángulos de Euler en radianes
			glm::quat quaternion = glm::quat(eulerAnglesRadians);

			// Asigna este cuaternión a component.rotation
			component.rotation = quaternion;
			DrawVec3Control("Scale", component.scale, 1.0f,100);
		});
		DrawComponent<ParentComponent>("Parent", entity, [](auto& component)
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

		ImGui::Text("Entity Parent:");
		});
		DrawComponent<ChildrenComponent>("Child", entity, [](auto& component)
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			ImGui::Text("HIJOS: %i", component.childEntities.size());
		});
		DrawComponent<MeshFilterComponent>("Mesh Filter", entity, [](auto& component)
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			// Muestra los valores en la ventana usando etiquetas y valores
			ImGui::Text("Model type: %s", ModelTypeToString(component.modelType).c_str());
			if (component.modelType == GLCore::MODEL_TYPES::EXTERNAL_FILE)
			{
				ImGui::Text("Model path: %s", component.modelPath.c_str());
			}

			ImGui::Text("Index Count: %u", component.meshData->indexCount);
			ImGui::Text("Number of Triangles: %u", component.meshData->indexCount / 3);

			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		});
		DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [](auto& component)
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			ImGui::Checkbox("Visible Model", &component.visibleModel);
			ImGui::Checkbox("Draw Local Bounding Box", &component.drawLocalBB);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Text("Current Shader: %s", component.currentShaderName.c_str());

			static int selectedItem = -1;  // El índice del elemento seleccionado, -1 significa que ningún elemento está seleccionado
			const char* comboPreviewContent = selectedItem >= 0 ? shaderNames[selectedItem] : "Select a shader...";  // Lo que se mostrará cuando no se haya desplegado la lista

			if (ImGui::BeginCombo("Shaders", comboPreviewContent))
			{
				for (int i = 0; i < shaderNames.size(); i++)
				{
					bool isSelected = selectedItem == i;
					if (ImGui::Selectable(shaderNames[i], isSelected))
					{
						selectedItem = i;
						component.currentShaderName = shaderNames[i];
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
		});
		DrawComponent<MaterialComponent>("Material", entity, [this](auto& component)
		{
			ImGui::SliderFloat("HDR INTENSITY", &component.hdrIntensity, 0.0f, 1.0f, "%.3f");
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			ImGui::SliderFloat("HDR Exposure", &component.exposure, 0.0f, 10.0f, "%.3f");
			ImGui::SliderFloat("HDR Gamma", &component.gamma, 0.0f, 10.0f, "%.3f");
			ImGui::SliderFloat("MAX REFLECTION LOD", &component.max_reflection_lod, 0.0f, 10.0f, "%.3f");
			ImGui::SliderFloat("IBL Intensity", &component.iblIntensity, 0.0f, 1.0f, "%.5f");

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::SliderInt2("Texture Repetition", glm::value_ptr(component.repetitionFactor), 1, 20);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			std::vector<Ref<Texture>> textures;

			textures.push_back(component.materialData->albedoMap);
			textures.push_back(component.materialData->normalMap);
			textures.push_back(component.materialData->metallicMap);
			textures.push_back(component.materialData->rougnessMap);
			textures.push_back(component.materialData->aOMap);

			for (size_t i = 0; i < textures.size(); i++)
			{
				Ref<Texture> texture = textures[i];

				ImGui::SetWindowFontScale(1.3); // 150% del tamaño original
				ImGui::Text("##%s", texture->typeString.c_str());
				ImGui::SetWindowFontScale(1.0); // Vuelve al tamaño original

			//	//--drop place
				ImGui::Image((void*)(intptr_t)texture->textureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

				SceneHierarchyPanel::HandleDragDropForTexture(texture, component, "ASSET_DRAG");

				if (texture->hasMap)
				{
					std::string treeNodeTitle = "Details: " + texture->typeString;
					if (ImGui::TreeNode(treeNodeTitle.c_str())) {
						ImGui::Text("File: %s", texture->image->path.c_str());
						ImGui::Text("Width: %i", texture->image->width);
						ImGui::Text("Height: %i", texture->image->height);
						ImGui::Text("Channels: %i", texture->image->channels);

						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::Text("Drop in black box");
				}



				if (texture->type == GLCore::TEXTURE_TYPES::ALBEDO)
				{
					ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
					ImGui::InputFloat("hdr Multiply", &component.hdrMultiply, 0.001f, 0.001f);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::NORMAL)
				{
					ImGui::SliderFloat("Normal Intensity", &component.normalIntensity, 0.0f, 1.0f);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::METALLIC)
				{
					ImGui::SliderFloat("Metallic Value", &component.metallicValue, 0.0f, 1.0f);
					ImGui::SliderFloat("Reflectance", &component.reflectanceValue, 0.0f, 1.0f, "%.2f");
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::ROUGHNESS)
				{
					ImGui::SliderFloat("Roughtness Value", &component.roughnessValue, 0.05f, 1.0f);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(10.0f, 20.0f));
				}
				else if (texture->type == GLCore::TEXTURE_TYPES::AO)
				{
					ImGui::Separator();
				}
			}
		});
		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [this](auto& component)
		{
			ImGui::Text("PointLight");
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Checkbox("Active", &component.active);
			ImGui::Checkbox("Debug", &component.debug);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::SliderFloat("Orbit X", &component.angleX, 0.0f, 6.28319f);
			ImGui::SliderFloat("Orbit Y", &component.angleY, 0.0f, 6.28319f);
			ImGui::SliderFloat("Scene Radius Offset", &component.sceneRadiusOffset, -(component.currentSceneRadius * 10.0f), (component.currentSceneRadius * 10.0f), "%.1f");

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::ColorEdit3("Ambient", glm::value_ptr(component.ambient));
			ImGui::ColorEdit3("Diffuse", glm::value_ptr(component.diffuse));
			ImGui::ColorEdit3("Specular", glm::value_ptr(component.specular));


			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Checkbox("Draw Shadows", &component.drawShadows);

			if (component.drawShadows == true)
			{
				ImGui::SliderFloat("Shadow Intensity", &component.shadowIntensity, 0.0f, 1.0f);
				ImGui::Checkbox("Use Poison Disk", &component.usePoisonDisk);

				ImGui::Dummy(ImVec2(0.0f, 20.0f));
				ImGui::Image((void*)(intptr_t)component.shadowTex, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));

				ImGui::Dummy(ImVec2(0.0f, 20.0f));
				ImGui::SliderFloat("Left", &component.orthoLeft, -100.0f, 100.0f);
				ImGui::SliderFloat("Right", &component.orthoRight, -100.0f, 100.0f);
				ImGui::SliderFloat("Top", &component.orthoTop, -100.0f, 100.0f);
				ImGui::SliderFloat("Bottom", &component.orthoBottom, -100.0f, 100.0f);

				ImGui::SliderFloat("Near", &component.orthoNear, 0.0f, 100.0f);
				ImGui::SliderFloat("Near Offset", &component.orthoNearOffset, -100.0f, 100.0f);
				ImGui::SliderFloat("Far", &component.orthoFar, 0.0f, 500.0f);
				ImGui::SliderFloat("Far Offset", &component.orthoFarOffset, -100.0f, 100.0f);

				ImGui::Dummy(ImVec2(0.0f, 20.0f));
				if (ImGui::CollapsingHeader("Shadow Bias")) {
					ImGui::InputFloat("00", &component.shadowBias[0][0], 0.001f);
					ImGui::InputFloat("01", &component.shadowBias[0][1], 0.001f);
					ImGui::InputFloat("02", &component.shadowBias[0][2], 0.001f);
					ImGui::InputFloat("03", &component.shadowBias[0][3], 0.001f);

					ImGui::InputFloat("10", &component.shadowBias[1][0], 0.001f);
					ImGui::InputFloat("11", &component.shadowBias[1][1], 0.001f);
					ImGui::InputFloat("12", &component.shadowBias[1][2], 0.001f);
					ImGui::InputFloat("13", &component.shadowBias[1][3], 0.001f);

					ImGui::InputFloat("20", &component.shadowBias[2][0], 0.001f);
					ImGui::InputFloat("21", &component.shadowBias[2][1], 0.001f);
					ImGui::InputFloat("22", &component.shadowBias[2][2], 0.001f);
					ImGui::InputFloat("23", &component.shadowBias[2][3], 0.001f);

					ImGui::InputFloat("30", &component.shadowBias[3][0], 0.001f);
					ImGui::InputFloat("31", &component.shadowBias[3][1], 0.001f);
					ImGui::InputFloat("32", &component.shadowBias[3][2], 0.001f);
					ImGui::InputFloat("33", &component.shadowBias[3][3], 0.001f);

					if (ImGui::Button("Reset")) {
						component.shadowBias = glm::mat4(
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
		});
	}

	void SceneHierarchyPanel::HandleDragDropForTexture(Ref<Texture> texture, MaterialComponent& materialComponent, const char* payloadType) {
		// Crear un área de drop. Si algo está siendo arrastrado sobre este área (que cumple con payloadType), resáltalo
		if (ImGui::BeginDragDropTarget()) {
			// Aceptar una carga útil de tipo payloadType. La carga útil necesita contener el identificador de la textura.
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType);
			if (payload != nullptr && payload->IsDataType(payloadType)) {

				if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
				{
					std::string dropped_fpath = (const char*)payload->Data;

					texture->image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
					texture->Bind();
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
	//----------------------------------------------------------------------------------
}
