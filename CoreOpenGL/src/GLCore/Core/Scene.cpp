#include "Scene.h"
#include <iostream>
#include "Application.h"
#include "../Render/ShaderManager.h"
#include <imGizmo/ImGuizmo.h>
#include "../../ECS/SpotLight.h"
#include "../../ECS/DirectionalLight.h"

#include "../Util/ModelLoader.h"
#include "../../ECS/CharacterController.h"
#include "../../ECS/Bloom.h"




namespace GLCore {

	std::pair<glm::vec3, float> Scene::SceneBounds = { glm::vec3(0.0f), 0.0f };

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
		GLCore::Render::ShaderManager::Load("pbr_ibl", "assets/shaders/Default.vert", "assets/shaders/pbr_ibl.fs");
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

		GLCore::Render::ShaderManager::Load("hdr", "assets/shaders/postpro/hdr.vs", "assets/shaders/postpro/hdr.fs");
		GLCore::Render::ShaderManager::Load("main_output_FBO", "assets/shaders/main_output_FBO.vs", "assets/shaders/main_output_FBO.fs");

		//--IBL
		GLCore::Render::ShaderManager::Load("equirectangularToCubemap", 
										    "assets/shaders/IBL/cubemap.vs", 
											"assets/shaders/IBL/equirectangular_to_cubemap.fs");

		GLCore::Render::ShaderManager::Load("irradiance",
											"assets/shaders/IBL/cubemap.vs",
											"assets/shaders/IBL/irradiance_convolution.fs");

		GLCore::Render::ShaderManager::Load("prefilter",
											"assets/shaders/IBL/cubemap.vs",
											"assets/shaders/IBL/prefilter.fs");

		GLCore::Render::ShaderManager::Load("brdf",
											"assets/shaders/IBL/brdf.vs",
											"assets/shaders/IBL/brdf.fs");

		GLCore::Render::ShaderManager::Load("background",
											"assets/shaders/IBL/background.vs",
											"assets/shaders/IBL/background.fs");
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

		


		//--MAIN FBO
		mainColorBuffers = GLCore::Render::FBOManager::CreateFBO_Color_RGBA16F(&mainFBO, &mainRboDepth, 3, 800, 600);
		EventManager::getWindowResizeEvent().subscribe([this](GLuint width, GLuint height) {
			GLCore::Render::FBOManager::UpdateFBO_Color_RGBA16F(&mainFBO, &mainRboDepth, mainColorBuffers, width, height);
		});
		// ---------------------------------------
		


		
		//--IBL
		hdrTexture_daylight = GLCore::Utils::ImageLoader::loadHDR("assets/default/HDR/newport_loft.hdr");
		prepare_PBR_IBL();
		//--------------------------------------------------------------------------------------------------------------------------------

		postprocessGameObject = &manager.addEntity();
		postprocessGameObject->name = "Postprocessing";
		//postproManager = new Utils::PostProcessingManager(800, 600);


		assetsPanel.SetDelegate([this](ImportOptions importOptions) {
			this->getModelPathFromAssets(importOptions);
		});


        return true;
    }



    void Scene::update(Timestep deltaTime)
    {

		//--INPUTS
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
        entitiesInScene = manager.getAllEntities();
		if (entitiesInScene.size() > 0)
		{
			CalcSceneBundle();
		}
		
		//----------------------------------------------------------------------------------------------------------------------------

        //--CAMERA
        aspectRatio = static_cast<float>(GLCore::Application::GetViewportWidth()) / static_cast<float>(GLCore::Application::GetViewportHeight());
        m_EditorCamera.GetCamera().SetProjection(m_EditorCamera.GetCamera().GetFov(), aspectRatio, 0.1f, 100.0f);
        m_EditorCamera.OnUpdate(deltaTime);
        //----------------------------------------------------------------------------------------------------------------------------
    }




    void Scene::render()
    {
		//-GENERICS TO ALL SHADER
		for (const auto& [name, shader] : GLCore::Render::ShaderManager::GetAllShaders())
		{
			GLCore::Render::ShaderManager::Get(name.c_str())->use();

			GLCore::Render::ShaderManager::Get(name.c_str())->setBool("useDirLight", useDirectionalLight);
			GLCore::Render::ShaderManager::Get(name.c_str())->setInt("numPointLights", totalPointLight);
			GLCore::Render::ShaderManager::Get(name.c_str())->setInt("numSpotLights", totalSpotLight);


			GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("globalAmbient", globalAmbient);

			GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("projection", m_EditorCamera.GetCamera().GetProjectionMatrix());
			GLCore::Render::ShaderManager::Get(name.c_str())->setMat4("view", m_EditorCamera.GetCamera().GetViewMatrix());
			GLCore::Render::ShaderManager::Get(name.c_str())->setVec3("viewPos", m_EditorCamera.GetCamera().GetPosition());
		}


		if (useHDRIlumination == true)
		{
			//--------------------------------------------IBL
			//glDepthFunc(GL_LEQUAL);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);  // display prefilter map

			GLCore::Render::ShaderManager::Get("pbr_ibl")->use();
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("irradianceMap", 0);
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("prefilterMap", 1);
			GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("brdfLUT", 2);
			//----------------------------------------------------------------------------------------------------
		}
		//else
		//{
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // display irradiance map
		//	glActiveTexture(GL_TEXTURE2);
		//	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);  // display prefilter map
		//}

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

		//	//-POINTS LIGHTS
		//	if (entitiesInScene[i]->hascomponent<ECS::PointLight>())
		//	{
		//		//if (entitiesInScene[i]->getComponent<ECS::PointLight>().drawShadows)
		//		//{
		//		//	entitiesInScene[i]->getComponent<ECS::PointLight>().shadowMappingProjection(entitiesInScene);
		//		//}
		//	}	
		}
		//----------------------------------------------------------------------------------------------------------------


		
		//-------------------------------------------------------------------------------------------------------------------------------------------





		
		if (useStandardFBO)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			glViewport(0, 0, Application::GetViewportWidth(), Application::GetViewportHeight());
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (useHDRIlumination == true)
			{
				glm::mat4 viewHDR = glm::mat4(glm::mat3(m_EditorCamera.GetCamera().GetViewMatrix()));
				// Escala la matriz de vista para hacer el skybox más grande
				float scale = 1.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
				viewHDR = glm::scale(viewHDR, glm::vec3(scale, scale, scale));
				GLCore::Render::ShaderManager::Get("background")->use();
				GLCore::Render::ShaderManager::Get("background")->setMat4("view", viewHDR);
				GLCore::Render::ShaderManager::Get("background")->setMat4("projection", m_EditorCamera.GetCamera().GetProjectionMatrix());
				GLCore::Render::ShaderManager::Get("background")->setInt("environmentMap", 0);
				renderCube();
			}

			drawAllEntities();
		}
		else
		{
			glDepthFunc(GL_LESS);
			//--PREPARE MAIN_FBO
			glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);


			glViewport(0, 0, Application::GetViewportWidth(), Application::GetViewportHeight());
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			if (useHDRIlumination == true)
			{
				//--------------------------------------------IBL
				glDepthFunc(GL_LEQUAL);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);  // display prefilter map

				GLCore::Render::ShaderManager::Get("pbr_ibl")->use();
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("irradianceMap", 0);
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("prefilterMap", 1);
				GLCore::Render::ShaderManager::Get("pbr_ibl")->setInt("brdfLUT", 2);


				glm::mat4 viewHDR = glm::mat4(glm::mat3(m_EditorCamera.GetCamera().GetViewMatrix()));
				// Escala la matriz de vista para hacer el skybox más grande
				float scale = 1.0f; // Ajusta este valor para obtener el tamaño deseado para tu skybox
				viewHDR = glm::scale(viewHDR, glm::vec3(scale, scale, scale));
				GLCore::Render::ShaderManager::Get("background")->use();
				GLCore::Render::ShaderManager::Get("background")->setMat4("view", viewHDR);
				GLCore::Render::ShaderManager::Get("background")->setMat4("projection", m_EditorCamera.GetCamera().GetProjectionMatrix());
				GLCore::Render::ShaderManager::Get("background")->setInt("environmentMap", 0);
				renderCube();
				//----------------------------------------------------------------------------------------------------
			}
			
			drawAllEntities();
			//-------------------------------------------------------------------------------------------------------------------------------------------

			

			//--DRAW MAIN_FBO in QUAD
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			glViewport(0, 0, Application::GetViewportWidth(), Application::GetViewportHeight());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			GLCore::Render::ShaderManager::Get("main_output_FBO")->use();
			for (size_t i = 0; i < mainColorBuffers.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, mainColorBuffers[i]);
				std::string uniformName = "colorBuffer_" + std::to_string(i);
				GLCore::Render::ShaderManager::Get("main_output_FBO")->setInt(uniformName.c_str(), i);
			}

			renderQuad();
			//-------------------------------------------------------------------------------------------------------------------------------------------
		}

		



		//-ACTIVE SELECTED ENTITY
		if (m_SelectedEntity != nullptr && m_SelectedEntity->hascomponent<ECS::MeshRenderer>())
		{
			m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = true;
		}
		CheckIfPointerIsOverObject();
		//-------------------------------------------------------------------------------------------------------------------------------------------


		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
		}



		//-RESET
		//if (usePostprocessing == true)
		//{
		//	postProcessingManager->RenderHDR();
		//	drawAllEntities();

		//	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		//	glViewport(0, 0, Application::GetViewportWidth(), Application::GetViewportHeight());
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//	
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, postProcessingManager->colorBuffer);

		//	GLCore::Render::ShaderManager::Get("hdr")->use();
		//	GLCore::Render::ShaderManager::Get("hdr")->setInt("hdr", usePostprocessing);
		//	GLCore::Render::ShaderManager::Get("hdr")->setInt("hdrBuffer", 0);
		//	GLCore::Render::ShaderManager::Get("hdr")->setFloat("exposure", exposure);
		//	GLCore::Render::ShaderManager::Get("hdr")->setFloat("gamma", gamma);
		//	renderQuad();
		//}
		//else
		//{
		//	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		//	glViewport(0, 0, Application::GetViewportWidth(), Application::GetViewportHeight());
		//	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//	Application::currentRenderPass = GLCore::Render::RenderPasses::RENDER_FORWARD;
		//	drawAllEntities();
		//}

		//----------------------------------------------------------------------------------------------------------------

		
    }

	void Scene::drawAllEntities()
	{
		//-OTHERS
		//if (skybox->isActive)
		//{
		//	skybox->Render(m_EditorCamera.GetCamera().GetViewMatrix(), m_EditorCamera.GetCamera().GetProjectionMatrix());
		//}
		gridWorldRef->Render();
		//----------------------------------------------------------------------------------------------------------------

		//-ENTITIES CALL DRAW()
		for (int i = 0; i < entitiesInScene.size(); i++)
		{
			//-MATERIALS
			if (entitiesInScene[i]->hascomponent<ECS::Material>()) // Only root entities
			{
				entitiesInScene[i]->getComponent<ECS::Material>().draw();
			}
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
		//manager.draw();
	}

	void Scene::prepare_PBR_IBL()
	{
		glGenFramebuffers(1, &IBL_FBO);
		glGenRenderbuffers(1, &IBL_RBO);

		glBindFramebuffer(GL_FRAMEBUFFER, IBL_FBO);
		glBindRenderbuffer(GL_RENDERBUFFER, IBL_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, IBL_RBO);


		// pbr: setup cubemap to render to and attach to framebuffer
		// ---------------------------------------------------------
		glGenTextures(1, &envCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



		// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
		// ----------------------------------------------------------------------------------------------
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};




		// pbr: convert HDR equirectangular environment map to cubemap equivalent
		// ----------------------------------------------------------------------
		GLCore::Render::ShaderManager::Get("equirectangularToCubemap")->use();
		GLCore::Render::ShaderManager::Get("equirectangularToCubemap")->setInt("equirectangularDayLightMap", 0);
		GLCore::Render::ShaderManager::Get("equirectangularToCubemap")->setFloat("mixFactor", mixFactor);
		GLCore::Render::ShaderManager::Get("equirectangularToCubemap")->setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture_daylight);

		glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, IBL_FBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			GLCore::Render::ShaderManager::Get("equirectangularToCubemap")->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);







		// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
		// --------------------------------------------------------------------------------
		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, IBL_FBO);
		glBindRenderbuffer(GL_RENDERBUFFER, IBL_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);


		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		// -----------------------------------------------------------------------------
		GLCore::Render::ShaderManager::Get("irradiance")->use();
		GLCore::Render::ShaderManager::Get("irradiance")->setInt("environmentMap", 0);
		GLCore::Render::ShaderManager::Get("irradiance")->setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, IBL_FBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			GLCore::Render::ShaderManager::Get("irradiance")->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);



		// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
		// --------------------------------------------------------------------------------
		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);




		// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
		// ----------------------------------------------------------------------------------------------------
		GLCore::Render::ShaderManager::Get("prefilter")->use();
		GLCore::Render::ShaderManager::Get("prefilter")->setInt("environmentMap", 0);
		GLCore::Render::ShaderManager::Get("prefilter")->setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glBindFramebuffer(GL_FRAMEBUFFER, IBL_FBO);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			glBindRenderbuffer(GL_RENDERBUFFER, IBL_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			GLCore::Render::ShaderManager::Get("prefilter")->setFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				GLCore::Render::ShaderManager::Get("prefilter")->setMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderCube();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);



		// pbr: generate a 2D LUT from the BRDF equations used.
		// ----------------------------------------------------
		glGenTextures(1, &brdfLUTTexture);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, Application::GetViewportWidth(), Application::GetViewportHeight(), 0, GL_RG, GL_FLOAT, 0);
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
		glBindFramebuffer(GL_FRAMEBUFFER, IBL_FBO);
		glBindRenderbuffer(GL_RENDERBUFFER, IBL_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Application::GetViewportWidth(), Application::GetViewportHeight());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

		glViewport(0, 0, Application::GetViewportWidth(), Application::GetViewportHeight());
		GLCore::Render::ShaderManager::Get("brdf")->use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// renderQuad() renders a 1x1 XY quad in NDC
	// -----------------------------------------
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

	void Scene::checkGizmo()
	{
		//---------------------------ImGUIZMO------------------------------------------
		if (m_SelectedEntity != nullptr)
		{
			if (m_SelectedEntity->hascomponent<ECS::Transform>())
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(0, 0, Application::GetViewportWidth(), Application::GetViewportHeight());

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
		for (int i = 0; i < entitiesInScene.size(); i++)
		{
			if (entitiesInScene[i]->getComponent<ECS::Transform>().parent == nullptr) // Only root entities
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				if (m_SelectedEntity == entitiesInScene[i])
					node_flags |= ImGuiTreeNodeFlags_Selected;

				std::string treeLabel = entitiesInScene[i]->name;

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
					m_SelectedEntity = entitiesInScene[i];
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
						auto& sourceEntity = entitiesInScene[sourceEntityIndex]; // Entidad que estás arrastrando
						auto& targetEntity = entitiesInScene[i]; // Entidad sobre la cual se hace el "drop"

						sourceEntity->getComponent<ECS::Transform>().parent = targetEntity;
						targetEntity->getComponent<ECS::Transform>().children.push_back(sourceEntity);

						//targetEntity->getComponent<ECS::Transform>().addChild(sourceEntity);
					}
					ImGui::EndDragDropTarget();
				}
				//--END DRAG_DROP



				if (treeOpen)
				{
					for (int j = 0; j < entitiesInScene[i]->getComponent<ECS::Transform>().children.size(); j++)
					{
						auto& child = entitiesInScene[i]->getComponent<ECS::Transform>().children[j];

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

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Checkbox("Default FBO", &useStandardFBO);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Checkbox("HDR Ilumination", &useHDRIlumination);
			if (useHDRIlumination == true)
			{
				ImGui::SliderFloat("HDR EXPOSURE", &exposure, 0.0f, 10.0f, "%.5f");
				ImGui::SliderFloat("HDR GAMMA", &gamma, 0.0f, 3.0f, "%.5f");
			}
			ImGui::Dummy(ImVec2(0.0f, 5.0f));


			ImGui::ColorEdit3("Clear color", (float*)&clearColor);
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			ImGui::ColorEdit3("Global Ambient", &globalAmbient[0]);
			ImGui::Dummy(ImVec2(0.0f, 5.0f));


			for (size_t i = 0; i < mainColorBuffers.size(); i++)
			{
				ImGui::Image((void*)(intptr_t)mainColorBuffers[i], ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
			}


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
			for (const auto& component : comps) {
				component->drawGUI_Inspector();
			}
		}
		ImGui::End();
		//------------------------------------------------------------------------------------------------




		//-------------------------------------------ASSETS PANEL--------------------------------------
		assetsPanel.OnImGuiRender();
		//------------------------------------------------------------------------------------------------




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
    }

	void Scene::createGameObject(MainMenuAction action)
	{
		ECS::Entity* gameObject = nullptr;

		if (action == MainMenuAction::AddCube)
		{
			GLCore::MeshData cube = GLCore::Render::PrimitivesHelper::CreateCube();

			gameObject = &manager.addEntity();
			gameObject->name = "Cube_" + std::to_string(entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(cube);
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddSegmentedCube)
		{
			GLCore::MeshData segCube = GLCore::Render::PrimitivesHelper::CreateSegmentedCube(1);

			gameObject = &manager.addEntity();
			gameObject->name = "SegCube_" + std::to_string(entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(segCube);
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddSphere)
		{
			GLCore::MeshData sphere = GLCore::Render::PrimitivesHelper::CreateSphere(1, 20, 20);

			gameObject = &manager.addEntity();
			gameObject->name = "Sphere_" + std::to_string(entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(sphere);
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddQuad)
		{
			GLCore::MeshData quad = GLCore::Render::PrimitivesHelper::CreateQuad();

			gameObject = &manager.addEntity();
			gameObject->name = "Quad_" + std::to_string(entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(quad);
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddPlane)
		{
			GLCore::MeshData plane = GLCore::Render::PrimitivesHelper::CreatePlane();

			gameObject = &manager.addEntity();
			gameObject->name = "Plane_" + std::to_string(entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(plane);
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

			gameObject->name = "SegCube_" + std::to_string(entitiesInScene.size());
			gameObject->addComponent<ECS::MeshFilter>().initMesh(segCube);
			gameObject->addComponent<ECS::MeshRenderer>();
			gameObject->addComponent<ECS::Material>();
			gameObject->getComponent<ECS::Material>().setDafaultMaterial();
		}
		else if (action == MainMenuAction::AddBloom)
		{

			postprocessGameObject->addComponent<ECS::Bloom>().prepare(Application::GetViewportWidth(),
																	  Application::GetViewportHeight());
		}



		

		//Autoselect in creating
		if (gameObject != nullptr)
		{
			m_SelectedEntity = gameObject;
		}
	}

	void Scene::getModelPathFromAssets(ImportOptions importOptions)
	{
		
		importOptions.modelID = entitiesInScene.size() + 1;

		loadFileModel(importOptions);
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
					entityChild->addComponent<ECS::MeshRenderer>();
					entityChild->addComponent<ECS::Material>().setMaterial(modelParent.modelInfos[i].model_material);
	
					entityChild->getComponent<ECS::Transform>().parent = entityParent;
					entityParent->getComponent<ECS::Transform>().children.push_back(entityChild);
				}
			}
			else
			{
				entityParent->addComponent<ECS::MeshFilter>().initMesh(modelParent.modelInfos[0].meshData);
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

		if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
		{
			return;
		}

		float viewportX = 0.0f;
		float viewportY = 0.0f;
		float viewportWidth = Application::GetViewportWidth();
		float viewportHeight = Application::GetViewportHeight();

		bool mouseInViewport = false;

		if (mouseX >= viewportX && mouseX <= viewportX + viewportWidth &&
			mouseY >= viewportY && mouseY <= viewportY + viewportHeight)
		{
			mouseInViewport = true;
		}
		else
		{
			mouseInViewport = false;
		}

		if (selectingEntity == true)
		{
			return;
		}

		selectingEntity = false;


		if (InputManager::Instance().IsMouseButtonJustReleased(GLFW_MOUSE_BUTTON_LEFT) && mouseInViewport)
		{
			pickingObj = false;
		}

		// Aquí empieza el raycasting
		if (InputManager::Instance().IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && mouseInViewport)
		{
			if (pickingObj) return; //Si esta bool está a true, retornará, y significa que hemos pulsado ya el mouse y hasta que no soltemos el boton, no se devuelve a false

			if (m_SelectedEntity != nullptr) //Si ya existe un objeto seleccionado y tiene drawable, desactivamos su BB
				if (m_SelectedEntity->hascomponent<ECS::MeshRenderer>()) m_SelectedEntity->getComponent<ECS::MeshRenderer>().drawLocalBB = false;

			m_SelectedEntity = nullptr; //Reset de la variable que almacena la entity seleccioanda preparandola para recibit o no una nueva selección

			//llevamos un punto 2D a un espacio 3D (mouse position -> escene)
			float normalizedX = (2.0f * mouseX) / Application::GetViewportWidth() - 1.0f;
			float normalizedY = ((2.0f * mouseY) / Application::GetViewportHeight() - 1.0f) * -1.0f;
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