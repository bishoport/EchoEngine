#include "ShaderManager.h"

namespace GLCore::Render {
    std::unordered_map<std::string, Shader*> ShaderManager::m_Shaders;

    void ShaderManager::Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
        Shader* shader = new Shader(vertexPath.c_str(), fragmentPath.c_str(), geometryPath.empty() ? nullptr : geometryPath.c_str());
        m_Shaders[name] = shader;
    }

    Shader* ShaderManager::Get(const std::string& name) {
        if (m_Shaders.find(name) != m_Shaders.end()) {
            return m_Shaders[name];
        }
        return nullptr;
    }

    void ShaderManager::CleanUp() {
        for (auto& [name, shader] : m_Shaders) {
            delete shader;
        }
        m_Shaders.clear();
    }
}

