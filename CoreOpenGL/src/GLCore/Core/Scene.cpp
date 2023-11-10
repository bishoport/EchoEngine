#include "Scene.h"
#include <iostream>
#include "Application.h"
#include "../Render/ShaderManager.h"
#include "../Render/RendererManager.h"
#include <imGizmo/ImGuizmo.h>

#include "../../ECS/MeshFilter.h"
#include "../../ECS/MeshRenderer.h"
#include "../../ECS/Material.h"
#include "../../ECS/PointLight.h"
#include "../../ECS/SpotLight.h"
#include "../../ECS/DirectionalLight.h"
#include "../../ECS/CharacterController.h"


#include "../Util/ModelLoader.h"
#include "../../ECS/Camera.h"


#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep


namespace GLCore {

	std::pair<glm::vec3, float> Scene::SceneBounds = { glm::vec3(0.0f), 0.0f };
	
	Render::RendererManager* rendererManager = new Render::RendererManager();
	
    Scene::Scene() : m_EditorCamera(16.0f / 9.0f) {}

    Scene::~Scene(){shutdown();}

    bool Scene::initialize()
    {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        //--CAMARA
        m_EditorCamera.GetCamera().SetPosition(glm::vec3(-1.492, 6.195f, 25.196f));
        m_EditorCamera.GetCamera().SetRotation(glm::vec2(3.2f, -451.199f));
        //----------------------------------------------------------------------------------------------------------------------------


        //--LOAD SHADERS
        GLCore::Render::ShaderManager::Load("pbr",           "assets/shaders/Default.vert",		       "assets/shaders/pbr.fs");
		GLCore::Render::ShaderManager::Load("pbr_ibl",       "assets/shaders/Default.vert",            "assets/shaders/pbr_ibl.fs");
        GLCore::Render::ShaderManager::Load("debug",         "assets/shaders/Debug.vert",              "assets/shaders/Debug.frag");
        GLCore::Render::ShaderManager::Load("skybox",        "assets/shaders/skybox/skybox.vs",        "assets/shaders/skybox/skybox.fs");
        GLCore::Render::ShaderManager::Load("dynamicSkybox", "assets/shaders/skybox/dynamicSkybox.vs", "assets/shaders/skybox/dynamicSkybox.fs");


        GLCore::Render::ShaderManager::Load("direct_light_depth_shadows",      
											"assets/shaders/shadows/directLight_shadow_mapping_depth_shader.vs", 
											"assets/shaders/shadows/directLight_shadow_mapping_depth_shader.fs");

		GLCore::Render::ShaderManager::Load("spotLight_depth_shadows",
											"assets/shaders/shadows/spotLight_shadow_map_depth.vs",
											"assets/shaders/shadows/spotLight_shadow_map_depth.fs");

		GLCore::Render::ShaderManager::Load("pointLight_depth_shadows",
											"assets/shaders/shadows/pointLight_shadow_mapping_depth_shader.vs",
											"assets/shaders/shadows/pointLight_shadow_mapping_depth_shader.fs",
											"assets/shaders/shadows/pointLight_shadow_mapping_depth_shader.gs");

		GLCore::Render::ShaderManager::Load("postprocessing", "assets/shaders/postpro/postprocessing.vs", "assets/shaders/postpro/postprocessing.fs");
		GLCore::Render::ShaderManager::Load("main_output_FBO", "assets/shaders/main_output_FBO.vs", "assets/shaders/main_output_FBO.fs");


		//--IBL
		GLCore::Render::ShaderManager::Load("equirectangularToCubemap","assets/shaders/IBL/cubemap.vs","assets/shaders/IBL/equirectangular_to_cubemap.fs");
		GLCore::Render::ShaderManager::Load("irradiance","assets/shaders/IBL/cubemap.vs","assets/shaders/IBL/irradiance_convolution.fs");
		GLCore::Render::ShaderManager::Load("prefilter","assets/shaders/IBL/cubemap.vs","assets/shaders/IBL/prefilter.fs");
		GLCore::Render::ShaderManager::Load("brdf","assets/shaders/IBL/brdf.vs","assets/shaders/IBL/brdf.fs");
		GLCore::Render::ShaderManager::Load("background","assets/shaders/IBL/background.vs","assets/shaders/IBL/background.fs");
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


		createGameObject(MainMenuAction::AddCube);

        return true;
    }



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
        manager.refresh();
        manager.update();

		rendererManager->entitiesInScene = manager.getAllEntities();
		
		if (rendererManager->entitiesInScene.size() > 0)
		{
			CalcSceneBundle();
		}
		//----------------------------------------------------------------------------------------------------------------------------


        //--EDITOR CAMERA
        aspectRatio = static_cast<float>(sceneSize.x) / static_cast<float>(sceneSize.y);

        m_EditorCamera.GetCamera().SetProjection(m_EditorCamera.GetCamera().GetFov(), aspectRatio, 0.1f, 100.0f);
        m_EditorCamera.OnUpdate(deltaTime);
        //----------------------------------------------------------------------------------------------------------------------------

		//--GAME CAMERAS
		for (int i = 0; i < cameras.size(); i++)
		{
			cameras[i]->SetProjection(cameras[i]->GetFov(), gameSize.x / gameSize.y, 0.1f, 100.0f);
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
		for (int i = 0; i < cameras.size(); i++)
		{
			glm::mat4 cameraProjectionMatrix = cameras[i]->GetProjectionMatrix();
			glm::mat4 cameraViewMatrix = cameras[i]->GetViewMatrix();
			glm::vec3 cameraPosition = cameras[i]->m_Position;
			SetGenericsUniforms(cameraProjectionMatrix, cameraViewMatrix, cameraPosition);

			FBO_Data fboData;
			fboData.FBO = &cameras[i]->FBO;
			fboData.depthBuffer = &cameras[i]->depthBuffer;
			fboData.colorBuffers = cameras[i]->colorBuffers;
			fboData.drawSize = gameSize;
			fboData.drawPos = gamePos;
			RenderPipeline(cameraProjectionMatrix, cameraViewMatrix, cameraPosition, fboData);
		}


		//-ACTIVE SELECTED ENTITY
		if (m_SelectedEntity != nullptr && m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
		{
			m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = true;
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

	void Scene::CalcSceneBundle() { 

		glm::vec3 sceneMinBounds = glm::vec3(FLT_MAX);
		glm::vec3 sceneMaxBounds = glm::vec3(-FLT_MAX);

		for (ECS::Entity* entity : rendererManager->entitiesInScene)
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

	void Scene::checkGizmo()
	{
		//---------------------------ImGUIZMO------------------------------------------
		if (m_SelectedEntity != nullptr)
		{
			if (m_SelectedEntity->hascomponent<ECS::Transform>())
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(scenePos.x, scenePos.y, sceneSize.x, sceneSize.y);

				glm::mat4 camera_view = m_EditorCamera.GetCamera().GetViewMatrix();
				glm::mat4 camera_projection = m_EditorCamera.GetCamera().GetProjectionMatrix();

				glm::mat4 entity_transform = m_SelectedEntity->getComponent<ECS::Transform>().getLocalModelMatrix();

				// Comprobación de parentesco
				if (m_SelectedEntity->getComponent<ECS::Transform>().parent != nullptr) {
					entity_transform = m_SelectedEntity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().getLocalModelMatrix() * entity_transform;
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
					if (m_SelectedEntity->getComponent<ECS::Transform>().parent != nullptr) {
						glm::mat4 parent_transform = m_SelectedEntity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().getLocalModelMatrix();
						glm::mat4 local_transform = glm::inverse(parent_transform) * entity_transform;
						glm::decompose(local_transform, scale, orientation, translation, skew, perspective);
					}

					m_SelectedEntity->getComponent<ECS::Transform>().rotation = orientation;
					m_SelectedEntity->getComponent<ECS::Transform>().position = translation;
					m_SelectedEntity->getComponent<ECS::Transform>().scale = scale;
				}
			}
		}
	}

	void Scene::renderGUI()
    {
		//-------------------------------------------HIERARCHY PANEL--------------------------------------
		ImGui::Begin("Hierarchy", nullptr);

		for (int i = 0; i < rendererManager->entitiesInScene.size(); i++)
		{
			if (rendererManager->entitiesInScene[i]->getComponent<ECS::Transform>().parent == nullptr) // Only root entities
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				if (m_SelectedEntity == rendererManager->entitiesInScene[i])
					node_flags |= ImGuiTreeNodeFlags_Selected;

				std::string treeLabel = rendererManager->entitiesInScene[i]->name;

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
					m_SelectedEntity = rendererManager->entitiesInScene[i];
				}

				if (m_SelectedEntity != nullptr)
				{
					if (ImGui::BeginPopupContextItem()) { // Click derecho en el elemento
						if (ImGui::MenuItem("Delete"))
						{
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
						auto& sourceEntity = rendererManager->entitiesInScene[sourceEntityIndex]; // Entidad que estás arrastrando
						auto& targetEntity = rendererManager->entitiesInScene[i]; // Entidad sobre la cual se hace el "drop"

						sourceEntity->getComponent<ECS::Transform>().parent = targetEntity;
						targetEntity->getComponent<ECS::Transform>().children.push_back(sourceEntity);

						//targetEntity->getComponent<ECS::Transform>().addChild(sourceEntity);
					}
					ImGui::EndDragDropTarget();
				}
				//--END DRAG_DROP



				if (treeOpen)
				{
					for (int j = 0; j < rendererManager->entitiesInScene[i]->getComponent<ECS::Transform>().children.size(); j++)
					{
						auto& child = rendererManager->entitiesInScene[i]->getComponent<ECS::Transform>().children[j];

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
								auto& sourceEntity = rendererManager->entitiesInScene[sourceEntityIndex]; // Entidad que estás arrastrando
								auto& targetEntity = rendererManager->entitiesInScene[i]; // Entidad sobre la cual se hace el "drop"

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
		ImGui::End();
		//---------------------------------------------------------------------------------------------------------------




		//-------------------------------------------EVIROMENT LIGHT PANEL--------------------------------------
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
		if (m_SelectedEntity != nullptr)
		{
			const auto& comps = m_SelectedEntity->getComponents();
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

		if (cameras.size() > 0)
		{
			ImGui::Image((void*)(intptr_t)cameras[0]->colorBuffers[0], ImVec2(gameSize.x, gameSize.y), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
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
				if (ImGui::Selectable(entitiesInRay[i]->name.c_str()))
				{
					m_SelectedEntity = entitiesInRay[i];
					selectingEntity = false;
					if (m_SelectedEntity) //Si hemos obtenido un objeto, revisamos si tiene posibilidad de drawable y en ese caso activamos su BB
						if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>()) m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = true;
				}
			}
			ImGui::EndPopup();
		}
		//---------------------------------------------------------------------------------------------------
    }

	void Scene::createGameObject(MainMenuAction action)
	{
		ECS::Entity* gameObject = nullptr;

		if (action == MainMenuAction::AddCube)
		{
			GLCore::MeshData cube = GLCore::Render::PrimitivesHelper::CreateCube();

			gameObject = &manager.addEntity();
			gameObject->name = "Cube_" + std::to_string(rendererManager->entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(cube);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_CUBE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();	
		}
		else if (action == MainMenuAction::AddSegmentedCube)
		{
			GLCore::MeshData segCube = GLCore::Render::PrimitivesHelper::CreateSegmentedCube(1);

			gameObject = &manager.addEntity();
			gameObject->name = "SegCube_" + std::to_string(rendererManager->entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(segCube);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_SEGMENTED_CUBE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddSphere)
		{
			GLCore::MeshData sphere = GLCore::Render::PrimitivesHelper::CreateSphere(1, 20, 20);

			gameObject = &manager.addEntity();
			gameObject->name = "Sphere_" + std::to_string(rendererManager->entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(sphere);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_SPHERE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddQuad)
		{
			GLCore::MeshData quad = GLCore::Render::PrimitivesHelper::CreateQuad();

			gameObject = &manager.addEntity();
			gameObject->name = "Quad_" + std::to_string(rendererManager->entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(quad);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_QUAD;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddPlane)
		{
			GLCore::MeshData plane = GLCore::Render::PrimitivesHelper::CreatePlane();

			gameObject = &manager.addEntity();
			gameObject->name = "Plane_" + std::to_string(rendererManager->entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(plane);
			gameObject->getComponent<ECS::MeshFilter>().modelType = PRIMIVITE_PLANE;
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddPointLight)
		{
			gameObject = &manager.addEntity();
			gameObject->addComponent<ECS::PointLight>();
			gameObject->getComponent<ECS::PointLight>().setId(totalPointLight);
			
			totalPointLight += 1;
		}
		else if (action == MainMenuAction::AddSpotLight)
		{
			gameObject = &manager.addEntity();
			gameObject->addComponent<ECS::SpotLight>();
			gameObject->getComponent<ECS::SpotLight>().setId(totalSpotLight);

			totalSpotLight += 1;
		}
		else if (action == MainMenuAction::AddDirectionalLight)
		{
			if (useDirectionalLight == false)
			{
				gameObject = &manager.addEntity();
				gameObject->addComponent<ECS::DirectionalLight>();

				useDirectionalLight = true;
			}
			else
			{
				std::cout << "Ya tienes una luz direccional en la escena" << std::endl;
			}
		}
		else if (action == MainMenuAction::AddCharacterController)
		{
			gameObject = &manager.addEntity();
			gameObject->addComponent<ECS::CharacterController>();
			
			GLCore::MeshData segCube = GLCore::Render::PrimitivesHelper::CreateSegmentedCube(1);

			gameObject->name = "CharacterController_" + std::to_string(rendererManager->entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(segCube);
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddCamera)
		{
			gameObject = &manager.addEntity();
			gameObject->addComponent<ECS::Camera>();
			gameObject->name = "Camera_" + std::to_string(rendererManager->entitiesInScene.size());

			cameras.push_back(&gameObject->getComponent<ECS::Camera>());
		}
		else if (action == MainMenuAction::SaveProject)
		{
			std::cout << "GUARDANDO..." << std::endl;

			SaveSceneToFile("scene.yaml");
		}


		//Autoselect in creating
		if (gameObject != nullptr)
		{
			m_SelectedEntity = gameObject;
		}
	}

	void Scene::getModelPathFromAssets(ImportOptions importOptions)
	{
		
		importOptions.modelID = rendererManager->entitiesInScene.size() + 1;

		loadFileModel(importOptions);
	}
	


	void Scene::SaveSceneToFile(const std::string& filename)
	{
		YAML::Emitter out;

		out << YAML::BeginSeq; // Comienza una secuencia para todas las entidades
		for (const auto& entity : rendererManager->entitiesInScene) {
			if (entity->isActive()) { // Asegúrate de serializar solo entidades activas
				out << entity->serialize(); // Serializa cada entidad y agrega al flujo de salida
			}
		}
		out << YAML::EndSeq; // Finaliza la secuencia

		// Escribe la cadena resultante del Emitter al archivo
		std::ofstream fout(filename);
		fout << out.c_str();
	}


	void Scene::LoadSceneFromFile(const std::string& filename, std::vector<ECS::Entity*>& entitiesInScene, ECS::Manager& manager) {
		std::ifstream fin(filename);
		if (!fin.is_open()) {
			// Manejar error al abrir el archivo
			std::cerr << "Error al abrir el archivo para cargar la escena" << std::endl;
			return;
		}

		YAML::Node data = YAML::Load(fin); // Carga el archivo en un nodo de YAML

		if (!data.IsSequence()) {
			// Manejar error de formato incorrecto
			std::cerr << "Formato de archivo de escena incorrecto" << std::endl;
			return;
		}

		// Limpia las entidades actuales en la escena antes de cargar
		entitiesInScene.clear();

		for (const auto& node : data) {
			// Aquí se asume que tienes una forma de crear entidades y que manager es accesible
			ECS::Entity& newEntity = manager.addEntity();
			newEntity.deserialize(node); // Deserializa la información de cada entidad
			entitiesInScene.push_back(&newEntity); // Añade la entidad al vector de entidades de la escena
		}
	}

	ModelParent Scene::loadFileModel(ImportOptions importOptions)
	{
		ModelParent modelParent = {};

		try {
			modelParent = GLCore::Utils::ModelLoader::LoadModel(importOptions);

			ECS::Entity* entityParent = &manager.addEntity();
			entityParent->name = modelParent.name;

			if (modelParent.modelInfos.size() > 1)
			{
				for (int i = 0; i < modelParent.modelInfos.size(); i++)
				{
					ECS::Entity* entityChild = &manager.addEntity();
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
		}
		catch (std::runtime_error& e) {
			std::cerr << "Error cargando el modelo: " << e.what() << std::endl;
		}
		return modelParent;
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

			if (m_SelectedEntity != nullptr) //Si ya existe un objeto seleccionado y tiene drawable, desactivamos su BB
				if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>()) m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = false;

			m_SelectedEntity = nullptr; //Reset de la variable que almacena la entity seleccioanda preparandola para recibit o no una nueva selección

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
				if (m_SelectedEntity) //Si hemos obtenido un objeto, revisamos si tiene posibilidad de drawable y en ese caso activamos su BB
					if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>()) m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = true;
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
}