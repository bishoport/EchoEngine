#pragma once
#include "../../glpch.h"



namespace GLCore {

    class GuiLayer {
    public:
        GuiLayer(GLFWwindow* window);
        ~GuiLayer();

        void begin();
        void end();
        void renderDockers();
        void renderMainMenuBar();
        void dockersDimensions();

        float width_dock_Inspector = 0;
        float width_dock_AssetScene = 0;

        // Función para establecer el delegado
        void SetDelegate(std::function<void(const MainMenuAction&)> delegado) {
            acctionPresedFromTopMenuDelegate = delegado;
        }



    private:
        bool ini_file_exists;
        ImGuiID dock_id_Inspector;
        ImGuiID dock_id_AssetScene;
        ImGuiID dock_id_toolbar;

        void OnPanelResized(const std::string& panelName, const ImVec2& newSize);

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

