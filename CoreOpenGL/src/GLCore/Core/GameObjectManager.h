#pragma once

#include "../../ECS/Entity.h"
#include "../../ECS/Manager.h"

#include "../../ECS/Camera.h"

#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep


namespace GLCore 
{

	class GameObjectManager
	{
	public:
		ECS::Entity* CreateEntity();
		ECS::Manager manager;
		std::vector<ECS::Camera*> cameras;
		ECS::Entity* m_SelectedEntity = nullptr;

		bool useDirectionalLight = false;
		int totalPointLight = 0;
		int totalSpotLight = 0;

		void createGameObject(MainMenuAction action);
		void loadFileModel(ImportOptions importOptions);
		void drawHierarchy();

		//--SERIALIZACIONES
		void SaveSceneToFile(const std::string& filename);
		void LoadSceneFromFile(const std::string& filename, std::vector<ECS::Entity*>& entitiesInScene, ECS::Manager& manager);

	private:
		float pi = 3.1415926535f;
		
		int idGenerated = 0;
	};
}

