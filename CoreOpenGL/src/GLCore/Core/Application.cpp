#include "Application.h"
#include <iostream>



namespace GLCore {

    GLuint Application::s_WindowWidth = 1920;
    GLuint Application::s_WindowHeight = 1080;

    GLuint Application::viewportWidth = 800;
    GLuint Application::viewportHeight = 600;

    bool  Application::pauseLoop = false;

    Application* Application::s_Instance = nullptr;

    std::unique_ptr<GuiLayer> Application::guiLayer = nullptr;

    std::unique_ptr<Scene> Application::scene = nullptr; 

    Application::Application() : window(nullptr) {
        if (!s_Instance)
        {
            // Initialize core
            Log::Init();
        }
    }

    Application::~Application() {}

   

    bool Application::initialize(const char* title, GLuint width, GLuint height)
    {
        //--INIT GLFW & GLAD
        if (!glfwInit()) {
            return false;
        }

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }
        //--------------------------------------------------------------------------------------------------


        //--GL_CALLBACKS
        SetupInputCallbacks(); //Primero nuestros callbacks antes que los de ImGUI
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        //--------------------------------------------------------------------------------------------------

        //--FLAGS-----------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        //------------------------------------------------------------------------


        //--GUI
        guiLayer = std::make_unique<GuiLayer>(window);
        guiLayer->SetDelegate([this](const MainMenuAction& action) {
            this->acctionPresedFromTopMenu(action);
            });
        //--------------------------------------------------------------------------------------------------



        //--------------------------------------------------------------------------------------------------

        //--DEFAULT SCENE
        Application::scene = std::make_unique<Scene>();
        if (!scene->initialize()) {
            shutdown();
            return false;
        }
        //--------------------------------------------------------------------------------------------------

        InputManager::Instance().subscribe();

        //--Subscriptions events
        // Suscribir una función al evento de redimensionamiento de los paneles de la gui
        EventManager::getOnPanelResizedEvent().subscribe([this](const std::string& panelName, const ImVec2& newSize) 
        {
            viewportWidth = s_WindowWidth - guiLayer->width_dock_Inspector - guiLayer->width_dock_AssetScene;
            viewportHeight = s_WindowHeight;

            //InputManager::Instance().SetViewportSize(viewportWidth, viewportHeight);  // Agregar esta línea
            EventManager::getWindowResizeEvent().trigger(viewportWidth, viewportHeight);
        });

        //--------------------------------------------------------------------------------------------------


        EventManager::getOnEndDeleteEntity().subscribe([this](const bool& result)
        {
                pauseLoop = false;
        });

        return true;
    }

    void Application::acctionPresedFromTopMenu(const MainMenuAction& action)
    {
        if (scene) {
            scene->createGameObject(action);
        }
    }





    void Application::run() {

        Timestep lastFrameTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {

            if (pauseLoop == false)
            {
                //IF PRESS ESC
                if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
                    glfwSetWindowShouldClose(this->window, true);
                }

                //DELTA TIME
                Timestep currentFrameTime = glfwGetTime();
                Timestep deltaTime = currentFrameTime - lastFrameTime;
                lastFrameTime = currentFrameTime;

                //RENDER PASS DATA
                viewportWidth = s_WindowWidth - guiLayer->width_dock_Inspector - guiLayer->width_dock_AssetScene;
                viewportHeight = s_WindowHeight;

                //-------------------------------------------------------------------------------------------------------
                //std::cout << currentRenderPassData.Width << std::endl;

                InputManager::Instance().Update();

                if (scene) {
                    scene->update(deltaTime);
                }

                guiLayer->begin();


                //glViewport(currentRenderPassData.x, currentRenderPassData.y, viewportWidth, viewportHeight);
                //glViewport(currentRenderPassData.X, currentRenderPassData.Y, currentRenderPassData.Width, currentRenderPassData.Height);

                if (scene) {
                    scene->render();
                }

                //--ImGUI
                guiLayer->renderDockers();

                if (scene) {
                    scene->checkGizmo();
                }
                guiLayer->renderMainMenuBar();

                if (scene) {
                    scene->renderGUI();
                }

                guiLayer->dockersDimensions();

                guiLayer->end();
                //------------------------------------------

                // Actualizar el InputManager para el próximo frame
                InputManager::Instance().EndFrame();

                glfwSwapBuffers(window);
                glfwPollEvents();
            }


            
        }
        glfwTerminate();
    }

    void Application::processInput() {
        
    }

    GLuint Application::GetWindowWidth() {
        return s_WindowWidth;
    }

    GLuint Application::GetWindowHeight() {
        return s_WindowHeight;
    }

    GLuint Application::GetViewportWidth()
    {
        return viewportWidth;
    }

    GLuint Application::GetViewportHeight()
    {
        return viewportHeight;
    }

    void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {

        s_WindowWidth = width;
        s_WindowHeight = height;

        viewportWidth = s_WindowWidth - guiLayer->width_dock_Inspector - guiLayer->width_dock_AssetScene;
        viewportHeight = s_WindowHeight;

        //InputManager::Instance().SetViewportSize(viewportWidth, viewportHeight);  // Agregar esta línea

        EventManager::getWindowResizeEvent().trigger(viewportWidth, viewportHeight);
    }

    void Application::shutdown() {

        glfwDestroyWindow(window);
        glfwTerminate();
    }


    void Application::SetupInputCallbacks() 
    {
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            // Primeramente, permitir que ImGui procese la entrada
            if (ImGui::GetIO().WantCaptureKeyboard) {
                return;
            }

            if (action == GLFW_PRESS) {
                InputManager::Instance().SetKeyPressed(key, true);
            }
            else if (action == GLFW_RELEASE) {
                InputManager::Instance().SetKeyPressed(key, false);
            }
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            
            if (action == GLFW_PRESS) {
                InputManager::Instance().SetMousePressed(button, x, y, true);
            }
            else if (action == GLFW_RELEASE) {
                InputManager::Instance().SetMousePressed(button, x, y, false);
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            InputManager::Instance().SetMousePosition(xpos, ypos);
        });
    }
}
