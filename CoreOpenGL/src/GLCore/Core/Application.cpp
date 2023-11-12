#include "Application.h"
#include <iostream>



namespace GLCore {

    GLuint Application::s_WindowWidth = 1920;
    GLuint Application::s_WindowHeight = 1080;

    bool  Application::pauseLoop = false;

    Application* Application::s_Instance = nullptr;

    std::unique_ptr<GuiLayer> Application::guiLayer = nullptr;
    std::unique_ptr<Scene> Application::scene = nullptr; 
    std::unique_ptr<GameObjectManager> Application::gameObjectManager = nullptr;

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

        //--GAMEOBJECT MANAGER
        Application::gameObjectManager = std::make_unique<GameObjectManager>();
        //--------------------------------------------------------------------------------------------------

        //--DEFAULT SCENE
        Application::scene = std::make_unique<Scene>();
        if (!scene->initialize()) {
            shutdown();
            return false;
        }
        //--------------------------------------------------------------------------------------------------

        


        //--SUBSCRIPCION A EVENTOS
        InputManager::Instance().subscribe();

        EventManager::getOnEndDeleteEntity().subscribe([this](const bool& result)
        {
            pauseLoop = false;
        });
        //--------------------------------------------------------------------------------------------------

        return true;
    }

    void Application::acctionPresedFromTopMenu(const MainMenuAction& action)
    {
        gameObjectManager->createGameObject(action);
    }



    void Application::run() {

        Timestep lastFrameTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {

            //IF PRESS ESC
            if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_ESCAPE)) {
                glfwSetWindowShouldClose(this->window, true);
            }

            //DELTA TIME
            Timestep currentFrameTime = glfwGetTime();
            Timestep deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            InputManager::Instance().Update();

            if (scene) {
                scene->update(deltaTime);
                scene->render();
            }

            //--ImGUI
            guiLayer->begin();
            guiLayer->renderDockers();
            guiLayer->renderMainMenuBar();

            if (scene) {
                scene->renderGUI();
            }
            guiLayer->end();
            //------------------------------------------


            // Actualizar el InputManager para el próximo frame
            InputManager::Instance().EndFrame();

            glfwSwapBuffers(window);
            glfwPollEvents();
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

    void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) 
    {
        s_WindowWidth = width;
        s_WindowHeight = height;
        EventManager::getWindowResizeEvent().trigger(s_WindowWidth, s_WindowHeight);
    }

    void Application::shutdown() {

        glfwDestroyWindow(window);
        glfwTerminate();
    }


    void Application::SetupInputCallbacks() 
    {
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            // Primeramente, permitir que ImGui procese la entrada
            /*if (ImGui::GetIO().WantCaptureKeyboard) {
                return;
            }*/

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
