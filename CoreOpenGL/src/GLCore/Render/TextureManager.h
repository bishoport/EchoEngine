#pragma once

#include "Shader.h"
#include <unordered_map>
#include <string>
#include <map>
#include "ShaderManager.h"

namespace GLCore::Render {

    struct TextureInfo {
        GLuint textureID;
        GLenum glTextureType; // Tipo de textura OpenGL (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, etc.)
        std::string type;     // Tipo de textura (albedo, normal, etc.)
        std::string name;     // Nombre descriptivo para uso en ImGui
    };

    class TextureManager {
    public:
        // Obtiene la instancia única de TextureManager
        static TextureManager& getInstance() {
            static TextureManager instance;
            return instance;
        }

        // Elimina los métodos de copia y asignación
        TextureManager(TextureManager const&) = delete;
        void operator=(TextureManager const&) = delete;


        // Desvincula todas las texturas de sus slots
        void unbindAllTextures() {
            for (const auto& [slot, texInfo] : boundTextures) {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(texInfo.glTextureType, 0); // Desvincula la textura del slot
            }
            // Opcionalmente, puede querer limpiar el registro después de desvincular
            clearBoundTextures();
        }


        // Limpia el registro de texturas vinculadas (opcionalmente, llamar al inicio de cada frame)
        void clearBoundTextures() {
            boundTextures.clear();
            resetSlots();
        }

        // Asigna un slot específico a un tipo de textura
        void assignSlotToTextureType(const std::string& textureType, int slot) {
            textureTypeSlots[textureType] = slot;
        }

        // Enlaza una textura a un slot específico basado en el tipo de textura
        int bindTextureToSlot(GLuint textureID, const std::string& uniformName, const std::string& shaderName, const std::string& textureType, const std::string& textureName, bool useShader)
        {
            int slotToReturn = -1;

            auto it = textureTypeSlots.find(textureType);
            if (it != textureTypeSlots.end()) {
                int slot = it->second;
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_2D, textureID);

                if (useShader)
                {
                    GLCore::Render::ShaderManager::Get(shaderName)->use();
                    GLCore::Render::ShaderManager::Get(shaderName)->setInt(uniformName, slot);
                }

                slotToReturn = slot;

                // Actualiza el registro de texturas
                boundTextures[slot] = { textureID, GL_TEXTURE_2D, textureType, textureName };
            } else 
            {
                std::cout << "ERROR CARGANDO GL_TEXTURE_2D: " << textureType << std::endl;
            }

            return slotToReturn;
        }

        void bindCubeMapToSlot(GLuint textureID, const std::string& uniformName, const std::string& shaderName, const std::string& textureType, const std::string& textureName,bool useShader)
        {
            auto it = textureTypeSlots.find(textureType);
            
            if (it != textureTypeSlots.end()) {

                int slot = it->second;
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // Usa GL_TEXTURE_CUBE_MAP aquí

                if (useShader)
                {
                    GLCore::Render::ShaderManager::Get(shaderName)->use();
                    GLCore::Render::ShaderManager::Get(shaderName)->setInt(uniformName, slot);
                }
                

                // Actualiza el registro de texturas con la nueva información, incluyendo el tipo OpenGL
                boundTextures[slot] = { textureID, GL_TEXTURE_CUBE_MAP, textureType, textureName };
            }
            else {
                std::cout << "ERROR CARGANDO GL_TEXTURE_CUBE_MAP: " << textureType << std::endl;
            }
        }

        // Función para obtener las texturas vinculadas y sus slots
        const std::unordered_map<int, TextureInfo>& getBoundTextures() const {
            return boundTextures;
        }

        // Restablece el contador de slots (opcional)
        void resetSlots() {
            nextSlot = 0;
        }

    private:
        TextureManager() : nextSlot(0) {} // Constructor privado

        int nextSlot; // Siguiente slot de textura disponible
        std::unordered_map<int, TextureInfo> boundTextures; // Registro de texturas vinculadas y sus slots
        std::map<std::string, int> textureTypeSlots; // Mapeo de tipos de textura a slots específicos
    };
}












//#pragma once
//
//#include "Shader.h"
//#include <unordered_map>
//#include <string>
//#include "ShaderManager.h"
//
//namespace GLCore::Render {
//    class TextureManager {
//    public:
//        // Obtiene la instancia única de TextureManager
//        static TextureManager& getInstance() {
//            static TextureManager instance;
//            return instance;
//        }
//
//        // Elimina los métodos de copia y asignación
//        TextureManager(TextureManager const&) = delete;
//        void operator=(TextureManager const&) = delete;
//
//
//        // Limpia el registro de texturas vinculadas (opcionalmente, llamar al inicio de cada frame)
//        void clearBoundTextures() {
//            boundTextures.clear();
//        }
//
//        // Enlaza una textura a un slot disponible y actualiza el shader
//        int bindTexture(GLuint textureID, const std::string& uniformName, const std::string& shaderName) {
//            int slot = nextSlot++;
//            glActiveTexture(GL_TEXTURE0 + slot);
//            glBindTexture(GL_TEXTURE_2D, textureID);
//
//            GLCore::Render::ShaderManager::Get(shaderName)->use();
//            GLCore::Render::ShaderManager::Get(shaderName)->setInt(uniformName, slot);
//
//            // Actualiza la información de la textura y su slot
//            boundTextures[slot] = textureID;
//
//            return slot;
//        }
//
//        // Función para obtener las texturas vinculadas y sus slots
//        const std::unordered_map<int, GLuint>& getBoundTextures() const {
//            return boundTextures;
//        }
//
//        // Restablece el contador de slots (opcional)
//        void resetSlots() {
//            nextSlot = 0;
//        }
//
//    private:
//        TextureManager() : nextSlot(0) {} // Constructor privado
//
//        int nextSlot; // Siguiente slot de textura disponible
//        std::unordered_map<int, GLuint> boundTextures; // Registro de texturas vinculadas y sus slots
//    };
//}



