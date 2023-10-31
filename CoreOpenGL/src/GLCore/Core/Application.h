#pragma once

#include "../../glpch.h"
#include "GuiLayer.h"
#include "Scene.h"
#include <memory>
#include "Input/InputManager.h"  // Asegúrate de que la ruta es correcta
#include "Input/keyCodes.h"  // Asegúrate de que la ruta es correcta


namespace GLCore {

    //enum class RenderPasses {
    //    SHADOW_MAPPING,
    //    RENDER_FORWARD
    //};

    //struct RenderPassData {
    //    int x, y, width, height;
    //};

    class Application {
    public:
        Application();
        ~Application();

        bool initialize(const char* title, GLuint width, GLuint height);
        void run();
        void shutdown();
        void processInput();

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        void SetupInputCallbacks();

        static GLuint GetWindowWidth();
        static GLuint GetWindowHeight();

        static GLuint GetViewportWidth();
        static GLuint GetViewportHeight();


        inline GLFWwindow& GetWindow() { return *window; }
        inline static Application& Get() { return *s_Instance; }

    private:
        static Application* s_Instance;

        GLFWwindow* window;
        static GLuint s_WindowWidth;
        static GLuint s_WindowHeight;

        static GLuint viewportWidth;
        static GLuint viewportHeight;


        static std::unique_ptr<Scene> scene;

        static std::unique_ptr<GuiLayer> guiLayer;

        void acctionPresedFromTopMenu(const MainMenuAction& action);

        
    };
}
