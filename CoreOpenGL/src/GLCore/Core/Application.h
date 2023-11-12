#pragma once

#include "../../glpch.h"

#include "GuiLayer.h"
#include "Scene.h"
#include "GameObjectManager.h"

#include "Input/InputManager.h"  // Asegúrate de que la ruta es correcta
#include "Input/keyCodes.h"  // Asegúrate de que la ruta es correcta

namespace GLCore {

    class Application {
    public:
        Application();
        ~Application();

        static bool pauseLoop;

        bool initialize(const char* title, GLuint width, GLuint height);
        void run();
        void shutdown();
        void processInput();

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        void SetupInputCallbacks();

        static GLuint GetWindowWidth();
        static GLuint GetWindowHeight();

        inline GLFWwindow& GetWindow() { return *window; }
        inline static Application& Get() { return *s_Instance; }

        static std::unique_ptr<GameObjectManager> gameObjectManager;

    private:
        static Application* s_Instance;

        GLFWwindow* window;

        static GLuint s_WindowWidth;
        static GLuint s_WindowHeight;

        static std::unique_ptr<Scene> scene;
        static std::unique_ptr<GuiLayer> guiLayer;
        

        void acctionPresedFromTopMenu(const MainMenuAction& action); 
    };
}
