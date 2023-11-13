#pragma once

#include "Shader.h"
#include <string>
#include <unordered_map>


namespace GLCore::Render {


    class ShaderManager {
    public:

        struct ShaderDataSource {
            GLCore::Render::Shader::ShaderType shaderType;
            std::string sourcePath;
            std::string currentCode;

            // Constructor por defecto con valores predeterminados
            ShaderDataSource()
                : shaderType(GLCore::Render::Shader::ShaderType::NONE), sourcePath("") {}

            // Constructor con parámetros para inicializar miembros
            ShaderDataSource(GLCore::Render::Shader::ShaderType type, const std::string& path)
                : shaderType(type), sourcePath(path) {}
        };

        struct ShaderProgramSource {
            std::string name;

            ShaderDataSource vertexDataSource;
            ShaderDataSource fragmentDataSource;
            ShaderDataSource geometryDataSource;

            // Constructor con valores predeterminados que asigna los tipos de shaders adecuados
            ShaderProgramSource(const std::string& nameValue = "",
                const std::string& vertexPath = "",
                const std::string& fragmentPath = "",
                const std::string& geometryPath = "")
                : name(nameValue),
                // Asigna el tipo de shader correspondiente a cada ShaderDataSource
                vertexDataSource(GLCore::Render::Shader::ShaderType::VERTEX, vertexPath),
                fragmentDataSource(GLCore::Render::Shader::ShaderType::FRAGMENT, fragmentPath),
                geometryDataSource(GLCore::Render::Shader::ShaderType::GEOMETRY, geometryPath.empty() ? "" : geometryPath) // Asigna el path solo si geometryPath no está vacío
            {
            }
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
        // Establece un tamaño grande para el buffer de código del shader.
        static const size_t ShaderBufferSize = 40000;

        static std::unordered_map<std::string, Shader*> compiledShaders;
        static std::vector<ShaderProgramSource> shaderProgramSources;
        static void ReloadAllShaders(ShaderProgramSource& programSource);

        static std::string readFile(const std::string& filePath);

    };
}

