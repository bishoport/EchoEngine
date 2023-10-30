#pragma once

#include "Shader.h"
#include <string>
#include <unordered_map>


namespace GLCore::Render {

    class FBOManager {

    public:
        static void CreateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, std::vector<GLuint*>& colorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT);
        static void UpdateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, std::vector<GLuint*>& colorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT);
    };
}
