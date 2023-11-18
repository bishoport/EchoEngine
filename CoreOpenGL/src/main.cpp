#define IMGUI_ENABLE_DOCKING
#define STB_IMAGE_IMPLEMENTATION

#include "GLCore/Core/Application.h"




int main() {
    GLCore::Application app;
    if (!app.initialize("OpenGL App", 1080, 720)) {
        return -1;
    }

    //auto m = mono_image_init("dd");

    app.run();
    app.shutdown();

    return 0;
}

//void processInput(GLFWwindow* window)
//{
//    // Si se presiona la tecla ESCAPE, cierra la ventana.
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // Ajusta el viewport cuando la ventana es redimensionada.
//    glViewport(0, 0, width, height);
//}
