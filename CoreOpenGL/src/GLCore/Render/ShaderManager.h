#pragma once

#include "Shader.h"
#include <string>
#include <unordered_map>


namespace GLCore::Render {
    class ShaderManager {
    public:

        struct ShaderProgramSource {
            std::string name= "";
            std::string vertexSource= "";
            std::string fragmentSource = "";
            std::string geometrySource = "";

            ShaderProgramSource(
                const std::string& nameValue = "",
                const std::string& vertexSourceValue = "",
                const std::string& fragmentSourceValue = "",
                const std::string& geometrySourceValue = "")
                : name(nameValue),
                vertexSource(vertexSourceValue),
                fragmentSource(fragmentSourceValue),
                geometrySource(geometrySourceValue) {}
        };


        static void LoadAllShaders();

        static void Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
        static Shader* Get(const std::string& name);
        static void CleanUp();

        static const std::unordered_map<std::string, Shader*>& GetAllShaders()
        {
            return compiledShaders;
        }

        static void DrawShaderEditorPanel();


    private:
        static std::unordered_map<std::string, Shader*> compiledShaders;
        static std::vector<ShaderProgramSource> shaderProgramSources;
    };
}

