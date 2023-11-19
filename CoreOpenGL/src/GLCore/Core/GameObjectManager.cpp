#include "GameObjectManager.h"
#include "../Render/PrimitivesHelper.h"
#include "../Util/ModelLoader.h"


//Components
#include "ECS/Transform.h"
#include "ECS/MeshFilter.h"
#include "ECS/MeshRenderer.h"
#include "ECS/Material.h"
#include "ECS/DirectionalLight.h"
#include "ECS/PointLight.h"
#include "ECS/SpotLight.h"
#include "ECS/CharacterController.h"
#include "ECS/Car.h"



namespace GLCore
{
    ECS::Entity* GLCore::GameObjectManager::CreateGameObject()
    {
		ECS::Entity* e = nullptr;
		e = &manager.addEntity();
		idGenerated++;
        //By Default with transform
		e->addComponent<ECS::Transform>();
        e->getComponent<ECS::Transform>().position = { 0.0f, 0.0f, 0.0f };
        e->getComponent<ECS::Transform>().scale = { 1.0f, 1.0f, 1.0f };
        glm::vec3 eulers = { (0 * pi) / 180, (0 * pi) / 180, (0 * pi) / 180 };// Inicializa el cuaternión de rotación con ángulos de Euler
        e->getComponent<ECS::Transform>().rotation = glm::quat(eulers);
        return e;
    }


	void GLCore::GameObjectManager::createPresetGameObject(MainMenuAction action)
	{
		ECS::Entity* gameObject = nullptr;
		if (action == MainMenuAction::AddCube)
		{
			GLCore::MeshData cube = GLCore::Render::PrimitivesHelper::CreateCube();

			gameObject = CreateGameObject();
			gameObject->name = "Cube_" + std::to_string(idGenerated);
			gameObject->addComponent<ECS::MeshFilter>().initMesh(cube);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_CUBE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();	
		}
		else if (action == MainMenuAction::AddSegmentedCube)
		{
			GLCore::MeshData segCube = GLCore::Render::PrimitivesHelper::CreateSegmentedCube(1);

			gameObject = CreateGameObject();
			gameObject->name = "SegCube_" + std::to_string(idGenerated);
			gameObject->addComponent<ECS::MeshFilter>().initMesh(segCube);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_SEGMENTED_CUBE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddSphere)
		{
			GLCore::MeshData sphere = GLCore::Render::PrimitivesHelper::CreateSphere(1, 20, 20);

			gameObject = CreateGameObject();
			gameObject->name = "Sphere_" + std::to_string(idGenerated);
			gameObject->addComponent<ECS::MeshFilter>().initMesh(sphere);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_SPHERE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddQuad)
		{
			GLCore::MeshData quad = GLCore::Render::PrimitivesHelper::CreateQuad();

			gameObject = CreateGameObject();
			gameObject->name = "Quad_" + std::to_string(idGenerated);
			gameObject->addComponent<ECS::MeshFilter>().initMesh(quad);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_QUAD;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddPlane)
		{
			GLCore::MeshData plane = GLCore::Render::PrimitivesHelper::CreatePlane();

			gameObject = CreateGameObject();
			gameObject->name = "Plane_" + std::to_string(idGenerated);
			gameObject->addComponent<ECS::MeshFilter>().initMesh(plane);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_PLANE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddCamera)
		{
			gameObject = CreateGameObject();
			gameObject->addComponent<ECS::Camera>();
			gameObject->name = "Camera_" + std::to_string(idGenerated);

			cameras.push_back(&gameObject->getComponent<ECS::Camera>());
		}
		else if (action == MainMenuAction::AddDirectionalLight)
		{
			gameObject = CreateGameObject();
			gameObject->addComponent<ECS::DirectionalLight>();

			useDirectionalLight = true;
		}
		else if (action == MainMenuAction::AddPointLight)
		{
			gameObject = CreateGameObject();
			gameObject->addComponent<ECS::PointLight>();
			gameObject->getComponent<ECS::PointLight>().setId(totalPointLight);
			
			totalPointLight += 1;
		}
		else if (action == MainMenuAction::AddSpotLight)
		{
			gameObject = CreateGameObject();
			gameObject->addComponent<ECS::SpotLight>();
			gameObject->getComponent<ECS::SpotLight>().setId(totalSpotLight);

			totalSpotLight += 1;
		}
		else if (action == MainMenuAction::AddCharacterController)
		{
			/*gameObject = CreateGameObject();
			gameObject->addComponent<ECS::CharacterController>();
			
			GLCore::MeshData segCube = GLCore::Render::PrimitivesHelper::CreateSegmentedCube(1);

			gameObject->name = "CharacterController_" + std::to_string(entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(segCube);
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();*/
		}
		else if (action == MainMenuAction::SaveProject)
		{
			std::cout << "GUARDANDO..." << std::endl;

			SaveSceneToFile("scene.yaml");
		}

		addNativeComponentToSelectedGameObject(action);

		//Autoselect in creating
		if (gameObject != nullptr)
		{
			m_SelectedEntity = gameObject;
		}
	}

	void GLCore::GameObjectManager::addNativeComponentToSelectedGameObject(MainMenuAction action)
	{
		if (action == MainMenuAction::AddCarController)
		{
			if (m_SelectedEntity != nullptr)
			{
				m_SelectedEntity->addComponent<ECS::Car>();
			}
		}
	}


	void GameObjectManager::loadFileModel(ImportOptions importOptions)
	{
		ModelParent modelParent = {};
		try {
			modelParent = GLCore::Utils::ModelLoader::LoadModel(importOptions);

			ECS::Entity* entityParent = CreateGameObject();
			entityParent->name = modelParent.name;

			if (modelParent.modelInfos.size() > 1)
			{
				for (int i = 0; i < modelParent.modelInfos.size(); i++)
				{
					ECS::Entity* entityChild = CreateGameObject();
					entityChild->name = modelParent.modelInfos[i].meshData.meshName + std::to_string(i);
					entityChild->addComponent<ECS::MeshFilter>().initMesh(modelParent.modelInfos[i].meshData);
					entityChild->getComponent<ECS::MeshFilter>().modelType = EXTERNAL_FILE;
					entityChild->getComponent<ECS::MeshFilter>().modelPath = importOptions.filePath + importOptions.fileName;
					entityChild->addComponent<ECS::MeshRenderer>();
					entityChild->addComponent<ECS::Material>().setMaterial(modelParent.modelInfos[i].model_material);

					entityChild->getComponent<ECS::Transform>().parent = entityParent;
					entityParent->getComponent<ECS::Transform>().children.push_back(entityChild);
				}
			}
			else
			{
				entityParent->addComponent<ECS::MeshFilter>().initMesh(modelParent.modelInfos[0].meshData);
				entityParent->getComponent<ECS::MeshFilter>().modelType = EXTERNAL_FILE;
				entityParent->getComponent<ECS::MeshFilter>().modelPath = importOptions.filePath + importOptions.fileName;
				entityParent->addComponent<ECS::MeshRenderer>();
				entityParent->addComponent<ECS::Material>().setMaterial(modelParent.modelInfos[0].model_material);
				entityParent->name = modelParent.modelInfos[0].meshData.meshName + std::to_string(0);
			}

			if (entityParent != nullptr)
			{
				m_SelectedEntity = entityParent;
			}
		}
		catch (std::runtime_error& e) {
			std::cerr << "Error cargando el modelo: " << e.what() << std::endl;
		}
	}
	void GameObjectManager::drawHierarchy()
	{
		//-------------------------------------------HIERARCHY PANEL--------------------------------------
		ImGui::Begin("Hierarchy", nullptr);

		for (int i = 0; i < manager.getAllEntities().size(); i++)
		{
			if (manager.getAllEntities()[i]->markedToDelete == false)
			{
				if (manager.getAllEntities()[i]->getComponent<ECS::Transform>().parent == nullptr) // Only root entities
				{
					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
					if (m_SelectedEntity == manager.getAllEntities()[i])
						node_flags |= ImGuiTreeNodeFlags_Selected;

					std::string treeLabel = manager.getAllEntities()[i]->name;

					bool treeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, treeLabel.c_str());

					if (ImGui::IsItemClicked()) // Select on (left) click
					{
						if (m_SelectedEntity != nullptr)
						{
							if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
							{
								m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = false;
							}
						}
						m_SelectedEntity = manager.getAllEntities()[i];
					}

					if (m_SelectedEntity != nullptr)
					{
						if (ImGui::BeginPopupContextItem()) { // Click derecho en el elemento
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedEntity->active = false;
								m_SelectedEntity->markedToDelete = true;
								m_SelectedEntity = nullptr;
							}
							ImGui::EndPopup();
						}
					}

					//--DRAG_DROP
					// Si comienza a arrastrar el nodo
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						ImGui::SetDragDropPayload("DRAG_ENTITY", &i, sizeof(int)); // Envía el índice de la entidad
						ImGui::Text("Drag %s", treeLabel.c_str());
						ImGui::EndDragDropSource();
					}

					// Si un nodo es arrastrado sobre otro nodo
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_ENTITY"))
						{
							int sourceEntityIndex = *(const int*)payload->Data;
							// Asegúrate de que los índices estén dentro de los límites
							if (sourceEntityIndex >= 0 && sourceEntityIndex < manager.getAllEntities().size() && sourceEntityIndex != i)
							{
								ECS::Entity* sourceEntity = manager.getAllEntities()[sourceEntityIndex]; // Entidad que estás arrastrando
								ECS::Entity* targetEntity = manager.getAllEntities()[i]; // Entidad sobre la cual se hace el "drop"
								// Asegúrate de que la entidad objetivo no sea hija de la entidad fuente
								if (!sourceEntity->getComponent<ECS::Transform>().isEntityChildOf(sourceEntity,targetEntity))
								{
									sourceEntity->getComponent<ECS::Transform>().setParent(targetEntity);
								}
							}
						}
						ImGui::EndDragDropTarget();
					}
					//--END DRAG_DROP




					if (treeOpen)
					{
						for (int j = 0; j < manager.getAllEntities()[i]->getComponent<ECS::Transform>().children.size(); j++)
						{
							auto& child = manager.getAllEntities()[i]->getComponent<ECS::Transform>().children[j];

							ImGuiTreeNodeFlags child_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
							if (m_SelectedEntity == child)
								child_flags |= ImGuiTreeNodeFlags_Selected;

							std::string childLabel = child->name;

							ImGui::TreeNodeEx((void*)(intptr_t)(i * 1000 + j), child_flags, childLabel.c_str()); // Ensure the ID is unique
							if (ImGui::IsItemClicked())
							{
								if (m_SelectedEntity)
								{
									if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
									{
										m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = false;
									}
								}
								m_SelectedEntity = child;
							}

							//--DRAG_DROP
							// Si comienza a arrastrar el nodo
							if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
							{
								ImGui::SetDragDropPayload("DRAG_ENTITY", &i, sizeof(int)); // Podrías enviar más datos si lo necesitas
								ImGui::Text("Drag %s", treeLabel.c_str());
								ImGui::EndDragDropSource();
							}

							// Si un nodo es arrastrado sobre otro nodo
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_ENTITY"))
								{
									int sourceEntityIndex = *(const int*)payload->Data;
									auto& sourceEntity = manager.getAllEntities()[sourceEntityIndex]; // Entidad que estás arrastrando
									auto& targetEntity = manager.getAllEntities()[i]; // Entidad sobre la cual se hace el "drop"

									sourceEntity->getComponent<ECS::Transform>().parent = targetEntity;
									targetEntity->getComponent<ECS::Transform>().children.push_back(sourceEntity);

									//targetEntity->getComponent<ECS::Transform>().addChild(sourceEntity);
								}
								ImGui::EndDragDropTarget();
							}
							//--END DRAG_DROP
						}
						ImGui::TreePop();
					}
				}

			}
			
		}
		ImGui::End();
		//---------------------------------------------------------------------------------------------------------------
	}

	void GameObjectManager::SaveSceneToFile(const std::string& filename)
	{
		YAML::Emitter out;

		out << YAML::BeginSeq; // Comienza una secuencia para todas las entidades
		for (const auto& entity : manager.getAllEntities()) {
			if (entity->isActive() && entity->markedToDelete == false) { // Asegúrate de serializar solo entidades activas
				out << entity->serialize(); // Serializa cada entidad y agrega al flujo de salida
			}
		}
		out << YAML::EndSeq; // Finaliza la secuencia

		// Escribe la cadena resultante del Emitter al archivo
		std::ofstream fout(filename);
		fout << out.c_str();
	}
	void GameObjectManager::LoadSceneFromFile(const std::string& filename, std::vector<ECS::Entity*>& entitiesInScene, ECS::Manager& manager) {
		//std::ifstream fin(filename);
		//if (!fin.is_open()) {
		//	// Manejar error al abrir el archivo
		//	std::cerr << "Error al abrir el archivo para cargar la escena" << std::endl;
		//	return;
		//}

		//YAML::Node data = YAML::Load(fin); // Carga el archivo en un nodo de YAML

		//if (!data.IsSequence()) {
		//	// Manejar error de formato incorrecto
		//	std::cerr << "Formato de archivo de escena incorrecto" << std::endl;
		//	return;
		//}

		//// Limpia las entidades actuales en la escena antes de cargar
		//entitiesInScene.clear();

		//for (const auto& node : data) {
		//	// Aquí se asume que tienes una forma de crear entidades y que manager es accesible
		//	ECS::Entity& newEntity = manager.addEntity();
		//	newEntity.deserialize(node); // Deserializa la información de cada entidad
		//	entitiesInScene.push_back(&newEntity); // Añade la entidad al vector de entidades de la escena
		//}
	}

	//RAY
	bool GameObjectManager::CheckIfGameObjectInRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection)
	{
		bool selectingEntity = false;

		//La lista de entidades actual que vamos a comprobar si atraviensan el rayo
		std::vector<ECS::Entity*> entities = manager.getAllEntities();


		entitiesInRay.clear();
		//Recorremos la lista de entidades 
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities[i]->hascomponent<ECS::MeshRenderer>())
			{
				// Obtener la matriz de transformación actual
				const glm::mat4& transform = entities[i]->getComponent<ECS::MeshRenderer>().model_transform_matrix;

				// Transformar los vértices min y max de la Bounding Box
				glm::vec3 transformedMin = glm::vec3(transform * glm::vec4(entities[i]->getComponent<ECS::MeshRenderer>().meshData.minBounds, 1.0f));
				glm::vec3 transformedMax = glm::vec3(transform * glm::vec4(entities[i]->getComponent<ECS::MeshRenderer>().meshData.maxBounds, 1.0f));

				// Verificar la intersección del rayo
				if (rayIntersectsBoundingBox(rayOrigin, rayDirection, transformedMin, transformedMax))
				{
					entitiesInRay.push_back(entities[i]);
				}
			}
		}


		if (entitiesInRay.size() > 1)
		{
			selectingEntity = true;
		}
		else if (entitiesInRay.size() > 0)
		{
			m_SelectedEntity = entitiesInRay[0];
			entitiesInRay.clear();
			if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
				m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = true;
		}


		return selectingEntity;
	}
	bool GameObjectManager::rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax)
	{
		float tMin = (boxMin.x - rayOrigin.x) / rayDirection.x;
		float tMax = (boxMax.x - rayOrigin.x) / rayDirection.x;

		if (tMin > tMax) std::swap(tMin, tMax);

		float tyMin = (boxMin.y - rayOrigin.y) / rayDirection.y;
		float tyMax = (boxMax.y - rayOrigin.y) / rayDirection.y;

		if (tyMin > tyMax) std::swap(tyMin, tyMax);

		if ((tMin > tyMax) || (tyMin > tMax))
			return false;

		if (tyMin > tMin)
			tMin = tyMin;

		if (tyMax < tMax)
			tMax = tyMax;

		float tzMin = (boxMin.z - rayOrigin.z) / rayDirection.z;
		float tzMax = (boxMax.z - rayOrigin.z) / rayDirection.z;

		if (tzMin > tzMax) std::swap(tzMin, tzMax);

		if ((tMin > tzMax) || (tzMin > tMax))
			return false;

		if (tzMin > tMin)
			tMin = tzMin;

		if (tzMax < tMax)
			tMax = tzMax;

		return true;
	}



}
