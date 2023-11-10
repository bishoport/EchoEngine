#include "GuiLayer.h"
#include "Application.h"
#include <imGizmo/ImGuizmo.h>
#include "imgui_internal.h"



namespace GLCore {

    GuiLayer::GuiLayer(GLFWwindow* window) {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Enable Docking
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }


        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");


        //--ImGUI Configuration--------------------
        std::ifstream ifile("imgui.ini");
        ini_file_exists = ifile.is_open();
        ifile.close();

        if (ini_file_exists)
            ImGui::LoadIniSettingsFromDisk("imgui.ini");
    }

    GuiLayer::~GuiLayer() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }



    void GuiLayer::begin() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuizmo::BeginFrame();
    }


    void GuiLayer::renderDockers()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        // Estamos usando el flag ImGuiWindowFlags_NoDocking para hacer que la ventana padre no se pueda acoplar,
        // porque sería confuso tener dos objetivos de acoplamiento uno dentro del otro.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        // Importante: notar que procedemos incluso si Begin() retorna falso (es decir, la ventana está colapsada).
        // Esto se debe a que queremos mantener nuestro DockSpace() activo. Si un DockSpace() está inactivo,
        // todas las ventanas activas ancladas en él perderán su padre y se desanclarán.
        // No podemos preservar la relación de anclaje entre una ventana activa y un anclaje inactivo, de lo contrario
        // cualquier cambio de espacio de anclaje/configuración haría que las ventanas quedaran atrapadas en el limbo y nunca fueran visibles.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);


        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);


        static auto first_time = true;

        if (first_time && !ini_file_exists)
        {
            std::cout << "First time: Setup ImGUI" << std::endl;
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetIO().DisplaySize);

            ImGuiID dock_main = dockspace_id;

            inspectorPanel.m_id = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
            assetsPanel.m_id = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
            scenePanel.m_id = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 100.0f, nullptr, &dock_main);
            toolbar.m_id = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Up, 0.065f, nullptr, &dock_main);

            ImGui::DockBuilderDockWindow("Dock_INSPECTOR", inspectorPanel.m_id);
            ImGui::DockBuilderDockWindow("Dock_ASSETS_SCENE", assetsPanel.m_id);
            ImGui::DockBuilderDockWindow("Dock_SCENE", scenePanel.m_id);
            ImGui::DockBuilderDockWindow("Dock_TOOLBAR", toolbar.m_id);

            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    void GuiLayer::renderMainMenuBar() {

        //--TOP MAIN MENU
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("ECHO ENGINE", NULL, false, false);
                //if (ImGui::MenuItem("New")){}
                //if (ImGui::MenuItem("Open", "Ctrl+O")){}
                //if (ImGui::MenuItem("Save", "Ctrl+S")){}
                if (ImGui::MenuItem("Save Scene")) 
                {
                    UsarDelegado(MainMenuAction::SaveProject);
                }

                //ImGui::Separator();

                //if (ImGui::BeginMenu("Options")){
                //    static bool b = true;
                //    ImGui::Checkbox("Auto Save", &b);
                //    ImGui::EndMenu();
                //}

                //if (ImGui::MenuItem("Quit", "Alt+F4")) {}

                ImGui::EndMenu();
            }

            /*if (ImGui::BeginMenu("Edit")) { ImGui::EndMenu(); }
            if (ImGui::BeginMenu("Project")) { ImGui::EndMenu(); }
            if (ImGui::BeginMenu("View")) { ImGui::EndMenu(); }*/

            if (ImGui::BeginMenu("GameObjects"))
            {
            	// Sección de Primitives
            	if (ImGui::BeginMenu("Primitives"))
            	{
            		if (ImGui::MenuItem("Cube"))
            		{
            			UsarDelegado(MainMenuAction::AddCube);
            		}
                    if (ImGui::MenuItem("Segmented Cube"))
                    {
                        UsarDelegado(MainMenuAction::AddSegmentedCube);
                    }
            		if (ImGui::MenuItem("Sphere"))
            		{
            			UsarDelegado(MainMenuAction::AddSphere);
            		}
            		if (ImGui::MenuItem("Quad"))
            		{
            			UsarDelegado(MainMenuAction::AddQuad);
            		}
                    if (ImGui::MenuItem("Plane"))
                    {
                        UsarDelegado(MainMenuAction::AddPlane);
                    }
            		ImGui::EndMenu(); // Fin del menú Primitives
            	}

            	// Sección de Lights
            	if (ImGui::BeginMenu("Lights"))
            	{
            		if (ImGui::MenuItem("Directional Light"))
            		{
            			UsarDelegado(MainMenuAction::AddDirectionalLight);
            		}
            		if (ImGui::MenuItem("Point Light"))
            		{
            			UsarDelegado(MainMenuAction::AddPointLight);
            		}
                    if (ImGui::MenuItem("Spot Light"))
                    {
                        UsarDelegado(MainMenuAction::AddSpotLight);
                    }

            		ImGui::EndMenu(); // Fin del menú Lights
            	}


                // Sección de OTROS
                if (ImGui::BeginMenu("TOOLS"))
                {
                    if (ImGui::MenuItem("Character controller"))
                    {
                        UsarDelegado(MainMenuAction::AddCharacterController);
                    }

                    if (ImGui::MenuItem("Camera"))
                    {
                        UsarDelegado(MainMenuAction::AddCamera);
                    }
                    
                    ImGui::EndMenu(); // Fin del menú OTROS
                }

            	ImGui::EndMenu(); // Fin del menú GameObjects
            }



            //if (ImGui::BeginMenu("Components")) { 

            //    if (ImGui::BeginMenu("Postprocessing"))
            //    {
            //        if (ImGui::MenuItem("Bloom"))
            //        {
            //            UsarDelegado(MainMenuAction::AddBloom);
            //        }
            //        ImGui::EndMenu(); // Fin del menú OTROS
            //    }
            //    ImGui::EndMenu(); 
            //}


            if (ImGui::BeginMenu("About")) { ImGui::EndMenu(); }
            ImGui::EndMenuBar();
        }
        //-----------------------------------------------------------------------------------------------

        ImGui::End();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    //No la estoy usando por ahora pero puede ser util en el futuro
    void GuiLayer::dockersDimensions()
    {
        inspectorPanel.m_id = 0x00000004;
        ImGuiDockNode* node_dock_id_inspector = ImGui::DockBuilderGetNode(inspectorPanel.m_id);
        if (node_dock_id_inspector) {

            ImVec2 size = node_dock_id_inspector->Size;
            ImVec2 position = node_dock_id_inspector->Pos; // Aquí obtenemos la posición

            inspectorPanel.m_posX = position.x;
            inspectorPanel.m_posY = position.y;
            inspectorPanel.m_width = size.x;
            inspectorPanel.m_height = size.y;
        }

        assetsPanel.m_id = 0x00000007;
        ImGuiDockNode* node_dock_id_AssetScene = ImGui::DockBuilderGetNode(assetsPanel.m_id);
        
        if (node_dock_id_AssetScene) {
            ImVec2 size = node_dock_id_AssetScene->Size;
            ImVec2 position = node_dock_id_AssetScene->Pos; // Aquí obtenemos la posición

            assetsPanel.m_posX = position.x;
            assetsPanel.m_posY = position.y;
            assetsPanel.m_width = size.x;
            assetsPanel.m_height = size.y;
        }

        scenePanel.m_id = 0x00000010;
        ImGuiDockNode* node_dock_id_Scene = ImGui::DockBuilderGetNode(scenePanel.m_id);
        if (node_dock_id_Scene)
        {
            ImVec2 size = node_dock_id_Scene->Size;
            ImVec2 position = node_dock_id_Scene->Pos; // Aquí obtenemos la posición

            scenePanel.m_posX = position.x;
            scenePanel.m_posY = position.y;
            scenePanel.m_width = size.x;
            scenePanel.m_height = size.y;
        }
    }
    void GuiLayer::end() {

        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindowWidth(), (float)app.GetWindowHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    
}
