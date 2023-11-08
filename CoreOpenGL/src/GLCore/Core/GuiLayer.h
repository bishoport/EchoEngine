#pragma once
#include "../../glpch.h"



namespace GLCore {

    class GuiLayer {
    public:

        struct dockPanel
        {
            ImGuiID m_id;
            int m_posX;
            int m_posY;
            int m_width;
            int m_height;
            dockPanel(ImGuiID id, int posX, int posY, int width, int height) : m_id(id), m_posX(posX), m_posY(posY), m_width(width), m_height(height) {}
        };
        GuiLayer(GLFWwindow* window);
        ~GuiLayer();

        void begin();
        void end();
        void renderDockers();
        void renderMainMenuBar();
        void dockersDimensions();

        dockPanel inspectorPanel = dockPanel(0, 0, 0, 0, 0);
        dockPanel assetsPanel = dockPanel(0, 0, 0, 0, 0);
        dockPanel toolbar = dockPanel(0, 0, 0, 0, 0);
        dockPanel scenePanel = dockPanel(0, 0, 0, 0, 0);

        // Función para establecer el delegado
        void SetDelegate(std::function<void(const MainMenuAction&)> delegado) {
            acctionPresedFromTopMenuDelegate = delegado;
        }

    private:
        bool ini_file_exists;

        //callbacks:
        std::function<void(const MainMenuAction&)> acctionPresedFromTopMenuDelegate;

        // Función que utiliza el delegado
        void UsarDelegado(const MainMenuAction& action) {
            if (acctionPresedFromTopMenuDelegate) {  // Verifica que el delegado haya sido establecido
                acctionPresedFromTopMenuDelegate(action);
            }
        }
    };
}

