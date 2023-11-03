#include "RendererManager.h"
#include "../../ECS/MeshFilter.h"
#include "../../ECS/MeshRenderer.h"
#include "../../ECS/Material.h"
#include "../../ECS/PointLight.h"
#include "../../ECS/SpotLight.h"
#include "../../ECS/DirectionalLight.h"


namespace GLCore::Render
{
	std::pair<glm::vec3, float> RendererManager::SceneBounds = { glm::vec3(0.0f), 0.0f };

	RendererManager::RendererManager()
	{
	}

	RendererManager::~RendererManager(){}


	void RendererManager::passLights()
	{
		for (int i = 0; i < entitiesInScene.size(); i++)
		{
			//-DIRECTIONAL LIGHTS
			if (entitiesInScene[i]->hascomponent<ECS::DirectionalLight>())
			{
				/*dynamicSkybox->Render(m_EditorCamera.GetCamera().GetViewMatrix(), m_EditorCamera.GetCamera().GetProjectionMatrix(),
					entitiesInScene[i]->getComponent<ECS::Transform>().position);*/

				entitiesInScene[i]->getComponent<ECS::DirectionalLight>().draw();
			}

			//-POINTS LIGHTS
			if (entitiesInScene[i]->hascomponent<ECS::PointLight>())
			{
				entitiesInScene[i]->getComponent<ECS::PointLight>().draw();
			}
			//-SPOT LIGHTS
			if (entitiesInScene[i]->hascomponent<ECS::SpotLight>())
			{
				entitiesInScene[i]->getComponent<ECS::SpotLight>().draw();
			}
		}
	}

	void RendererManager::passShadow()
	{
		//--SHADOWS PASS
		for (int i = 0; i < entitiesInScene.size(); i++)
		{
			//-DIRECTIONAL LIGHTS
			if (entitiesInScene[i]->hascomponent<ECS::DirectionalLight>())
			{
				if (entitiesInScene[i]->getComponent<ECS::DirectionalLight>().drawShadows)
				{
					entitiesInScene[i]->getComponent<ECS::DirectionalLight>().shadowMappingProjection(entitiesInScene);
				}
			}

			//-SPOT LIGHTS
			if (entitiesInScene[i]->hascomponent<ECS::SpotLight>())
			{
				if (entitiesInScene[i]->getComponent<ECS::SpotLight>().drawShadows)
				{
					entitiesInScene[i]->getComponent<ECS::SpotLight>().shadowMappingProjection(entitiesInScene);
				}
			}

			//-POINTS LIGHTS
			if (entitiesInScene[i]->hascomponent<ECS::PointLight>())
			{
				if (entitiesInScene[i]->getComponent<ECS::PointLight>().drawShadows)
				{
					entitiesInScene[i]->getComponent<ECS::PointLight>().shadowMappingProjection(entitiesInScene);
				}
			}
		}
	}

	void RendererManager::passGeometry()
	{
		for (int i = 0; i < entitiesInScene.size(); i++)
		{
			//-MATERIALS
			if (entitiesInScene[i]->hascomponent<ECS::Material>()) // Only root entities
			{
				entitiesInScene[i]->getComponent<ECS::Material>().draw();
			}
			//-MESH FILTERS
			if (entitiesInScene[i]->hascomponent<ECS::MeshFilter>())
			{
				entitiesInScene[i]->getComponent<ECS::MeshFilter>().draw();
			}
			//-MESH RENDERER
			if (entitiesInScene[i]->hascomponent<ECS::MeshRenderer>())
			{
				entitiesInScene[i]->getComponent<ECS::MeshRenderer>().draw();
			}
		}
	}







	void RendererManager::CalcSceneBundle() {

		glm::vec3 sceneMinBounds = glm::vec3(FLT_MAX);
		glm::vec3 sceneMaxBounds = glm::vec3(-FLT_MAX);

		for (ECS::Entity* entity : entitiesInScene)
		{
			if (entity->hascomponent<ECS::MeshRenderer>()) {
				ECS::MeshRenderer& renderer = entity->getComponent<ECS::MeshRenderer>();
				GLCore::MeshData& meshData = renderer.meshData;

				glm::mat4 transform = renderer.model_transform_matrix;

				for (int i = 0; i < 8; ++i) {
					glm::vec3 corner = glm::vec3(
						(i & 1) ? meshData.maxBounds.x : meshData.minBounds.x,
						(i & 2) ? meshData.maxBounds.y : meshData.minBounds.y,
						(i & 4) ? meshData.maxBounds.z : meshData.minBounds.z
					);
					glm::vec3 worldCorner = transform * glm::vec4(corner, 1.0f);
					sceneMinBounds = glm::min(sceneMinBounds, worldCorner);
					sceneMaxBounds = glm::max(sceneMaxBounds, worldCorner);
				}
			}
		}

		glm::vec3 sceneCenter = (sceneMinBounds + sceneMaxBounds) * 0.5f;
		float sceneRadius = glm::length(sceneMaxBounds - sceneMinBounds) * 0.5f;

		if (!std::isinf(sceneRadius)) {
			SceneBounds = std::make_pair(sceneCenter, sceneRadius);
		}
	}
}
