#pragma once

#include "../../glpch.h"
#include <entt.hpp>

#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep




namespace GLCore 
{

	struct TransformComponent
	{
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		glm::mat4 m_modelMatrix;
	};


	struct MeshFilterComponent
	{
		GLCore::MeshData meshData;
		GLCore::MODEL_TYPES modelType = GLCore::MODEL_TYPES::NONE;
		std::string modelPath = "none";
	};

	struct MeshRendererComponent
	{
		GLCore::MeshData meshData;
		std::string currentShaderName = "pbr_ibl";
		bool visibleModel = true;
		bool drawLocalBB = false;
		bool dropShadow = true;
	};

	struct MaterialComponent
	{
		std::unique_ptr<GLCore::Material> material = nullptr;
		std::string currentShaderName = "pbr_ibl";
		glm::ivec2 repetitionFactor = glm::ivec2(1, 1);

		//Defaults
		glm::vec3 defaultColor = glm::vec3(1.0f, 1.0f, 1.0f);
		
		std::string defaultAlbedoMap    = "assets/default/default_white.jpg";
		std::string defaultNormalMap	= "assets/default/default_normal.jpg";
		std::string defaultMetallicMap  = "assets/default/default_black.jpg";
		std::string defaultRoughnessMap = "assets/default/default_black.jpg";
		std::string defaultAOMap		= "assets/default/default_white.jpg";
	};




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

