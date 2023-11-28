#include "Scene.h"
#include <iostream>
#include "Application.h"
#include "../Render/ShaderManager.h"

#include <imGizmo/ImGuizmo.h>

#include "../Util/ModelLoader.h"

#include "Entity.h"
#include "../Render/PrimitivesHelper.h"

#include "SceneHierarchyPanel.h"
#include "../Util/IMGLoader.h"
#include "../Render/MaterialHelper.h"

#include <windows.h>


namespace GLCore {

	//--SCENE BOUNDS
	std::pair<glm::vec3, float> Scene::SceneBounds = { glm::vec3(0.0f), 0.0f };

	//--ENTT
	SceneHierarchyPanel* sceneHierarchyPanel;

	//--CONSTRUCTOR
    Scene::Scene() : m_EditorCamera(16.0f / 9.0f) {}
    Scene::~Scene(){shutdown();}


	//--GAME OBJECTS
	void Scene::CreateEmptyGameObject()
	{
		Entity entity = CreateEntity("Empty Entity");
	}
	void Scene::InstantiatePrefab(MainMenuAction action)
	{
		if (action == MainMenuAction::AddCube)
		{
			auto meshData = GLCore::Render::PrimitivesHelper::CreateCube();
				
			Entity entity = CreateEntity("Cube");
			RegistrySingleton::getRegistry().emplace<MeshFilterComponent>(entity, meshData,PRIMIVITE_CUBE);
			RegistrySingleton::getRegistry().emplace<MeshRendererComponent>(entity);

			MeshRendererComponent* meshRendererComponent = &RegistrySingleton::getRegistry().get<MeshRendererComponent>(entity);
			TransformComponent* transformComponent = &RegistrySingleton::getRegistry().get<TransformComponent>(entity);

			meshRendererComponent->meshData = meshData;
			meshRendererComponent->meshData->meshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshRotation = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshScale = glm::vec3(1.0f, 1.0f, 1.0f);

			transformComponent->position = meshRendererComponent->meshData->meshLocalPosition;

			RegistrySingleton::getRegistry().emplace<MaterialComponent>(entity).setDefaultMaterial();
		}
		else if (action == MainMenuAction::AddSegmentedCube)
		{
			auto meshData = GLCore::Render::PrimitivesHelper::CreateSegmentedCube();

			Entity entity = CreateEntity("SegmentedCube");
			RegistrySingleton::getRegistry().emplace<MeshFilterComponent>(entity, meshData, PRIMIVITE_SEGMENTED_CUBE);
			RegistrySingleton::getRegistry().emplace<MeshRendererComponent>(entity);

			MeshRendererComponent* meshRendererComponent = &RegistrySingleton::getRegistry().get<MeshRendererComponent>(entity);
			TransformComponent* transformComponent = &RegistrySingleton::getRegistry().get<TransformComponent>(entity);

			meshRendererComponent->meshData = meshData;

			meshRendererComponent->meshData->meshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshRotation = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshScale = glm::vec3(1.0f, 1.0f, 1.0f);
			transformComponent->position = meshRendererComponent->meshData->meshLocalPosition;

			RegistrySingleton::getRegistry().emplace<MaterialComponent>(entity).setDefaultMaterial();
		}
		else if (action == MainMenuAction::AddPlane)
		{
			auto meshData = GLCore::Render::PrimitivesHelper::CreatePlane();

			Entity entity = CreateEntity("Plane");
			RegistrySingleton::getRegistry().emplace<MeshFilterComponent>(entity, meshData, PRIMIVITE_PLANE);
			RegistrySingleton::getRegistry().emplace<MeshRendererComponent>(entity);

			MeshRendererComponent* meshRendererComponent = &RegistrySingleton::getRegistry().get<MeshRendererComponent>(entity);
			TransformComponent* transformComponent = &RegistrySingleton::getRegistry().get<TransformComponent>(entity);

			meshRendererComponent->meshData = meshData;

			meshRendererComponent->meshData->meshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshRotation = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshScale = glm::vec3(1.0f, 1.0f, 1.0f);

			transformComponent->position = meshRendererComponent->meshData->meshLocalPosition;

			RegistrySingleton::getRegistry().emplace<MaterialComponent>(entity).setDefaultMaterial();
		}
		else if (action == MainMenuAction::AddSphere)
		{
			auto meshData = GLCore::Render::PrimitivesHelper::CreateSphere(1, 20, 20);

			Entity entity = CreateEntity("Sphere");
			RegistrySingleton::getRegistry().emplace<MeshFilterComponent>(entity, meshData, PRIMIVITE_SPHERE);
			RegistrySingleton::getRegistry().emplace<MeshRendererComponent>(entity);

			MeshRendererComponent* meshRendererComponent = &RegistrySingleton::getRegistry().get<MeshRendererComponent>(entity);
			TransformComponent* transformComponent = &RegistrySingleton::getRegistry().get<TransformComponent>(entity);

			meshRendererComponent->meshData = meshData;

			meshRendererComponent->meshData->meshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshRotation = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshScale = glm::vec3(1.0f, 1.0f, 1.0f);

			transformComponent->position = meshRendererComponent->meshData->meshLocalPosition;

			RegistrySingleton::getRegistry().emplace<MaterialComponent>(entity).setDefaultMaterial();
		}
		else if (action == MainMenuAction::AddQuad)
		{
			auto meshData = GLCore::Render::PrimitivesHelper::CreateQuad();

			Entity entity = CreateEntity("Quad");
			RegistrySingleton::getRegistry().emplace<MeshFilterComponent>(entity, meshData, PRIMIVITE_QUAD);
			RegistrySingleton::getRegistry().emplace<MeshRendererComponent>(entity);

			MeshRendererComponent* meshRendererComponent = &RegistrySingleton::getRegistry().get<MeshRendererComponent>(entity);
			TransformComponent* transformComponent = &RegistrySingleton::getRegistry().get<TransformComponent>(entity);

			meshRendererComponent->meshData = meshData;

			meshRendererComponent->meshData->meshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshRotation = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshScale = glm::vec3(1.0f, 1.0f, 1.0f);

			transformComponent->position = meshRendererComponent->meshData->meshLocalPosition;

			RegistrySingleton::getRegistry().emplace<MaterialComponent>(entity).setDefaultMaterial();
		}
		else if (action == MainMenuAction::AddDirectionalLight)
		{
			Entity entity = CreateEntity("Directiona Light");
			DirectionalLightComponent* directionalLightComponent = &RegistrySingleton::getRegistry().emplace<DirectionalLightComponent>(entity);
			directionalLightComponent->prepareShadows();
			useDirectionalLight = true;
		}
	}
	//-------------------------------------------------------------------------------------------



	//--ENTITIES
	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}
	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { RegistrySingleton::getRegistry().create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;

		return entity;
	}
	Entity Scene::DuplicateEntity(Entity entity)
	{
		return Entity();
	}
	void Scene::DestroyEntity(Entity entity)
	{
		if (entity.HasComponent<MeshRendererComponent>())
		{
			MeshRendererComponent mr = entity.GetComponent<MeshRendererComponent>();
			glDeleteVertexArrays(1, &mr.meshData->VAO);
			glDeleteBuffers(1, &mr.meshData->VBO);
			glDeleteBuffers(1, &mr.meshData->EBO);
		}

		if (entity.HasComponent<MaterialComponent>())
		{
			MaterialComponent mc = entity.GetComponent<MaterialComponent>();
			std::vector<Ref<Texture>> textures;

			textures.push_back(mc.materialData->albedoMap);
			textures.push_back(mc.materialData->normalMap);
			textures.push_back(mc.materialData->metallicMap);
			textures.push_back(mc.materialData->rougnessMap);
			textures.push_back(mc.materialData->aOMap);

			for (size_t i = 0; i < textures.size(); i++)
			{
				glDeleteTextures(1, &textures[i]->textureID);
				textures[i]->hasMap = false;
			}
		}

		entity.RemoveAllComponents();

		//size_t numElements = m_EntityMap->size();
		//std::cout << "m_EntityMap-> " << numElements << std::endl;

		// Elimina la entidad de la lista de entidades mapeadas por UUID ???
		/*m_EntityMap.erase(entity.GetUUID());
		*/
	}
	Entity Scene::FindEntityByName(std::string_view name)
	{
		return Entity();
	}
	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		return Entity();
	}
	Entity Scene::GetPrimaryCameraEntity()
	{
		return Entity();
	}
	//-----------------------------------------------------------------------------




	//--INIT
    bool Scene::initialize()
    {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        //--CAMARA
        m_EditorCamera.GetCamera().SetPosition(glm::vec3(-1.492, 6.195f, 25.196f));
        m_EditorCamera.GetCamera().SetRotation(glm::vec2(3.2f, -451.199f));
        //----------------------------------------------------------------------------------------------------------------------------


		//--LOAD SHADERS
		GLCore::Render::ShaderManager::LoadAllShaders();
	
		//--IBL
		iblManager.prepare_PBR_IBL(800, 600);
		//--------------------------------------------------------------------------------------------------------------------------------

		////--SKYBOX
		//std::vector<const char*> faces
		//{
		//	"assets/default/Skybox/right.jpg",
		//	"assets/default/Skybox/left.jpg",
		//	"assets/default/Skybox/top.jpg",
		//	"assets/default/Skybox/bottom.jpg",
		//	"assets/default/Skybox/front.jpg",
		//	"assets/default/Skybox/back.jpg"
		//};
		//skybox = new Utils::Skybox(faces);
		//dynamicSkybox = new Utils::DynamicSkybox(faces);
		//----------------------------------------------------------------------------------------------------------------------------


		//--GRID WORLD REFENRENCE
		gridWorldRef = new Utils::GridWorldReference();
		//----------------------------------------------------------------------------------------------------------------------------


		//--POST-PROCESS
		postproManager = new Utils::PostProcessingManager();
		postproManager->Init(800,600);
		//--------------------------------------------------------------------------------------------------------------------------------


		//--FBO SCENE
		scene_colorBuffers = GLCore::Render::FBOManager::CreateFBO_Color_RGBA16F(&scene_FBO, &scene_depthBuffer, 1, 800, 600);
		// ---------------------------------------


		//--RESIZE SCENE PANEL EVENT
		EventManager::getOnPanelResizedEvent().subscribe([this](const std::string name, const ImVec2& size, const ImVec2& position)
		{
			if (name == "SCENE")
			{
				GLCore::Render::FBOManager::UpdateFBO_Color_RGBA16F(&scene_FBO, &scene_depthBuffer, scene_colorBuffers, size.x, size.y);
			}
		});
		//---------------------------------------------------


		assetsPanel.SetDelegate([this](ImportOptions importOptions) {
			this->getModelPathFromAssets(importOptions);
		});


		sceneHierarchyPanel = new SceneHierarchyPanel(this);

		//Application::gameObjectManager->InstantiatePrefab(MainMenuAction::AddCube);

        return true;
    }
	//--------------------------------------------------------------------------------



	//--MAIN LOOP
    void Scene::update(Timestep deltaTime)
    {
		//--INPUTS TOOLS
		if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_1)) 
		{
			m_GizmoOperation = GizmoOperation::Translate;
		}
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_2))
		{
			m_GizmoOperation = GizmoOperation::Rotate3D;
		}
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_3))
		{
			m_GizmoOperation = GizmoOperation::Scale;
		}
		//------------------------------------------------------------------------------

		if (m_EntityMap.size() > 0)
		{
			CalcSceneBundle();
		}


		//--ENTITIES UPDATE
		auto viewTransform_UPDATE = RegistrySingleton::getRegistry().view<TransformComponent,MeshRendererComponent>();
		for (auto entity : viewTransform_UPDATE)
		{
			auto [transform,meshRenderer] = viewTransform_UPDATE.get<TransformComponent, MeshRendererComponent>(entity);

			// Ahora, directamente podemos obtener la matriz de transformación del objeto
			meshRenderer.model_transform_matrix = transform.getLocalModelMatrix();

			// Si hay un padre, combinamos nuestras transformaciones con las de él.
			Entity entityCheck{ entity, this };
			if (entityCheck.HasComponent<ParentComponent>()) {
				Entity entityParent{ entityCheck.GetComponent<ParentComponent>().parentEntity, this };
				meshRenderer.model_transform_matrix = entityParent.GetComponent<TransformComponent>().getLocalModelMatrix() * meshRenderer.model_transform_matrix;
			}
		}


		auto viewDirectionalLight_UPDATE = RegistrySingleton::getRegistry().view<TransformComponent, DirectionalLightComponent>();
		for (auto entity : viewDirectionalLight_UPDATE)
		{
			auto [transform, directionalLightComponent] = viewDirectionalLight_UPDATE.get<TransformComponent, DirectionalLightComponent>(entity);

			auto [sceneCenter, sceneRadius] = SceneBounds;

			directionalLightComponent.currentSceneRadius = sceneRadius;

			if (sceneRadius > 0.0f)
			{
				sceneRadius += directionalLightComponent.sceneRadiusOffset;

				directionalLightComponent.orthoLeft = -sceneRadius;
				directionalLightComponent.orthoRight = sceneRadius;
				directionalLightComponent.orthoBottom = -sceneRadius;
				directionalLightComponent.orthoTop = sceneRadius;
				directionalLightComponent.orthoNear = -sceneRadius - directionalLightComponent.orthoNearOffset;
				directionalLightComponent.orthoFar = (2 * sceneRadius) + directionalLightComponent.orthoFarOffset;

				// Calcula la posición de la luz basada en los ángulos y la distancia al centro de la escena
				transform.position.x = sceneCenter.x + sceneRadius * sin(directionalLightComponent.angleX) * cos(directionalLightComponent.angleY);
				transform.position.y = sceneCenter.y + sceneRadius * cos(directionalLightComponent.angleX);
				transform.position.z = sceneCenter.z + sceneRadius * sin(directionalLightComponent.angleX) * sin(directionalLightComponent.angleY);

				// Actualiza la dirección de la luz
				directionalLightComponent.direction = glm::normalize(sceneCenter - transform.position);
			}
		}
		//----------------------------------------------------------------------------------------------------------------------------


        //--EDITOR CAMERA
        aspectRatio = static_cast<float>(sceneSize.x) / static_cast<float>(sceneSize.y);

        m_EditorCamera.GetCamera().SetProjection(m_EditorCamera.GetCamera().GetFov(), aspectRatio, 0.1f, 100.0f);
        m_EditorCamera.OnUpdate(deltaTime);
        //----------------------------------------------------------------------------------------------------------------------------

		//--GAME CAMERAS
		/*for (int i = 0; i < Application::gameObjectManager->cameras.size(); i++)
		{
			Application::gameObjectManager->cameras[i]->SetProjection(Application::gameObjectManager->cameras[i]->GetFov(), gameSize.x / gameSize.y, 0.1f, 100.0f);
		}*/
    }
    void Scene::render()
    {
		//--EDITOR CAMERA
		glm::mat4 cameraEditorProjectionMatrix = m_EditorCamera.GetCamera().GetProjectionMatrix();
		glm::mat4 cameraEditorViewMatrix       = m_EditorCamera.GetCamera().GetViewMatrix();
		glm::vec3 cameraEditorPosition         = m_EditorCamera.GetCamera().GetPosition();
		SetGenericsUniforms(cameraEditorProjectionMatrix, cameraEditorViewMatrix, cameraEditorPosition);


		FBO_Data fboDataEditor;
		fboDataEditor.FBO = &scene_FBO;
		fboDataEditor.depthBuffer = &scene_depthBuffer;
		fboDataEditor.colorBuffers = scene_colorBuffers;
		fboDataEditor.drawSize = sceneSize;
		fboDataEditor.drawPos = scenePos;
		RenderPipeline(cameraEditorProjectionMatrix, cameraEditorViewMatrix, cameraEditorPosition, fboDataEditor);
		//----------------------------------------------------------------------------------------



		//--GAME CAMERAS
		/*for (int i = 0; i < Application::gameObjectManager->cameras.size(); i++)
		{
			glm::mat4 cameraProjectionMatrix = Application::gameObjectManager->cameras[i]->GetProjectionMatrix();
			glm::mat4 cameraViewMatrix = Application::gameObjectManager->cameras[i]->GetViewMatrix();
			glm::vec3 cameraPosition = Application::gameObjectManager->cameras[i]->m_Position;
			SetGenericsUniforms(cameraProjectionMatrix, cameraViewMatrix, cameraPosition);

			FBO_Data fboData;
			fboData.FBO = &Application::gameObjectManager->cameras[i]->FBO;
			fboData.depthBuffer = &Application::gameObjectManager->cameras[i]->depthBuffer;
			fboData.colorBuffers = Application::gameObjectManager->cameras[i]->colorBuffers;
			fboData.drawSize = gameSize;
			fboData.drawPos = gamePos;
			RenderPipeline(cameraProjectionMatrix, cameraViewMatrix, cameraPosition, fboData);
		}*/


		//-ACTIVE SELECTED ENTITY
		Entity selectedEntity = sceneHierarchyPanel->GetSelectedEntity();
		if (selectedEntity)
		{
			if (selectedEntity.HasComponent<MeshRendererComponent>())
			{
				selectedEntity.GetComponent<MeshRendererComponent>().drawLocalBB = true;
			}
		}
		CheckIfPointerIsOverObject();
		//-------------------------------------------------------------------------------------------------------------------------------------------
    }
	void GLCore::Scene::SetGenericsUniforms(glm::mat4 cameraProjectionMatrix, glm::mat4 cameraViewMatrix, glm::vec3 cameraPosition)
	{
		//-GENERICS TO ALL SHADER
		for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
		{
			GLCore::Render::ShaderManager::Get(name.c_str())->use();

			GLCore::Render::ShaderManager::Get(name.c_str())->setBool("useDirLight", useDirectionalLight);
			GLCore::Render::ShaderManager::Get(name.c_str())->setInt("numPointLights", totalPointLight);
			GLCore::Render::ShaderManager::Get(name.c_str())->setInt("numSpotLights", totalSpotLight);

			GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("globalAmbient", globalAmbient);

			GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("projection", cameraProjectionMatrix);
			GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("view", cameraViewMatrix);
			GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("viewPos", cameraPosition);
		}
		//-------------------------------------------------------------------------------------------------------------------------------------------
	}
	void GLCore::Scene::RenderPipeline(glm::mat4 cameraProjectionMatrix, glm::mat4 cameraViewMatrix, glm::vec3 cameraPosition, FBO_Data fboData)
	{

		for (int i = 0; i <= 12; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0); // Desvincula cualquier textura 2D
		}

		//--RENDER-PIPELINE
		// 
		//__1.-SHADOW PASS
		auto viewDirectionalLight = RegistrySingleton::getRegistry().view<TransformComponent,DirectionalLightComponent>();
		for (auto entity : viewDirectionalLight)
		{
			auto [transformComponent,directionalLightComponent] = viewDirectionalLight.get<TransformComponent,DirectionalLightComponent>(entity);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindFramebuffer(GL_FRAMEBUFFER, directionalLightComponent.shadowFBO);
			glViewport(0, 0, directionalLightComponent.shadowMapResolution, directionalLightComponent.shadowMapResolution);
			glClearColor(1, 1, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			glm::mat4 shadowProjMat = glm::ortho(directionalLightComponent.orthoLeft, 
												 directionalLightComponent.orthoRight, 
												 directionalLightComponent.orthoBottom, 
												 directionalLightComponent.orthoTop, directionalLightComponent.orthoNear,   directionalLightComponent.orthoFar);

			glm::mat4 shadowViewMat = glm::lookAt(transformComponent.position, directionalLightComponent.direction, glm::vec3(0, 1, 0));

			directionalLightComponent.shadowMVP = shadowProjMat * shadowViewMat;

			GLCore::Render::ShaderManager::Get("direct_light_depth_shadows")->use();

			auto viewDirectionalLightShadowPass = RegistrySingleton::getRegistry().view<MeshRendererComponent>();
			for (auto entity : viewDirectionalLightShadowPass)
			{
				auto meshRendererComponent = viewDirectionalLightShadowPass.get<MeshRendererComponent>(entity);

				if (meshRendererComponent.dropShadow)
				{
					glm::mat4 entityShadowMVP = directionalLightComponent.shadowMVP * meshRendererComponent.model_transform_matrix;
					GLCore::Render::ShaderManager::Get("direct_light_depth_shadows")->setMat4("shadowMVP", entityShadowMVP);
					GLCore::Render::DrawMesh(&meshRendererComponent);
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		//clear
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		int SlotTextureCounter = 0;

		if (usePostprocessing)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, postproManager->FBO);
			glViewport(0, 0, fboData.drawSize.x, fboData.drawSize.y);
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			//RenderPipeline
			//PASS LIGHTS
			auto viewDirectionalLight = RegistrySingleton::getRegistry().view<TransformComponent, DirectionalLightComponent>();
			for (auto entity : viewDirectionalLight)
			{
				auto [transformComponent, directionalLightComponent] = viewDirectionalLight.get<TransformComponent, DirectionalLightComponent>(entity);

				GLCore::Render::DrawDirectionalLight(&directionalLightComponent, &transformComponent);
			}

			//GEOMETRY PASS
			gridWorldRef->Render();

			auto viewMaterialPass = RegistrySingleton::getRegistry().view<TransformComponent, MaterialComponent, MeshRendererComponent>();

			for (auto entity : viewMaterialPass)
			{
				auto [transform, matComponent, meshRendererComponent] = viewMaterialPass.get<TransformComponent, MaterialComponent, MeshRendererComponent>(entity);

				GLCore::Render::ActiveTextures(&matComponent, 1);
				GLCore::Render::DrawMesh(&meshRendererComponent);
				GLCore::Render::DrawBoundingBox(&meshRendererComponent);
			}

			SlotTextureCounter = 5;
			//--------------------------------------------IBL
			glDepthFunc(GL_LEQUAL);

			GLCore::Render::ShaderManager::Get("pbr_ibl")->use();

			glActiveTexture(GL_TEXTURE0 + SlotTextureCounter);
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("brdfLUT", SlotTextureCounter);
			SlotTextureCounter++;
			glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.envCubemap);

			glActiveTexture(GL_TEXTURE0 + SlotTextureCounter);
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("irradianceMap", SlotTextureCounter);
			SlotTextureCounter++;
			glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.irradianceMap); // display irradiance map

			glActiveTexture(GL_TEXTURE0 + SlotTextureCounter);
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("prefilterMap", SlotTextureCounter);
			SlotTextureCounter++;
			glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.prefilterMap);  // display prefilter map

			GLCore::Render::ShaderManager::Get("pbr_ibl")->setBool("useIBL", useIBL);

			glm::mat4 viewHDR = glm::mat4(glm::mat3(cameraViewMatrix));

			// Escala la matriz de vista para hacer el skybox más grande
			float scale = 1.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
			viewHDR = glm::scale(viewHDR, glm::vec3(scale, scale, scale));
			GLCore::Render::ShaderManager::Get("background")->use();
			GLCore::Render::ShaderManager::Get("background")->setMat4("view", viewHDR);
			GLCore::Render::ShaderManager::Get("background")->setMat4("projection", cameraProjectionMatrix);
			GLCore::Render::ShaderManager::Get("background")->setInt("environmentMap", 0);
			SlotTextureCounter += 1;

			if (showIBLSkybox == true)
			{
				renderCube();
			}
			//------------------------------------------------------------------------------------------------------------------------------


			//__4.-POSTPROCESING
			postproManager->RenderWithPostProcess();
			//-------------------------------------------------------------------------------------------------------------------------------------------

			//__5.-DRAW POSTPROCESED IMAGE TO TEXTURE IN QUAD & DRAW IN SCENE_FBO
			glBindFramebuffer(GL_FRAMEBUFFER, *fboData.FBO);
			glViewport(0, 0, fboData.drawSize.x, fboData.drawSize.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderQuad();
			//-------------------------------------------------------------------------------------------------------------------------------------------

			//__6.-BACK TO DEFAULT FBO AND CONTINUE DRAWING OTHER STUFF´s
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			//-------------------------------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, *fboData.FBO);
			glViewport(0, 0, fboData.drawSize.x, fboData.drawSize.y);
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			//RenderPipeline
			//PASS LIGHTS
			auto viewDirectionalLight = RegistrySingleton::getRegistry().view<TransformComponent, DirectionalLightComponent>();
			for (auto entity : viewDirectionalLight)
			{
				auto [transformComponent, directionalLightComponent] = viewDirectionalLight.get<TransformComponent, DirectionalLightComponent>(entity);

				GLCore::Render::DrawDirectionalLight(&directionalLightComponent, &transformComponent);
			}

			//GEOMETRY PASS
			gridWorldRef->Render();

			auto viewMaterialPass = RegistrySingleton::getRegistry().view<TransformComponent, MaterialComponent, MeshRendererComponent>();
			
			for (auto entity : viewMaterialPass)
			{
				auto [transform, matComponent, meshRendererComponent] = viewMaterialPass.get<TransformComponent, MaterialComponent, MeshRendererComponent>(entity);
				
				GLCore::Render::ActiveTextures(&matComponent, 1);
				GLCore::Render::DrawMesh(&meshRendererComponent);
				GLCore::Render::DrawBoundingBox(&meshRendererComponent);
			}

			SlotTextureCounter = 5;
			//--------------------------------------------IBL
			glDepthFunc(GL_LEQUAL);

			GLCore::Render::ShaderManager::Get("pbr_ibl")->use();


			glActiveTexture(GL_TEXTURE0 + SlotTextureCounter);
			glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.envCubemap);
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("brdfLUT", SlotTextureCounter);
			SlotTextureCounter++;
			

			glActiveTexture(GL_TEXTURE0 + SlotTextureCounter);
			glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.irradianceMap); // display irradiance map
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("irradianceMap", SlotTextureCounter);
			SlotTextureCounter++;
			

			glActiveTexture(GL_TEXTURE0 + SlotTextureCounter);
			glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.prefilterMap);  // display prefilter map
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("prefilterMap", SlotTextureCounter);
			SlotTextureCounter++;
			

			GLCore::Render::ShaderManager::Get("pbr_ibl")->setBool("useIBL", useIBL);

			glm::mat4 viewHDR = glm::mat4(glm::mat3(cameraViewMatrix));

			// Escala la matriz de vista para hacer el skybox más grande
			float scale = 1.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
			viewHDR = glm::scale(viewHDR, glm::vec3(scale, scale, scale));
			GLCore::Render::ShaderManager::Get("background")->use();
			GLCore::Render::ShaderManager::Get("background")->setMat4("view", viewHDR);
			GLCore::Render::ShaderManager::Get("background")->setMat4("projection", cameraProjectionMatrix);

			GLCore::Render::ShaderManager::Get("background")->setInt("environmentMap", 10);
			SlotTextureCounter++;

			if (showIBLSkybox == true)
			{
				renderCube();
			}

			//------------------------------------------------------------------------------------------------------------------------------

			//RENDER FINAL QUAD
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			glViewport(0, 0, fboData.drawSize.x, fboData.drawSize.y);
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0 + SlotTextureCounter);
			glBindTexture(GL_TEXTURE_2D, fboData.colorBuffers[0]);

			GLCore::Render::ShaderManager::Get("main_output_FBO")->use();
			GLCore::Render::ShaderManager::Get("main_output_FBO")->setInt("colorBuffer_0", SlotTextureCounter);

			renderQuad();
		}
		//-------------------------------------------------------------------------------------------------------------------------------------------

	}
	void Scene::renderGUI()
	{
		//------------------------------------------HIERARCHY PANEL-----------------------------------------------------
		sceneHierarchyPanel->OnImGuiRender();
		//---------------------------------------------------------------------------------------------------------------

		//------------------------------------------SYSTEM INFO PANEL
		if (ImGui::Begin("SYSTEM INFO"))
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);

			if (GlobalMemoryStatusEx(&memInfo)) {
				ImGui::Text("Total Physical Memory (RAM): %i MB", memInfo.ullTotalPhys / (1024 * 1024));
				ImGui::Text("Available Physical Memory: %i MB", memInfo.ullAvailPhys / (1024 * 1024));
				ImGui::Text("Total Virtual Memory: %i MB", memInfo.ullTotalVirtual / (1024 * 1024));
				ImGui::Text("Available Virtual Memory: %iMB", memInfo.ullAvailVirtual / (1024 * 1024));
			}
			else {
				ImGui::Text("Error obtaining memory status");
			}
		}
		ImGui::End();
		//---------------------------------------------------------------------------------------------------------------


		//-------------------------------------------EVIROMENT LIGHT PANEL-----------------------------------------------
		if (ImGui::Begin("Enviroment Panel"))
		{
			//--WIRE-FRAME
			if (ImGui::Checkbox("Wireframe Mode", &isWireframe))
			{
				if (isWireframe)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Habilita modo wireframe
				}
				else
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Vuelve al modo normal
				}
			}
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			//----------------------------------------------------------------

			//--IBL
			ImGui::Checkbox("Use IBL", &useIBL);
			if (useIBL == true)
			{
				ImGui::Checkbox("Show HDR SkyBox", &showIBLSkybox);
			}
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			//----------------------------------------------------------------

			//--CLEAR COLOR
			ImGui::ColorEdit3("Clear color", (float*)&clearColor);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			//----------------------------------------------------------------

			//--AMBIENT COLOR
			ImGui::ColorEdit3("Global Ambient", &globalAmbient[0]);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			//----------------------------------------------------------------

			//--POST-PROCESSING
			ImGui::Checkbox("PostPro", &usePostprocessing);

			if (usePostprocessing == true)
			{
				postproManager->DrawGUI_Inspector();
			}
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			//----------------------------------------------------------------


			//ImGui::Checkbox("Skybox", &skybox->isActive);
			//ImGui::Dummy(ImVec2(0.0f, 5.0f));

			//ImGui::Text("Dynamic Skybox");
			//ImGui::ColorEdit3("dayLightColor", (float*)&dynamicSkybox->dayLightColor);
			//ImGui::ColorEdit3("sunsetColor", (float*)&dynamicSkybox->sunsetColor);
			//ImGui::ColorEdit3("dayNightColor", (float*)&dynamicSkybox->dayNightColor);
			//ImGui::ColorEdit3("groundColor", (float*)&dynamicSkybox->groundColor);
			//ImGui::Dummy(ImVec2(0.0f, 3.0f));

			//float sunDiskSizeValue = dynamicSkybox->m_sunDiskSize.x;  // Asumimos que todos los valores son iguales
			//if (ImGui::SliderFloat("Sun disk size", &sunDiskSizeValue, 0.0f, 1.0f, "%.4f")) {
			//	dynamicSkybox->m_sunDiskSize = glm::vec3(sunDiskSizeValue, sunDiskSizeValue, sunDiskSizeValue);
			//}
			//ImGui::Dummy(ImVec2(0.0f, 3.0f));
			//ImGui::SliderFloat("Sun disk m_gradientIntensity", &dynamicSkybox->m_gradientIntensity, 0.0f, 10.0f, "%.4f");
			//ImGui::SliderFloat("Sun disk auraIntensity", &dynamicSkybox->auraIntensity, 0.0f, 1.0f, "%.4f");
			//ImGui::SliderFloat("Sun disk auraSize", &dynamicSkybox->auraSize, 0.0f, 1.0f, "%.4f");
			//ImGui::SliderFloat("Sun disk edgeSoftness", &dynamicSkybox->edgeSoftness, 0.0001f, 0.1f, "%.4f");
			//ImGui::Dummy(ImVec2(0.0f, 3.0f));
			//ImGui::SliderFloat("Sun disk upperBound", &dynamicSkybox->upperBound, 0.00001f, 0.1f, "%.4f");
			//ImGui::SliderFloat("Sun disk lowerBound", &dynamicSkybox->lowerBound, 0.00001f, 0.1f, "%.4f");

		}
		ImGui::End();
		//------------------------------------------------------------------------------------------------


		//-------------------------------------------ASSETS PANEL--------------------------------------
		assetsPanel.OnImGuiRender();
		//------------------------------------------------------------------------------------------------



		//-------------------------------------------SCENE PANEL--------------------------------------------
		ImGui::Begin("SCENE", nullptr);
		sceneSize = ImGui::GetWindowSize();
		scenePos = ImGui::GetWindowPos();

		EventManager::getOnPanelResizedEvent().trigger("SCENE", sceneSize, scenePos);

		ImGui::Image((void*)(intptr_t)scene_colorBuffers[0], ImVec2(sceneSize.x, sceneSize.y), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
		mouseInScene = false;

		if (ImGui::IsWindowHovered())
		{
			currentPanelSize = sceneSize;
			currentPanelPos = scenePos;

			mouseInScene = true;
			checkGizmo();
		}
		ImGui::End();
		//---------------------------------------------------------------------------------------------------


		//-------------------------------------------GAME PANEL--------------------------------------------
		ImGui::Begin("GAME", nullptr);
		gameSize = ImGui::GetWindowSize();
		gamePos = ImGui::GetWindowPos();

		EventManager::getOnPanelResizedEvent().trigger("GAME", gameSize, gamePos);

		/*if (Application::gameObjectManager->cameras.size() > 0)
		{
			ImGui::Image((void*)(intptr_t)Application::gameObjectManager->cameras[0]->colorBuffers[0], ImVec2(gameSize.x, gameSize.y), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
		}
		else
		{
			ImGui::Text("No camera/s added in scene");
		}*/


		mouseInGame = false;
		if (ImGui::IsWindowHovered())
		{
			currentPanelSize = gameSize;
			currentPanelPos = gamePos;

			mouseInGame = true;
			checkGizmo();
		}

		ImGui::End();
		//---------------------------------------------------------------------------------------------------



		//--SHADER EDITOR PANEL
		GLCore::Render::ShaderManager::DrawShaderEditorPanel();
		//---------------------------------------------------------------------------------------------------

		//-------------------------------------------------DIALOGS--------------------------------------
		if (selectingEntity == true)
		{
			ImGui::OpenPopup("Seleccionar Entidad");
		}
		if (ImGui::BeginPopup("Seleccionar Entidad") && selectingEntity == true)
		{
			if (ImGui::IsWindowHovered())
			{
				cursorOverSelectEntityDialog = true;
			}
			else
			{
				cursorOverSelectEntityDialog = false;
			}

			for (size_t i = 0; i < entitiesInRay.size(); ++i)
			{
				if (ImGui::Selectable(entitiesInRay[i].GetName().c_str()))
				{
					sceneHierarchyPanel->SetSelectedEntity(entitiesInRay[i]);
					selectingEntity = false;
					
					Entity selectedEntity = sceneHierarchyPanel->GetSelectedEntity();
					if (selectedEntity)
					{
						selectedEntity.GetComponent<MeshRendererComponent>().drawLocalBB = true;
					}
				}
			}

			ImGui::EndPopup();
		}
		//---------------------------------------------------------------------------------------------------
	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------


	

	//--HELPERS
	void Scene::renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	void Scene::renderCube()
	{
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				 // bottom face
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 // top face
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	void Scene::CalcSceneBundle() { 

		glm::vec3 sceneMinBounds = glm::vec3(FLT_MAX);
		glm::vec3 sceneMaxBounds = glm::vec3(-FLT_MAX);


		auto viewDirectionalLightShadowPass = RegistrySingleton::getRegistry().view<MeshRendererComponent>();
		for (auto entity : viewDirectionalLightShadowPass)
		{
			auto meshRendererComponent = viewDirectionalLightShadowPass.get<MeshRendererComponent>(entity);

			if (meshRendererComponent.dropShadow)
			{

				glm::mat4 transform = meshRendererComponent.model_transform_matrix;

				for (int i = 0; i < 8; ++i) {
					glm::vec3 corner = glm::vec3(
						(i & 1) ? meshRendererComponent.meshData->maxBounds.x : meshRendererComponent.meshData->minBounds.x,
						(i & 2) ? meshRendererComponent.meshData->maxBounds.y : meshRendererComponent.meshData->minBounds.y,
						(i & 4) ? meshRendererComponent.meshData->maxBounds.z : meshRendererComponent.meshData->minBounds.z
					);
					glm::vec3 worldCorner = transform * glm::vec4(corner, 1.0f);
					sceneMinBounds = glm::min(sceneMinBounds, worldCorner);
					sceneMaxBounds = glm::max(sceneMaxBounds, worldCorner);
				}
			}
		}



		/*for (ECS::Entity* entity : rendererManager->entitiesInScene)
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
		}*/

		glm::vec3 sceneCenter = (sceneMinBounds + sceneMaxBounds) * 0.5f;
		float sceneRadius = glm::length(sceneMaxBounds - sceneMinBounds) * 0.5f;

		if (!std::isinf(sceneRadius)) {
			SceneBounds = std::make_pair(sceneCenter, sceneRadius);
		}
	}
	void Scene::checkGizmo()
	{
		Entity selectedEntity = sceneHierarchyPanel->GetSelectedEntity();

		//---------------------------ImGUIZMO------------------------------------------
		if (selectedEntity)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(scenePos.x, scenePos.y, sceneSize.x, sceneSize.y);

			glm::mat4 camera_view = m_EditorCamera.GetCamera().GetViewMatrix();
			glm::mat4 camera_projection = m_EditorCamera.GetCamera().GetProjectionMatrix();

			glm::mat4 entity_transform = selectedEntity.GetComponent<TransformComponent>().getLocalModelMatrix();

			// Comprobación de parentesco
			bool hasParent = false;
			if (selectedEntity.HasComponent<ParentComponent>()) {
				Entity entityParent{ selectedEntity.GetComponent<ParentComponent>().parentEntity, this };
				entity_transform = entityParent.GetComponent<TransformComponent>().getLocalModelMatrix() * entity_transform;
				hasParent = true;
			}


			switch (m_GizmoOperation)
			{
			case GizmoOperation::Translate:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Rotate3D:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::ROTATE, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Scale:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::SCALE, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			}

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale, skew;
				glm::quat orientation;
				glm::vec4 perspective;

				glm::decompose(entity_transform, scale, orientation, translation, skew, perspective);

				// Cálculo de la transformación local
				if (hasParent) 
				{

					Entity entityParent{ selectedEntity.GetComponent<ParentComponent>().parentEntity, this };
					glm::mat4 parent_transform = entityParent.GetComponent<TransformComponent>().getLocalModelMatrix();
					glm::mat4 local_transform = glm::inverse(parent_transform) * entity_transform;
					glm::decompose(local_transform, scale, orientation, translation, skew, perspective);
				}

				selectedEntity.GetComponent<TransformComponent>().rotation = orientation;
				selectedEntity.GetComponent<TransformComponent>().position = translation;
				selectedEntity.GetComponent<TransformComponent>().scale = scale;
			}
		}
	}
	void Scene::getModelPathFromAssets(ImportOptions importOptions)
	{
		importOptions.modelID = 0;
		loadFileModel(importOptions);
	}
	void Scene::loadFileModel(ImportOptions importOptions)
	{
		ModelParent modelParent = {};

		//try {
		modelParent = GLCore::Utils::ModelLoader::LoadModel(importOptions);

		Entity entityParent = CreateEntity(modelParent.name);

		if (modelParent.modelInfos.size() > 1)
		{
			auto& parentChildrenComponent = RegistrySingleton::getRegistry().get_or_emplace<ChildrenComponent>(entityParent);

			for (int i = 0; i < modelParent.modelInfos.size(); i++)
			{
				Entity entityChild = CreateEntity(modelParent.modelInfos[i].meshData->meshName + std::to_string(i));

				// Emplace el componente ParentComponent que apunta al padre
				auto& childParentComponent = RegistrySingleton::getRegistry().emplace<ParentComponent>(entityChild, entityParent);

				// Agregar la entidad del hijo al componente ChildrenComponent del padre
				parentChildrenComponent.childEntities.push_back(entityChild);

				auto& meshFilterComponent = RegistrySingleton::getRegistry().emplace<MeshFilterComponent>(entityChild, modelParent.modelInfos[i].meshData, EXTERNAL_FILE);
				auto& meshRendererComponent = RegistrySingleton::getRegistry().emplace<MeshRendererComponent>(entityChild);
				auto& transformComponent = RegistrySingleton::getRegistry().get<TransformComponent>(entityChild);

				meshFilterComponent.modelPath = importOptions.filePath + importOptions.fileName;
				meshRendererComponent.meshData = modelParent.modelInfos[i].meshData;
				meshRendererComponent.meshData->meshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
				meshRendererComponent.meshData->meshRotation = glm::vec3(0.0f, 0.0f, 0.0f);
				meshRendererComponent.meshData->meshScale = glm::vec3(1.0f, 1.0f, 1.0f);
				transformComponent.position = meshRendererComponent.meshData->meshLocalPosition;

				RegistrySingleton::getRegistry().emplace<MaterialComponent>(entityChild).materialData = modelParent.modelInfos[i].model_textures;
			}
		}
		else
		{
			RegistrySingleton::getRegistry().emplace<MeshFilterComponent>(entityParent, modelParent.modelInfos[0].meshData, EXTERNAL_FILE);
			RegistrySingleton::getRegistry().emplace<MeshRendererComponent>(entityParent);

			MeshFilterComponent* meshFilterComponent = &RegistrySingleton::getRegistry().get<MeshFilterComponent>(entityParent);
			MeshRendererComponent* meshRendererComponent = &RegistrySingleton::getRegistry().get<MeshRendererComponent>(entityParent);
			TransformComponent* transformComponent = &RegistrySingleton::getRegistry().get<TransformComponent>(entityParent);

			meshFilterComponent->modelPath = importOptions.filePath + importOptions.fileName;

			meshRendererComponent->meshData = modelParent.modelInfos[0].meshData;

			meshRendererComponent->meshData->meshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshRotation = glm::vec3(0.0f, 0.0f, 0.0f);
			meshRendererComponent->meshData->meshScale = glm::vec3(1.0f, 1.0f, 1.0f);

			transformComponent->position = meshRendererComponent->meshData->meshLocalPosition;

			RegistrySingleton::getRegistry().emplace<MaterialComponent>(entityParent).materialData = modelParent.modelInfos[0].model_textures;
		}

		

		/*if (entityParent != nullptr)
		{
			m_SelectedEntity = entityParent;
		}
		}
		catch (std::runtime_error& e) {
			std::cerr << "Error cargando el modelo: " << e.what() << std::endl;
		}*/
	}
	void Scene::CheckIfPointerIsOverObject()
	{
		float mouseX, mouseY;
		std::tie(mouseX, mouseY) = InputManager::Instance().GetMousePosition();

		mouseX += scenePos.x;
		mouseY -= scenePos.y;

		if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
		{
			return;
		}

		if (selectingEntity == true)
		{
			return;
		}

		selectingEntity = false;


		if (InputManager::Instance().IsMouseButtonJustReleased(GLFW_MOUSE_BUTTON_LEFT) && mouseInScene)
		{
			pickingObj = false;
		}

		// Aquí empieza el raycasting
		if (InputManager::Instance().IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && mouseInScene)
		{
			if (pickingObj) return; //Si esta bool está a true, retornará, y significa que hemos pulsado ya el mouse y hasta que no soltemos el boton, no se devuelve a false

			//llevamos un punto 2D a un espacio 3D (mouse position -> escene)
			float normalizedX = (2.0f * mouseX) / currentPanelSize.x - 1.0f;
			float normalizedY = ((2.0f * mouseY) / currentPanelSize.y - 1.0f) * -1.0f;
			glm::vec3 clipSpaceCoordinates(normalizedX, normalizedY, 1.0);

			glm::vec4 homogenousCoordinates = glm::inverse(m_EditorCamera.GetCamera().GetProjectionMatrix() *
				m_EditorCamera.GetCamera().GetViewMatrix()) * glm::vec4(clipSpaceCoordinates, 1.0);
			glm::vec3 worldCoordinates = glm::vec3(homogenousCoordinates / homogenousCoordinates.w);


			//Preparamos el rayo para lanzarlo desde la camara hasta la posicion del mouse ya convertido al espacio 3D
			glm::vec3 rayOrigin = m_EditorCamera.GetCamera().GetPosition();
			glm::vec3 rayDirection = glm::normalize(worldCoordinates - rayOrigin);
			glm::vec3 rayEnd = glm::vec3(0.0);


			entitiesInRay.clear();

			auto viewMeshRendererComponent = RegistrySingleton::getRegistry().view<MeshRendererComponent>();
			for (auto entity : viewMeshRendererComponent)
			{
				MeshRendererComponent meshRendererComponent = viewMeshRendererComponent.get<MeshRendererComponent>(entity);

				// Obtener la matriz de transformación actual
				const glm::mat4& transform = meshRendererComponent.model_transform_matrix;

				// Transformar los vértices min y max de la Bounding Box
				glm::vec3 transformedMin = glm::vec3(transform * glm::vec4(meshRendererComponent.meshData->minBounds, 1.0f));
				glm::vec3 transformedMax = glm::vec3(transform * glm::vec4(meshRendererComponent.meshData->maxBounds, 1.0f));

				// Verificar la intersección del rayo
				if (rayIntersectsBoundingBox(rayOrigin, rayDirection, transformedMin, transformedMax))
				{
					Entity selectedEntity(entity, this);
					entitiesInRay.push_back(selectedEntity);
				}
			}

			int entitiesInRaySize = entitiesInRay.size();


			//Si ya hay algo seleccionado lo apaga
			Entity selectedEntity = sceneHierarchyPanel->GetSelectedEntity();
			if (selectedEntity)
			{
				if (selectedEntity.HasComponent<MeshRendererComponent>())
				{
					selectedEntity.GetComponent<MeshRendererComponent>().drawLocalBB = false;
				}
			}
			sceneHierarchyPanel->SetSelectedEntity({});


			if (entitiesInRaySize == 0)
			{
				selectingEntity = false;
			}
			else
			{
				if (entitiesInRaySize == 1)
				{
					sceneHierarchyPanel->SetSelectedEntity(entitiesInRay[0]);
					entitiesInRay[0].GetComponent<MeshRendererComponent>().drawLocalBB = true;
					selectingEntity = false;
				}
				else if (entitiesInRaySize > 1)
				{
					selectingEntity = true;
				}
			}
			
			//Flag para evitar que se vuelva a pasar por esta funcion hasta que se levante el dedo del boton del mouse
			pickingObj = true;
		}
	}
	bool Scene::rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax)
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
	void Scene::shutdown() {}




	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshFilterComponent>(Entity entity, MeshFilterComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
	{
		
	}	

	template<>
	void Scene::OnComponentAdded<ParentComponent>(Entity entity, ParentComponent& component){}

	template<>
	void Scene::OnComponentAdded<ChildrenComponent>(Entity entity, ChildrenComponent& component){}
}