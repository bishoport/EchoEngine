#include "ShaderManager.h"

#include "../../glpch.h"

#include <filesystem>
#include <fstream>
#include <sys/stat.h>

namespace fs = std::filesystem;

namespace GLCore::Render 
{

    std::vector<GLCore::Render::ShaderManager::ShaderProgramSource> ShaderManager::shaderProgramSources;
    std::unordered_map<std::string, Shader*> ShaderManager::compiledShaders;


    TextEditor ShaderManager::vertexEditor;
    TextEditor ShaderManager::fragmentEditor;
    TextEditor ShaderManager::geometryEditor;

    void ShaderManager::LoadAllShaders()
    {
        ShaderManager::shaderProgramSources.clear();

        //--LOAD SHADERS
        // Llamadas para cargar y almacenar las fuentes de los programas de shader
        ShaderManager::shaderProgramSources.emplace_back("pbr", "assets/shaders/Default.vert", "assets/shaders/pbr.fs");
        ShaderManager::shaderProgramSources.emplace_back("pbr_ibl", "assets/shaders/Default.vert", "assets/shaders/pbr_ibl.fs");

        ShaderManager::shaderProgramSources.emplace_back("debug", "assets/shaders/Debug.vert", "assets/shaders/Debug.frag");
        ShaderManager::shaderProgramSources.emplace_back("skybox", "assets/shaders/skybox/skybox.vs", "assets/shaders/skybox/skybox.fs");
        ShaderManager::shaderProgramSources.emplace_back("dynamicSkybox", "assets/shaders/skybox/dynamicSkybox.vs", "assets/shaders/skybox/dynamicSkybox.fs");

        ShaderManager::shaderProgramSources.emplace_back("direct_light_depth_shadows", "assets/shaders/shadows/directLight_shadow_mapping_depth_shader.vs", "assets/shaders/shadows/directLight_shadow_mapping_depth_shader.fs");
        ShaderManager::shaderProgramSources.emplace_back("spotLight_depth_shadows", "assets/shaders/shadows/spotLight_shadow_map_depth.vs", "assets/shaders/shadows/spotLight_shadow_map_depth.fs");
        ShaderManager::shaderProgramSources.emplace_back("pointLight_depth_shadows", "assets/shaders/shadows/pointLight_shadow_mapping_depth_shader.vs", "assets/shaders/shadows/pointLight_shadow_mapping_depth_shader.fs", "assets/shaders/shadows/pointLight_shadow_mapping_depth_shader.gs");

        ShaderManager::shaderProgramSources.emplace_back("postprocessing", "assets/shaders/postpro/postprocessing.vs", "assets/shaders/postpro/postprocessing.fs");
        ShaderManager::shaderProgramSources.emplace_back("main_output_FBO", "assets/shaders/main_output_FBO.vs", "assets/shaders/main_output_FBO.fs");

        // IBL shaders
        ShaderManager::shaderProgramSources.emplace_back("equirectangularToCubemap", "assets/shaders/IBL/cubemap.vs", "assets/shaders/IBL/equirectangular_to_cubemap.fs");
        ShaderManager::shaderProgramSources.emplace_back("irradiance", "assets/shaders/IBL/cubemap.vs", "assets/shaders/IBL/irradiance_convolution.fs");
        ShaderManager::shaderProgramSources.emplace_back("prefilter", "assets/shaders/IBL/cubemap.vs", "assets/shaders/IBL/prefilter.fs");
        ShaderManager::shaderProgramSources.emplace_back("brdf", "assets/shaders/IBL/brdf.vs", "assets/shaders/IBL/brdf.fs");
        ShaderManager::shaderProgramSources.emplace_back("background", "assets/shaders/IBL/background.vs", "assets/shaders/IBL/background.fs");


        //CARGAMOS LOS DATOS
        for (const auto& shaderSource : shaderProgramSources) {

            if (shaderSource.geometryDataSource.sourcePath.c_str() == "")
            {
                ShaderManager::Load(shaderSource.name.c_str(), shaderSource.vertexDataSource.sourcePath.c_str(), shaderSource.fragmentDataSource.sourcePath.c_str());
            }
            else
            {
                ShaderManager::Load(shaderSource.name.c_str(), shaderSource.vertexDataSource.sourcePath.c_str(), shaderSource.fragmentDataSource.sourcePath.c_str(), shaderSource.geometryDataSource.sourcePath.c_str());
            }
        }
        //----------------------------------------------------------------------------------------------------------------------------


        //--Capturamos el codigo de los archivos para usarlo en el editor
        for (int i = 0; i < shaderProgramSources.size(); i++)
        {
            shaderProgramSources[i].vertexDataSource.currentCode   = readFile(shaderProgramSources[i].vertexDataSource.sourcePath);
            shaderProgramSources[i].fragmentDataSource.currentCode = readFile(shaderProgramSources[i].fragmentDataSource.sourcePath);

            if (shaderProgramSources[i].geometryDataSource.sourcePath != "")
            {
                shaderProgramSources[i].geometryDataSource.currentCode = readFile(shaderProgramSources[i].geometryDataSource.sourcePath);
            }
        }

        ShaderManager::InitializeShaderEditors();
    }

    void ShaderManager::InitializeShaderEditors() {
        // Configura aqu� los editores
        TextEditor::LanguageDefinition glsl = TextEditor::LanguageDefinition::GLSL();
        vertexEditor.SetLanguageDefinition(glsl);
        fragmentEditor.SetLanguageDefinition(glsl);
        geometryEditor.SetLanguageDefinition(glsl);

       
    }



    
    void ShaderManager::Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
        Shader* shader = new Shader(vertexPath.c_str(), fragmentPath.c_str(), geometryPath.empty() ? nullptr : geometryPath.c_str());
        compiledShaders[name] = shader;
    }

    Shader* ShaderManager::Get(const std::string& name) {
        if (compiledShaders.find(name) != compiledShaders.end()) {
            return compiledShaders[name];
        }
        return nullptr;
    }

    void ShaderManager::CleanUp() {
        for (auto& [name, shader] : compiledShaders) {
            delete shader;
        }
        compiledShaders.clear();
    }

    std::string cleanShaderCode(const std::string& code) {
        // Elimina caracteres no imprimibles y posiblemente otros caracteres no deseados
        std::string cleanedCode;
        cleanedCode.reserve(code.size());

        for (char c : code) {
            if (std::isprint(static_cast<unsigned char>(c)) || c == '\n' || c == '\t') {
                cleanedCode.push_back(c);
            }
        }

        return cleanedCode;
    }

    bool createDirectory(const std::string& path) {
        #if defined(_WIN32)
            int status = _mkdir(path.c_str());
        #else
            int status = mkdir(path.c_str(), 0777);
        #endif
        return status == 0 || errno == EEXIST;
    }

    void saveShaderCode(const std::string& filepath, const std::string& code) {
        std::ofstream out(filepath);
        if (out.is_open()) {
            out << code;
            out.close();
        }
        else {
            std::cerr << "Unable to open file for writing: " << filepath << std::endl;
        }
    }

    void ShaderManager::ReloadAllShaders() {
        // Limpia los shaders compilados anteriores para evitar fugas de memoria
        CleanUp();

        // Crea la carpeta para guardar los shaders, si no existe
        std::string savedShadersPath = "savedShaders";
        if (!createDirectory(savedShadersPath)) {
            std::cerr << "Failed to create directory: " << savedShadersPath << std::endl;
            return;
        }

        for (auto& shaderSource : shaderProgramSources) {
            // Limpia el c�digo del shader antes de usarlo
            std::string vertexCode = cleanShaderCode(shaderSource.vertexDataSource.currentCode);
            std::string fragmentCode = cleanShaderCode(shaderSource.fragmentDataSource.currentCode);
            std::string geometryCode = shaderSource.geometryDataSource.currentCode.empty() ? "" : cleanShaderCode(shaderSource.geometryDataSource.currentCode);

            // Guarda el c�digo del shader en archivos
            saveShaderCode(savedShadersPath + "/" + shaderSource.name + "_vertex.glsl", vertexCode);
            saveShaderCode(savedShadersPath + "/" + shaderSource.name + "_fragment.glsl", fragmentCode);
            if (!geometryCode.empty()) {
                saveShaderCode(savedShadersPath + "/" + shaderSource.name + "_geometry.glsl", geometryCode);
            }

            Shader* shader = nullptr;
            if (!geometryCode.empty()) {
                shader = new Shader(true, vertexCode, fragmentCode, geometryCode);
            }
            else {
                shader = new Shader(true, vertexCode, fragmentCode);
            }

            compiledShaders[shaderSource.name] = shader;
        }
    }



    std::vector<std::string> GetShaderFiles(const std::string& path) {
        std::vector<std::string> items;

        for (const auto& entry : fs::directory_iterator(path)) {
            items.push_back(entry.path().filename().string());
        }
        return items;
    }



    std::string ShaderManager::readFile(const std::string& filePath) {

        std::ifstream fileStream(filePath, std::ios::in);

        if (!fileStream.is_open()) {
            std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
            return "";
        }

        std::string content;
        std::string line = "";
        while (!fileStream.eof()) {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }



    void ShaderManager::DrawShaderEditorPanel() {

        static std::string currentVertexCode(ShaderManager::ShaderBufferSize, '\0'); // Reservamos espacio para 20000 caracteres
        static std::string currentFragmentCode(ShaderManager::ShaderBufferSize, '\0');
        static std::string currentGeometryCode(ShaderManager::ShaderBufferSize, '\0');

        //// En alg�n lugar de tu inicializaci�n (por ejemplo, donde actualmente est�s declarando las variables static):
        currentVertexCode.resize(ShaderManager::ShaderBufferSize, '\0'); // Establece el contenido inicial a nulos para el buffer completo.
        currentFragmentCode.resize(ShaderManager::ShaderBufferSize, '\0');
        currentGeometryCode.resize(ShaderManager::ShaderBufferSize, '\0');

        static int selectedShaderIndex = -1;
        static int selectedProgramIndex = -1;

        // Lista de programas de shader a la izquierda
        if (ImGui::BeginChild("Shader List", ImVec2(150, 0), true)) 
        {
            //--LISTA
            for (int programIndex = 0; programIndex < shaderProgramSources.size(); programIndex++) {
                std::string treeNodeId = shaderProgramSources[programIndex].name + "##" + std::to_string(programIndex);
                bool node_open = ImGui::TreeNodeEx(treeNodeId.c_str(), 0);

                if (node_open) {
                    
                    if (ImGui::Selectable("Vertex", selectedShaderIndex == programIndex * 3 + 1)) {
                        selectedShaderIndex = programIndex * 3 + 1;
                        selectedProgramIndex = programIndex; // Aqu� actualizamos selectedProgramIndex

                        currentVertexCode = shaderProgramSources[programIndex].vertexDataSource.currentCode;
                    }
                    if (ImGui::Selectable("Fragment", selectedShaderIndex == programIndex * 3 + 2)) {
                        selectedShaderIndex = programIndex * 3 + 2;
                        selectedProgramIndex = programIndex;

                        currentFragmentCode = shaderProgramSources[programIndex].fragmentDataSource.currentCode;
                    }
                    if (!shaderProgramSources[programIndex].geometryDataSource.sourcePath.empty() &&
                        ImGui::Selectable("Geometry", selectedShaderIndex == programIndex * 3 + 3)) {
                        selectedShaderIndex = programIndex * 3 + 3;
                        selectedProgramIndex = programIndex;

                        currentGeometryCode = shaderProgramSources[programIndex].geometryDataSource.currentCode;
                    }
                    ImGui::TreePop();
                }
            }

            //--BOTONES
            if (selectedShaderIndex != -1) {

                if (ImGui::Button("Reload")) 
                {
                    currentVertexCode = vertexEditor.GetText();
                    currentFragmentCode = fragmentEditor.GetText();
                    currentGeometryCode = geometryEditor.GetText();

                    std::string theName = shaderProgramSources[selectedProgramIndex].name;
                    std::cout << "shaderProgramSources[selectedProgramIndex].name " << theName << std::endl;
                    shaderProgramSources[selectedProgramIndex].fragmentDataSource.currentCode = currentFragmentCode;
                    ReloadAllShaders();
                }

                ImGui::SameLine();

                if (ImGui::Button("Save")) {}
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        static int lastSelectedShaderIndex = -1; // Agrega una variable est�tica para rastrear la �ltima selecci�n

        if (selectedShaderIndex != -1 || selectedProgramIndex != -1) {
            if (ImGui::BeginChild("Shader Code Editor", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true)) {
                if (selectedShaderIndex != -1 && selectedShaderIndex != lastSelectedShaderIndex) {
                    // Actualiza el contenido del editor solo si la selecci�n ha cambiado
                    lastSelectedShaderIndex = selectedShaderIndex;
                    int shaderType = (selectedShaderIndex - 1) % 3;

                    switch (shaderType) {
                    case 0: // Vertex Shader
                        vertexEditor.SetText(currentVertexCode);
                        break;
                    case 1: // Fragment Shader
                        fragmentEditor.SetText(currentFragmentCode);
                        break;
                    case 2: // Geometry Shader
                        geometryEditor.SetText(currentGeometryCode);
                        break;
                    }
                }

                ImGuiIO& io = ImGui::GetIO();
                ImGui::PushFont(io.Fonts->Fonts[1]);
                if (selectedShaderIndex != -1) {
                    switch ((selectedShaderIndex - 1) % 3) {
                    case 0: // Vertex Shader
                        ImGui::Text("Vertex Shader");
                        vertexEditor.Render("VertexShaderEditor");
                        break;
                    case 1: // Fragment Shader
                        ImGui::Text("Fragment Shader");
                        fragmentEditor.Render("FragmentShaderEditor");
                        break;
                    case 2: // Geometry Shader
                        ImGui::Text("Geometry Shader");
                        geometryEditor.Render("GeometryShaderEditor");
                        break;
                    }
                }
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

}
