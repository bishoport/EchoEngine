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
            dock_id_Inspector = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
            dock_id_AssetScene = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
            dock_id_toolbar = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Up, 0.065f, nullptr, &dock_main);


            ImGui::DockBuilderDockWindow("Dock_INSPECTOR", dock_id_Inspector);
            ImGui::DockBuilderDockWindow("Dock_ASSETS_SCENE", dock_id_AssetScene);
            ImGui::DockBuilderDockWindow("Dock_Top", dock_id_toolbar);

            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    void GuiLayer::renderMainMenuBar() {

        //--TOP MAIN MENU
        if (ImGui::BeginMenuBar())
        {
            /*if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("SANDBOX", NULL, false, false);
                if (ImGui::MenuItem("New")){}
                if (ImGui::MenuItem("Open", "Ctrl+O")){}
                if (ImGui::MenuItem("Save", "Ctrl+S")){}
                if (ImGui::MenuItem("Save As..")) {}

                ImGui::Separator();

                if (ImGui::BeginMenu("Options")){
                    static bool b = true;
                    ImGui::Checkbox("Auto Save", &b);
                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Quit", "Alt+F4")) {}

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) { ImGui::EndMenu(); }
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


    void GuiLayer::dockersDimensions()
    {
        dock_id_Inspector = 0x00000004;
        // Recuperar el nodo de docking usando el ImGuiID
        ImGuiDockNode* node_dock_id_inspector = ImGui::DockBuilderGetNode(dock_id_Inspector);

        if (node_dock_id_inspector) {
            // Ahora puedes obtener la información del nodo de docking
            ImVec2 size = node_dock_id_inspector->Size;
            if (size.x != width_dock_Inspector ) { //|| size.y != height_dock_Inspector
                OnPanelResized("Inspector", size);
                width_dock_Inspector = size.x;
               // height_dock_Inspector = size.y;
            }
        }

        dock_id_AssetScene = 0x00000007;
        // Recuperar el nodo de docking usando el ImGuiID
        ImGuiDockNode* node_dock_id_AssetScene = ImGui::DockBuilderGetNode(dock_id_AssetScene);

        if (node_dock_id_AssetScene) {
            // Ahora puedes obtener la información del nodo de docking
            ImVec2 size = node_dock_id_AssetScene->Size;
            if (size.x != width_dock_AssetScene ) { //|| size.y != height_dock_AssetScene
                OnPanelResized("AssetScene", size);
                width_dock_AssetScene = size.x;
                //height_dock_AssetScene = size.y;
            }
        }
    }


    void GuiLayer::OnPanelResized(const std::string& panelName, const ImVec2& newSize)
    {
        EventManager::getOnPanelResizedEvent().trigger(panelName, newSize);
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
