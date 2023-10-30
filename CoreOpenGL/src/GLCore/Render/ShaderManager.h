#pragma once

#include "Shader.h"
#include <string>
#include <unordered_map>


namespace GLCore::Render {
    class ShaderManager {
    public:
        static void Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
        static Shader* Get(const std::string& name);
        static void CleanUp();
        static const std::unordered_map<std::string, Shader*>& GetAllShaders()
        {
            return m_Shaders;
        }

    private:
        static std::unordered_map<std::string, Shader*> m_Shaders;
    };
}

