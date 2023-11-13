#include "ShaderManager.h"

#include "../../glpch.h"

#include <filesystem> // Para std::filesystem::path y las operaciones relacionadas con archivos

namespace fs = std::filesystem;

namespace GLCore::Render 
{

    std::vector<GLCore::Render::ShaderManager::ShaderProgramSource> ShaderManager::shaderProgramSources;
    std::unordered_map<std::string, Shader*> ShaderManager::compiledShaders;

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

    void ShaderManager::ReloadAllShaders(ShaderProgramSource& programSource) {
        // Obtén el programa de shader actual y desvincula todos los shaders.
        glUseProgram(0);
        GLuint programID = compiledShaders[programSource.name]->ID;
        GLint shaderCount;
        glGetProgramiv(programID, GL_ATTACHED_SHADERS, &shaderCount);

        std::vector<GLuint> attachedShaders(shaderCount);
        glGetAttachedShaders(programID, shaderCount, NULL, &attachedShaders[0]);

        for (GLuint shader : attachedShaders) {
            glDetachShader(programID, shader);
            glDeleteShader(shader);
        }

        //CleanUp();

        for (const auto& shaderSource : shaderProgramSources) {

            if (shaderSource.geometryDataSource.sourcePath.c_str() == "")
            {
                Shader* shader = new Shader(shaderSource.vertexDataSource.currentCode.c_str(), shaderSource.fragmentDataSource.currentCode.c_str());
                compiledShaders[shaderSource.name] = shader;
            }
            else
            {
                Shader* shader = new Shader(true,shaderSource.vertexDataSource.currentCode.c_str(), 
                                            shaderSource.fragmentDataSource.currentCode.c_str(), 
                                            shaderSource.geometryDataSource.currentCode.c_str());
                compiledShaders[shaderSource.name] = shader;
            }
        }

        //LoadAllShaders();

        //// Crea y compila los nuevos shaders.
        //GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, programSource.vertexDataSource.sourceCode);
        //GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, programSource.fragmentDataSource.sourceCode);
        //GLuint geometryShader = 0;
        //if (!programSource.geometryDataSource.sourceCode.empty()) {
        //    geometryShader = CompileShader(GL_GEOMETRY_SHADER, programSource.geometryDataSource.sourceCode);
        //}

        //// Vincula los nuevos shaders al programa.
        //glAttachShader(programID, vertexShader);
        //glAttachShader(programID, fragmentShader);
        //if (geometryShader != 0) {
        //    glAttachShader(programID, geometryShader);
        //}

        //// Enlaza el programa.
        //glLinkProgram(programID);
        //checkLinkStatus(programID);

        //// Luego de vincular, ya puedes eliminar los shaders individuales.
        //glDeleteShader(vertexShader);
        //glDeleteShader(fragmentShader);
        //if (geometryShader != 0) {
        //    glDeleteShader(geometryShader);
        //}

        //// Reemplaza el ID del programa en tu mapa de shaders compilados si es necesario.
        //compiledShaders[programSource.name]->ID = programID;

        //// Ahora puedes utilizar el nuevo programa con las modificaciones.
        //glUseProgram(programID);
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

        //// En algún lugar de tu inicialización (por ejemplo, donde actualmente estás declarando las variables static):
        currentVertexCode.resize(ShaderManager::ShaderBufferSize, '\0'); // Establece el contenido inicial a nulos para el buffer completo.
        currentFragmentCode.resize(ShaderManager::ShaderBufferSize, '\0');
        currentGeometryCode.resize(ShaderManager::ShaderBufferSize, '\0');

        static int selectedShaderIndex = -1;
        static int selectedProgramIndex = -1;

        // Variables estáticas para el tamaño de fuente y la escala de fuente
        static float fontSize = 16.0f; // Tamaño inicial de fuente
        static float fontScale = 1.0f; // Escala inicial (1.0f es el tamaño por defecto)


        ImGui::Begin("Shader Editor");
        ImGui::SliderFloat("Font Scale", &fontScale, 0.5f, 2.0f, "%.1f");
        

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
                        selectedProgramIndex = programIndex; // Aquí actualizamos selectedProgramIndex

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

                if (ImGui::Button("Reload")) {

                    std::string theName = shaderProgramSources[selectedProgramIndex].name;
                    std::cout << "shaderProgramSources[selectedProgramIndex].name " << theName << std::endl;

                    //shaderProgramSources[selectedProgramIndex].vertexDataSource.currentCode = currentVertexCode;
                    shaderProgramSources[selectedProgramIndex].fragmentDataSource.currentCode = currentFragmentCode;

                    ReloadAllShaders(shaderProgramSources[selectedProgramIndex]);
                }

                ImGui::SameLine();

                if (ImGui::Button("Save")) {
                    // Lógica para guardar el código actual del shader en un archivo
                }
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        //--EDITOR
        if (selectedShaderIndex != -1 || selectedProgramIndex != -1) {
            
            ImFont* font = ImGui::GetFont();
            font->Scale = fontScale; // Establece la nueva escala
            
            if (ImGui::BeginChild("Shader Code Editor", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true)) {
                if (selectedShaderIndex != -1) {
                    int shaderType = (selectedShaderIndex - 1) % 3;
                    switch (shaderType) {
                    case 0: // Vertex Shader
                        ImGui::Text("Vertex Shader");
                        ImGui::InputTextMultiline("##VertexCode", &currentVertexCode[0], ShaderBufferSize, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_AllowTabInput);
                        break;
                    case 1: // Fragment Shader
                        ImGui::Text("Fragment Shader");
                        ImGui::InputTextMultiline("##FragmentCode", &currentFragmentCode[0], ShaderBufferSize, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_AllowTabInput);
                        break;
                    case 2: // Geometry Shader
                        ImGui::Text("Geometry Shader");
                        ImGui::InputTextMultiline("##GeometryCode", &currentGeometryCode[0], ShaderBufferSize, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_AllowTabInput);
                        break;
                    }
                }
            }
            font->Scale = 1.0f;
            ImGui::EndChild();
        }

        ImGui::End();
    }



}

