#include "ScriptableGameObjectManager.h"
#include <iostream>

#include "ECS_SCRIPTING/ScriptableComponent.h"


namespace Scripting
{
	//-DLL------------------------------------------------------------------------
	ScriptableGameObjectManager::ScriptableGameObjectManager() {}
	ScriptableGameObjectManager::~ScriptableGameObjectManager()
	{
		if (m_ptrMonoDomain)
		{
			mono_jit_cleanup(m_ptrMonoDomain);
		}
	}
	//----------------------------------------------------------------------------

	

	ECS_SCRIPTING::ScriptableEntity& ScriptableGameObjectManager::CreateEmptyGameObject()
	{
		// Agregar una nueva entidad al manager y obtener una referencia a ella.
		ECS_SCRIPTING::ScriptableEntity* entity = &manager.addEntity();
		entity->addComponentScriptableComponent("Transform");
		m_SelectedEntity = entity;
		return *entity;
	}

	void ScriptableGameObjectManager::addComponentToSelectedGameObject(MainMenuAction action)
	{

	}

	//Operaciones desde menu TOP
	void ScriptableGameObjectManager::createPresetGameObject(MainMenuAction action)
	{
		if (action == MainMenuAction::AddEmpty)
		{
			CreateEmptyGameObject();
		}
	}





	//C# REFLECTIONS
	/*MonoObject* ScriptableGameObjectManager::CreateCsComponent(const std::string& className)
	{
		MonoClass* monoClass = mono_class_from_name(m_ptrCsEngineAssemblyImage, "CsEngineScript.ECS", className.c_str());
		if (monoClass)
		{
			MonoObject* component = mono_object_new(m_ptrMonoDomain, monoClass);
			if (component)
			{
				mono_runtime_object_init(component);
				m_Classes[className] = monoClass;
				m_Instances[className] = component;
				return component;
			}
		}

		return nullptr;
	}*/

	void ScriptableGameObjectManager::InvokeMethod(const std::string& className, const std::string& methodName, void** params, int paramCount)
	{
		/*if (m_Instances.find(className) != m_Instances.end())
		{
			MonoMethod* method = mono_class_get_method_from_name(m_Classes[className], methodName.c_str(), paramCount);
			if (method)
			{
				mono_runtime_invoke(method, m_Instances[className], params, nullptr);
			}
		}*/
	}
	MonoObject* ScriptableGameObjectManager::GetCsComponentProperty(const std::string& className, const std::string& propertyName)
	{
		/*if (m_Instances.find(className) != m_Instances.end())
		{
			MonoMethod* getPropertyMethod = mono_class_get_method_from_name(m_Classes[className], "GetSerializableProperty", 1);
			if (getPropertyMethod)
			{
				void* args[1];
				MonoString* monoPropertyName = mono_string_new(m_ptrMonoDomain, propertyName.c_str());
				args[0] = monoPropertyName;

				return mono_runtime_invoke(getPropertyMethod, m_Instances[className], args, nullptr);
			}
		}*/
		return nullptr;
	}
	void ScriptableGameObjectManager::SetCsComponentProperty(const std::string& className, const std::string& propertyName, MonoObject* value)
	{
		/*if (m_Instances.find(className) != m_Instances.end())
		{
			MonoMethod* setPropertyMethod = mono_class_get_method_from_name(m_Classes[className], "SetSerializableProperty", 2);
			if (setPropertyMethod)
			{
				void* args[2];
				MonoString* monoPropertyName = mono_string_new(m_ptrMonoDomain, propertyName.c_str());
				args[0] = monoPropertyName;
				args[1] = value;

				mono_runtime_invoke(setPropertyMethod, m_Instances[className], args, nullptr);
			}
		}*/
	}





	void ScriptableGameObjectManager::loadFileModel(GLCore::ImportOptions importOptions)
	{
	}
	void ScriptableGameObjectManager::drawHierarchy()
	{
		////-------------------------------------------HIERARCHY PANEL--------------------------------------
		//ImGui::Begin("Hierarchy", nullptr);

		//std::vector<ECS_SCRIPTING::ScriptableEntity*> allEntities = manager.getAllEntities();
		//for (ECS_SCRIPTING::ScriptableEntity* entity : allEntities) {

		//	ECS_SCRIPTING::ScriptableComponent* componentTransform = entity->getComponent("Transform");

		//	if (componentTransform != nullptr)
		//	{
		//		if (componentTransform.parent == nullptr) // Only root entities
		//		{
		//			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		//			if (m_SelectedEntity == manager.getAllEntities()[i])
		//				node_flags |= ImGuiTreeNodeFlags_Selected;

		//			std::string treeLabel = manager.getAllEntities()[i]->name;

		//			bool treeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, treeLabel.c_str());

		//			if (ImGui::IsItemClicked()) // Select on (left) click
		//			{
		//				if (m_SelectedEntity != nullptr)
		//				{
		//					if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
		//					{
		//						m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = false;
		//					}
		//				}
		//				m_SelectedEntity = manager.getAllEntities()[i];
		//			}

		//			if (m_SelectedEntity != nullptr)
		//			{
		//				if (ImGui::BeginPopupContextItem()) { // Click derecho en el elemento
		//					if (ImGui::MenuItem("Delete"))
		//					{
		//						m_SelectedEntity->active = false;
		//						m_SelectedEntity->markedToDelete = true;
		//						m_SelectedEntity = nullptr;
		//					}
		//					ImGui::EndPopup();
		//				}
		//			}

		//			//--DRAG_DROP
		//			// Si comienza a arrastrar el nodo
		//			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		//			{
		//				ImGui::SetDragDropPayload("DRAG_ENTITY", &i, sizeof(int)); // Envía el índice de la entidad
		//				ImGui::Text("Drag %s", treeLabel.c_str());
		//				ImGui::EndDragDropSource();
		//			}

		//			// Si un nodo es arrastrado sobre otro nodo
		//			if (ImGui::BeginDragDropTarget())
		//			{
		//				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_ENTITY"))
		//				{
		//					int sourceEntityIndex = *(const int*)payload->Data;
		//					// Asegúrate de que los índices estén dentro de los límites
		//					if (sourceEntityIndex >= 0 && sourceEntityIndex < manager.getAllEntities().size() && sourceEntityIndex != i)
		//					{
		//						ECS::Entity* sourceEntity = manager.getAllEntities()[sourceEntityIndex]; // Entidad que estás arrastrando
		//						ECS::Entity* targetEntity = manager.getAllEntities()[i]; // Entidad sobre la cual se hace el "drop"
		//						// Asegúrate de que la entidad objetivo no sea hija de la entidad fuente
		//						if (!sourceEntity->getComponent<ECS::Transform>().isEntityChildOf(sourceEntity, targetEntity))
		//						{
		//							sourceEntity->getComponent<ECS::Transform>().setParent(targetEntity);
		//						}
		//					}
		//				}
		//				ImGui::EndDragDropTarget();
		//			}
		//			//--END DRAG_DROP




		//			if (treeOpen)
		//			{
		//				for (int j = 0; j < manager.getAllEntities()[i]->getComponent<ECS::Transform>().children.size(); j++)
		//				{
		//					auto& child = manager.getAllEntities()[i]->getComponent<ECS::Transform>().children[j];

		//					ImGuiTreeNodeFlags child_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		//					if (m_SelectedEntity == child)
		//						child_flags |= ImGuiTreeNodeFlags_Selected;

		//					std::string childLabel = child->name;

		//					ImGui::TreeNodeEx((void*)(intptr_t)(i * 1000 + j), child_flags, childLabel.c_str()); // Ensure the ID is unique
		//					if (ImGui::IsItemClicked())
		//					{
		//						if (m_SelectedEntity)
		//						{
		//							if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
		//							{
		//								m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = false;
		//							}
		//						}
		//						m_SelectedEntity = child;
		//					}

		//					//--DRAG_DROP
		//					// Si comienza a arrastrar el nodo
		//					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		//					{
		//						ImGui::SetDragDropPayload("DRAG_ENTITY", &i, sizeof(int)); // Podrías enviar más datos si lo necesitas
		//						ImGui::Text("Drag %s", treeLabel.c_str());
		//						ImGui::EndDragDropSource();
		//					}

		//					// Si un nodo es arrastrado sobre otro nodo
		//					if (ImGui::BeginDragDropTarget())
		//					{
		//						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_ENTITY"))
		//						{
		//							int sourceEntityIndex = *(const int*)payload->Data;
		//							auto& sourceEntity = manager.getAllEntities()[sourceEntityIndex]; // Entidad que estás arrastrando
		//							auto& targetEntity = manager.getAllEntities()[i]; // Entidad sobre la cual se hace el "drop"

		//							sourceEntity->getComponent<ECS::Transform>().parent = targetEntity;
		//							targetEntity->getComponent<ECS::Transform>().children.push_back(sourceEntity);

		//							//targetEntity->getComponent<ECS::Transform>().addChild(sourceEntity);
		//						}
		//						ImGui::EndDragDropTarget();
		//					}
		//					//--END DRAG_DROP
		//				}
		//				ImGui::TreePop();
		//			}
		//		}




		//	}


		//}
		//ImGui::End();
		////---------------------------------------------------------------------------------------------------------------
	}
	bool ScriptableGameObjectManager::rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax)
	{
		return false;
	}
	bool ScriptableGameObjectManager::CheckIfGameObjectInRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection)
	{
		return false;
	}
	void ScriptableGameObjectManager::SaveSceneToFile(const std::string& filename)
	{
	}
	void ScriptableGameObjectManager::LoadSceneFromFile(const std::string& filename, std::vector<ECS_SCRIPTING::ScriptableEntity*>& entitiesInScene, ECS_SCRIPTING::ScriptableManager& manager)
	{
	}

	

	

}