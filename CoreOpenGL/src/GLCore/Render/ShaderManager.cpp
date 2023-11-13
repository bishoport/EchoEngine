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

            if (shaderSource.geometrySource.c_str() == "")
            {
                ShaderManager::Load(shaderSource.name.c_str(), shaderSource.vertexSource.c_str(), shaderSource.fragmentSource.c_str());
                /*Shader* shader = new Shader(shaderSource.vertexSource.c_str(), shaderSource.fragmentSource.c_str());
                compiledShaders[shaderSource.name.c_str()] = shader;*/
            }
            else
            {
                ShaderManager::Load(shaderSource.name.c_str(), shaderSource.vertexSource.c_str(), shaderSource.fragmentSource.c_str(), shaderSource.geometrySource.c_str());
                /*Shader* shader = new Shader(shaderSource.vertexSource.c_str(), shaderSource.fragmentSource.c_str(), shaderSource.geometrySource.c_str());
                compiledShaders[shaderSource.name.c_str()] = shader;*/
            }
        }
        //----------------------------------------------------------------------------------------------------------------------------
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


    std::vector<std::string> GetShaderFiles(const std::string& path) {
        std::vector<std::string> items;

        for (const auto& entry : fs::directory_iterator(path)) {
            items.push_back(entry.path().filename().string());
        }

        return items;
    }


    std::string readFile(const std::string& filePath) {
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
        static int selectedItem = -1;
        static std::string currentVertexCode;
        static std::string currentFragmentCode;
        static std::string currentGeometryCode;

        ImGui::Begin("Shader Editor");

        // Lista de programas de shader a la izquierda
        if (ImGui::BeginChild("Shader List", ImVec2(150, 0), true)) {
            for (int i = 0; i < shaderProgramSources.size(); i++) {
                if (ImGui::Selectable(shaderProgramSources[i].name.c_str(), selectedItem == i)) {
                    selectedItem = i;
                    // Carga los códigos fuente de los shaders seleccionados
                    currentVertexCode = readFile(shaderProgramSources[i].vertexSource);
                    currentFragmentCode = readFile(shaderProgramSources[i].fragmentSource);
                    // Solo carga el código de geometría si está presente
                    if (!shaderProgramSources[i].geometrySource.empty()) {
                        currentGeometryCode = readFile(shaderProgramSources[i].geometrySource);
                    }
                    else {
                        currentGeometryCode.clear();
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Editor de texto para el código de los shaders a la derecha
        if (selectedItem != -1) {
            if (ImGui::BeginChild("Shader Code Editor", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true)) {
                if (selectedItem != -1) {
                    // Editor de texto para Vertex Shader
                    ImGui::Text("Vertex Shader");
                    ImGui::InputTextMultiline("##VertexCode", &currentVertexCode[0], currentVertexCode.capacity(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);

                    // Editor de texto para Fragment Shader
                    ImGui::Text("Fragment Shader");
                    ImGui::InputTextMultiline("##FragmentCode", &currentFragmentCode[0], currentFragmentCode.capacity(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);

                    // Si hay un Geometry Shader, añade un editor para eso también
                    if (!currentGeometryCode.empty()) {
                        ImGui::Text("Geometry Shader");
                        ImGui::InputTextMultiline("##GeometryCode", &currentGeometryCode[0], currentGeometryCode.capacity(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
                    }
                }
            }
            ImGui::EndChild();

        }

        ImGui::End();
    }

}

