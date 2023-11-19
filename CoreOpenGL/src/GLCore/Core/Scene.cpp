#include "Scene.h"
#include <iostream>
#include "Application.h"
#include "../Render/ShaderManager.h"
#include "../Render/RendererManager.h"
#include <imGizmo/ImGuizmo.h>
#include "../Util/ModelLoader.h"


#include "ECS/Transform.h"
#include "ECS/MeshRenderer.h"
#include "ECS/Camera.h"


namespace GLCore 
{

	Render::RendererManager* rendererManager = new Render::RendererManager();

	const int BUF_SIZE = 256;
	char textBuffer[BUF_SIZE] = "CsTestComponent"; // Buffer para ImGui
	std::string inputString = textBuffer; // std::string sincronizado con textBuffer

    Scene::Scene() : m_EditorCamera(16.0f / 9.0f) {}
    Scene::~Scene(){shutdown();}

	//-INIT
    bool Scene::initialize()
    {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        //--CAMARA
        m_EditorCamera.GetCamera().SetPosition(glm::vec3(-1.492, 6.195f, 25.196f));
        m_EditorCamera.GetCamera().SetRotation(glm::vec2(3.2f, -451.199f));
        //----------------------------------------------------------------------------------------------------------------------------


		//--LOAD SHADERS
		GLCore::Render::ShaderManager::LoadAllShaders();
		//----------------------------------------------------------------------------------------------------------------------------


		//--SKYBOX
		std::vector<const char*> faces
		{
			"assets/default/Skybox/right.jpg",
			"assets/default/Skybox/left.jpg",
			"assets/default/Skybox/top.jpg",
			"assets/default/Skybox/bottom.jpg",
			"assets/default/Skybox/front.jpg",
			"assets/default/Skybox/back.jpg"
		};
		skybox = new Utils::Skybox(faces);
		dynamicSkybox = new Utils::DynamicSkybox(faces);
		//----------------------------------------------------------------------------------------------------------------------------


		//--GRID WORLD REFENRENCE
		gridWorldRef = new Utils::GridWorldReference();
		//----------------------------------------------------------------------------------------------------------------------------


		//--IBL
		iblManager.prepare_PBR_IBL(800, 600);
		//--------------------------------------------------------------------------------------------------------------------------------


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


		Application::gameObjectManager->createGameObject(MainMenuAction::AddCube);

        return true;
    }
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------


	//-MAIN LOOP
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


		//--ENTITIES
		Application::gameObjectManager->manager.refresh();
		Application::gameObjectManager->manager.update(deltaTime);
		rendererManager->entitiesInScene = Application::gameObjectManager->manager.getAllEntities();

		if (rendererManager->entitiesInScene.size() > 0)
		{
			rendererManager->CalcSceneBundle();
		}
		//----------------------------------------------------------------------------------------------------------------------------



        //--EDITOR CAMERA
        aspectRatio = static_cast<float>(sceneSize.x) / static_cast<float>(sceneSize.y);
        m_EditorCamera.GetCamera().SetProjection(m_EditorCamera.GetCamera().GetFov(), aspectRatio, 0.1f, 100.0f);
        m_EditorCamera.OnUpdate(deltaTime);
        //----------------------------------------------------------------------------------------------------------------------------


		//--GAME CAMERAS
		for (int i = 0; i < Application::gameObjectManager->cameras.size(); i++)
		{
			Application::gameObjectManager->cameras[i]->SetProjection(Application::gameObjectManager->cameras[i]->GetFov(), gameSize.x / gameSize.y, 0.1f, 100.0f);
		}
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
		for (int i = 0; i < Application::gameObjectManager->cameras.size(); i++)
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
		}


		//-ACTIVE SELECTED ENTITY
		/*if (Application::gameObjectManager->m_SelectedEntity != nullptr && Application::gameObjectManager->m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
		{
			Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = true;
		}*/
		CheckIfPointerIsOverObject();
		//-------------------------------------------------------------------------------------------------------------------------------------------
    }

	void GLCore::Scene::SetGenericsUniforms(glm::mat4 cameraProjectionMatrix, glm::mat4 cameraViewMatrix, glm::vec3 cameraPosition)
	{
		//-GENERICS TO ALL SHADER
		for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
		{
			GLCore::Render::ShaderManager::Get(name.c_str())->use();

			GLCore::Render::ShaderManager::Get(name.c_str())->setBool("useDirLight", Application::gameObjectManager->useDirectionalLight);
			GLCore::Render::ShaderManager::Get(name.c_str())->setInt("numPointLights", Application::gameObjectManager->totalPointLight);
			GLCore::Render::ShaderManager::Get(name.c_str())->setInt("numSpotLights", Application::gameObjectManager->totalSpotLight);

			GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("globalAmbient", globalAmbient);

			GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("projection", cameraProjectionMatrix);
			GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("view", cameraViewMatrix);
			GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("viewPos", cameraPosition);
		}
		//-------------------------------------------------------------------------------------------------------------------------------------------
	}
	
	void GLCore::Scene::RenderPipeline(glm::mat4 cameraProjectionMatrix, glm::mat4 cameraViewMatrix, glm::vec3 cameraPosition, FBO_Data fboData)
	{
		//--RENDER-PIPELINE

		//__1.-SHADOW PASS
		rendererManager->passShadow();

		//clear
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (usePostprocessing)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, postproManager->FBO);
			glViewport(0, 0, fboData.drawSize.x, fboData.drawSize.y);
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			//--------------------------------------------IBL
			if (useIBL == true)
			{
				glDepthFunc(GL_LEQUAL);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.envCubemap);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.irradianceMap); // display irradiance map
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.prefilterMap);  // display prefilter map

				GLCore::Render::ShaderManager::Get("pbr_ibl")->use();
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("irradianceMap", 0);
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("prefilterMap", 1);
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("brdfLUT", 2);

				if (showIBLSkybox == true)
				{
					glm::mat4 viewHDR = glm::mat4(glm::mat3(cameraViewMatrix));

					// Escala la matriz de vista para hacer el skybox más grande
					float scale = 1.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
					viewHDR = glm::scale(viewHDR, glm::vec3(scale, scale, scale));
					GLCore::Render::ShaderManager::Get("background")->use();
					GLCore::Render::ShaderManager::Get("background")->setMat4("view", viewHDR);
					GLCore::Render::ShaderManager::Get("background")->setMat4("projection", cameraProjectionMatrix);
					GLCore::Render::ShaderManager::Get("background")->setInt("environmentMap", 0);
					renderCube();
				}
			}
			else
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
			//------------------------------------------------------------------------------------------------------------------------------

			



			//__2.-LIGHT PASS
			rendererManager->passLights();

			//__3.-GEOMETRY PASS
			gridWorldRef->Render();
			rendererManager->passGeometry();

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

			//--------------------------------------------IBL
			if (useIBL == true)
			{
				glDepthFunc(GL_LEQUAL);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.envCubemap);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.irradianceMap); // display irradiance map
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, iblManager.prefilterMap);  // display prefilter map

				GLCore::Render::ShaderManager::Get("pbr_ibl")->use();
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("irradianceMap", 0);
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("prefilterMap", 1);
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("brdfLUT", 2);

				if (showIBLSkybox == true)
				{
					glm::mat4 viewHDR = glm::mat4(glm::mat3(cameraViewMatrix));

					// Escala la matriz de vista para hacer el skybox más grande
					float scale = 1.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
					viewHDR = glm::scale(viewHDR, glm::vec3(scale, scale, scale));
					GLCore::Render::ShaderManager::Get("background")->use();
					GLCore::Render::ShaderManager::Get("background")->setMat4("view", viewHDR);
					GLCore::Render::ShaderManager::Get("background")->setMat4("projection", cameraProjectionMatrix);
					GLCore::Render::ShaderManager::Get("background")->setInt("environmentMap", 0);
					renderCube();
				}
			}
			else
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
			//------------------------------------------------------------------------------------------------------------------------------

			//RenderPipeline
			rendererManager->passLights();
			gridWorldRef->Render();
			rendererManager->passGeometry();

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, fboData.colorBuffers[0]);

			GLCore::Render::ShaderManager::Get("main_output_FBO")->use();
			GLCore::Render::ShaderManager::Get("main_output_FBO")->setInt("colorBuffer_0", 5);


			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			glViewport(0, 0, fboData.drawSize.x, fboData.drawSize.y);
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderQuad();
		}
		//-------------------------------------------------------------------------------------------------------------------------------------------

	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------




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

	void Scene::checkGizmo()
	{
		//---------------------------ImGUIZMO------------------------------------------
		if (Application::gameObjectManager->m_SelectedEntity != nullptr)
		{
			if (Application::gameObjectManager->m_SelectedEntity->hascomponent<ECS::Transform>())
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(scenePos.x, scenePos.y, sceneSize.x, sceneSize.y);

				glm::mat4 camera_view = m_EditorCamera.GetCamera().GetViewMatrix();
				glm::mat4 camera_projection = m_EditorCamera.GetCamera().GetProjectionMatrix();

				glm::mat4 entity_transform = Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().getLocalModelMatrix();

				// Comprobación de parentesco
				if (Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().parent != nullptr) {
					entity_transform = Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().getLocalModelMatrix() * entity_transform;
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
					if (Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().parent != nullptr) {
						glm::mat4 parent_transform = Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().getLocalModelMatrix();
						glm::mat4 local_transform = glm::inverse(parent_transform) * entity_transform;
						glm::decompose(local_transform, scale, orientation, translation, skew, perspective);
					}

					Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().rotation = orientation;
					Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().position = translation;
					Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::Transform>().scale = scale;
				}
			}
		}
	}

	void Scene::renderGUI()
    {
		//------------------------------------------HIERARCHY PANEL-----------------------------------------------------
		Application::gameObjectManager->drawHierarchy();
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


		//-------------------------------------------INSPECTOR PANEL--------------------------------------
		ImGui::Begin("Inspector", nullptr);
		if (Application::gameObjectManager->m_SelectedEntity != nullptr)
		{
			// Input Text con buffer de caracteres
			if (ImGui::InputText("##input", textBuffer, BUF_SIZE))
			{
				// Actualizar std::string cuando el texto cambia
				inputString = textBuffer;
			}

			ImGui::SameLine();

			// Crear un botón y comprobar si se ha presionado
			if (ImGui::Button("->"))
			{
				// Llamar a MyFunction con el texto del campo de texto
				//Application::gameObjectManager->addCsComponentToSelectedGameObject(inputString);
			}

			const auto& comps = Application::gameObjectManager->m_SelectedEntity->getComponents();
			for (const auto& component : comps) 
			{
				if (component) {
					component->drawGUI_Inspector();
				}
			}
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
		gamePos  = ImGui::GetWindowPos();
		
		EventManager::getOnPanelResizedEvent().trigger("GAME", gameSize, gamePos);

		if (Application::gameObjectManager->cameras.size() > 0)
		{
			ImGui::Image((void*)(intptr_t)Application::gameObjectManager->cameras[0]->colorBuffers[0], ImVec2(gameSize.x, gameSize.y), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
		}
		else
		{
			ImGui::Text("No camera/s added in scene");
		}
		
		
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

		////-------------------------------------------------DIALOGS--------------------------------------
		//if (selectingEntity == true)
		//{
		//	ImGui::OpenPopup("Seleccionar Entidad");
		//}
		//
		//if (ImGui::BeginPopup("Seleccionar Entidad") && selectingEntity == true)
		//{
		//	if (ImGui::IsWindowHovered())
		//	{
		//		cursorOverSelectEntityDialog = true;
		//	}
		//	else
		//	{
		//		cursorOverSelectEntityDialog = false;
		//	}

		//	for (size_t i = 0; i < entitiesInRay.size(); ++i)
		//	{
		//		if (ImGui::Selectable(entitiesInRay[i]->name.c_str()))
		//		{
		//			//Application::gameObjectManager->m_SelectedEntity = entitiesInRay[i];
		//			//selectingEntity = false;
		//			//if (Application::gameObjectManager->m_SelectedEntity) //Si hemos obtenido un objeto, revisamos si tiene posibilidad de drawable y en ese caso activamos su BB
		//			//	if (Application::gameObjectManager->m_SelectedEntity->hascomponent<ECS::MeshRenderer>()) 
		//			//		Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = true;
		//		}
		//	}
		//	ImGui::EndPopup();
		//}
		//---------------------------------------------------------------------------------------------------
    }

	void Scene::getModelPathFromAssets(ImportOptions importOptions)
	{
		importOptions.modelID = rendererManager->entitiesInScene.size() + 1;
		Application::gameObjectManager->loadFileModel(importOptions);
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

			if (Application::gameObjectManager->m_SelectedEntity != nullptr) //Si ya existe un objeto seleccionado y tiene drawable, desactivamos su BB
				if (Application::gameObjectManager->m_SelectedEntity->hascomponent<ECS::MeshRenderer>()) Application::gameObjectManager->m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = false;

			Application::gameObjectManager->m_SelectedEntity = nullptr; //Reset de la variable que almacena la entity seleccioanda preparandola para recibit o no una nueva selección

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


			selectingEntity = Application::gameObjectManager->CheckIfGameObjectInRay(rayOrigin, rayDirection);

			//Flag para evitar que se vuelva a pasar por esta funcion hasta que se levante el dedo del boton del mouse
			pickingObj = true;
		}
	}
	
	void Scene::shutdown() {}
}