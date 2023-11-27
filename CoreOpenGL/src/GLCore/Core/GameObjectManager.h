#pragma once

#include "../../glpch.h"
#include <entt.hpp>

#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep




namespace GLCore 
{

	class GameObjectManager
	{
	public:

		bool useDirectionalLight = false;
		int totalPointLight = 0;
		int totalSpotLight = 0;


		void CreateEmptyGameObject();
		void InstantiatePrefab(MainMenuAction action);

		void drawHierarchy();

		//void createGameObject(MainMenuAction action);
		//void loadFileModel(ImportOptions importOptions);


		//--SERIALIZACIONES
		/*void SaveSceneToFile(const std::string& filename);
		void LoadSceneFromFile(const std::string& filename, std::vector<ECS::Entity*>& entitiesInScene, ECS::Manager& manager);*/

	private:

		entt::entity m_SelectedEntity;
		entt::registry m_registry;


		float pi = 3.1415926535f;
		
		int idGenerated = 0;
	};
}

