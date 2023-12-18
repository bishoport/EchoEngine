#pragma once
#include "../../glpch.h"

#include <entt.hpp>
#include "UUID.h"
#include "../Util/IMGLoader.h"
#include "../DataStruct.h"
#include <tuple> // Asegúrate de incluir esta cabecera para std::tuple

namespace GLCore
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
		glm::quat rotation = glm::vec3(0.0, 0.0, 0.0);
		glm::vec3 scale = glm::vec3(1.0,1.0,1.0);
		glm::mat4 m_modelMatrix;

		glm::mat4 getLocalModelMatrix() const {
			return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct ParentComponent {
		entt::entity parentEntity;
	};

	struct ChildrenComponent {
		std::vector<entt::entity> childEntities;
	};

	struct MeshFilterComponent
	{
		Ref<GLCore::MeshData> meshData;
		GLCore::MODEL_TYPES modelType = GLCore::MODEL_TYPES::NONE;
		std::string modelPath = "none";
	};

	struct MeshRendererComponent
	{
		Ref<GLCore::MeshData> meshData;
		std::string currentShaderName = "pbr_ibl";
		bool visibleModel = true;
		bool drawLocalBB = false;
		bool dropShadow = true;
		glm::mat4 model_transform_matrix{ glm::mat4(1.0f) };
	};

	struct SkinedMeshComponent
	{
		Ref<GLCore::MeshData> meshData;
		std::string currentShaderName = "pbr_ibl";
		bool visibleModel = true;
		bool drawLocalBB = false;
		bool dropShadow = true;
		glm::mat4 model_transform_matrix{ glm::mat4(1.0f) };
	};

	struct MaterialComponent
	{
		//VALUES
		float shininess;
		float hdrMultiply = 0.0f;
		float hdrIntensity = 0.3f;
		float exposure = 1.0f;
		float gamma = 2.2f;
		float max_reflection_lod = 4.0;
		float iblIntensity = 0.0;
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		float normalIntensity = 0.5f;
		float metallicValue = 0.0f;
		float roughnessValue = 0.05f;
		float reflectanceValue = 0.04f;
		float fresnelCoefValue = 5.0f;
		//-------------------------------------------------------

		std::string currentShaderName = "pbr_ibl";
		glm::ivec2 repetitionFactor = glm::ivec2(1, 1);

		Ref <MaterialData> materialData = nullptr;

		void setDefaultMaterial() 
		{
			std::string defaultPathTexture_white  = "assets/default/default_white.jpg";
			std::string defaultPathTexture_normal = "assets/default/default_normal.jpg";
			std::string defaultPathTexture_black  = "assets/default/default_black.jpg";

			materialData = std::make_shared<GLCore::MaterialData>();

			//--ALBEDO
			Ref<Texture> textureAlbedo = std::make_shared<GLCore::Texture>();
			textureAlbedo->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_white);
			textureAlbedo->image->path = defaultPathTexture_white;
			textureAlbedo->hasMap = true;
			materialData->albedoMap = textureAlbedo;
			//---------------------------------------------------------------------------------------------------

			//--NORMAL
			Ref<Texture> textureNormal = std::make_shared<GLCore::Texture>();
			textureNormal->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_normal);
			textureNormal->image->path = defaultPathTexture_normal;
			textureNormal->hasMap = true;
			materialData->normalMap = textureNormal;
			//---------------------------------------------------------------------------------------------------

			//--METALLIC
			Ref<Texture> textureMetallic = std::make_shared<GLCore::Texture>();
			textureMetallic->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_black);
			textureMetallic->image->path = defaultPathTexture_black;
			textureMetallic->hasMap = true;
			materialData->metallicMap = textureMetallic;
			//---------------------------------------------------------------------------------------------------

			//--ROUGHNESS
			Ref<Texture> textureRoughness = std::make_shared<GLCore::Texture>();
			textureRoughness->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_black);
			textureRoughness->image->path = defaultPathTexture_black;
			textureRoughness->hasMap = true;
			materialData->rougnessMap = textureRoughness;
			//---------------------------------------------------------------------------------------------------

			//--AO
			Ref<Texture> textureAO = std::make_shared<GLCore::Texture>();
			textureAO->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture_white);
			textureAO->image->path = defaultPathTexture_white;
			textureAO->hasMap = true;
			materialData->aOMap = textureAO;
			//---------------------------------------------------------------------------------------------------

			materialData->albedoMap->typeString = "ALBEDO";
			materialData->normalMap->typeString = "NORMAL";
			materialData->metallicMap->typeString = "METALLIC";
			materialData->rougnessMap->typeString = "ROUGHNESS";
			materialData->aOMap->typeString = "AO";

			materialData->albedoMap->type = TEXTURE_TYPES::ALBEDO;
			materialData->normalMap->type = TEXTURE_TYPES::NORMAL;
			materialData->metallicMap->type = TEXTURE_TYPES::METALLIC;
			materialData->rougnessMap->type = TEXTURE_TYPES::ROUGHNESS;
			materialData->aOMap->type = TEXTURE_TYPES::AO;

			materialData->albedoMap->Bind();
			materialData->normalMap->Bind();
			materialData->metallicMap->Bind();
			materialData->rougnessMap->Bind();
			materialData->aOMap->Bind();
		}
	};

	struct AnimatorComponent
	{
		Ref<GLCore::MeshData> meshData;
		Ref<GLCore::Animation> m_CurrentAnimation = nullptr;

		std::vector<glm::mat4> m_FinalBoneMatrices;

		float m_CurrentTime = 0.0f;
		float m_DeltaTime = 0.0f;

		void SetAnimation(std::string animationPath)
		{
			auto anim = std::make_shared<Animation>(animationPath, meshData);

			m_CurrentTime = 0.0;
			m_CurrentAnimation = anim;

			m_FinalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
				m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		void UpdateAnimation(float dt)
		{
			m_DeltaTime = dt;
			if (m_CurrentAnimation)
			{
				m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
				m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
				CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
			}
		}

		void PlayAnimation()
		{
			m_CurrentTime = 0.0f;
		}

		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
		{
			std::string nodeName = node->name;
			glm::mat4 nodeTransform = node->transformation;

			GLCore::Util::SkeletalAnimation::Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

			if (Bone)
			{
				Bone->Update(m_CurrentTime);
				nodeTransform = Bone->GetLocalTransform();
			}

			glm::mat4 globalTransformation = parentTransform * nodeTransform;

			auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
			if (boneInfoMap.find(nodeName) != boneInfoMap.end())
			{
				int index = boneInfoMap[nodeName].id;
				glm::mat4 offset = boneInfoMap[nodeName].offset;
				m_FinalBoneMatrices[index] = globalTransformation * offset;
			}

			for (int i = 0; i < node->childrenCount; i++)
				CalculateBoneTransform(&node->children[i], globalTransformation);
		}

		std::vector<glm::mat4> GetFinalBoneMatrices()
		{
			return m_FinalBoneMatrices;
		}
		
	};

	struct DirectionalLightComponent
	{
		unsigned int lightID = 0;
		bool active = true;
		bool debug = true;

		

		glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

		//DirectionalLight values
		glm::vec3 direction = glm::vec3(0.0f, -10.0f, 0.0f);
		float currentSceneRadius = 0.0f;
		float sceneRadiusOffset = 0.0f;

		//Shadow values
		int shadowMapResolution = 1024;
		int chanelShadowTexture = 0;
		GLuint shadowFBO = 0;
		GLuint shadowTex = 0;
		GLuint shadowDepth = 0;


		glm::mat4 shadowMVP = glm::mat4(1.0f);
		bool drawShadows = false;
		float near_plane = 0.1f, far_plane = 100.0f;
		float shadowIntensity = 0.5f;
		bool usePoisonDisk = false;
		float orthoLeft = -10.0f;
		float orthoRight = 10.0f;
		float orthoBottom = -10.0f;
		float orthoTop = 10.0f;

		float orthoNear = 0.1f;
		float orthoNearOffset = 0.0f;
		float orthoFar = 100.0f;
		float orthoFarOffset = 0.0f;

		float angleX = 0.0f;
		float angleY = 0.0f;

		glm::mat4 shadowBias = glm::mat4(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		void prepareShadows()
		{
			GLCore::Render::FBOManager::CreateShadowMapFBO(&shadowFBO, &shadowDepth, &shadowTex, shadowMapResolution);
		}
	};

	inline std::tuple<IDComponent, TagComponent, TransformComponent, ParentComponent, ChildrenComponent,
		MeshFilterComponent, SkinedMeshComponent, MeshRendererComponent, MaterialComponent, DirectionalLightComponent, AnimatorComponent> GetAllComponentTypes() {
		return std::make_tuple(IDComponent{}, TagComponent{}, TransformComponent{}, ParentComponent{}, ChildrenComponent{},
			MeshFilterComponent{}, SkinedMeshComponent{}, MeshRendererComponent{}, MaterialComponent{}, DirectionalLightComponent{}, AnimatorComponent{});
	}
}
