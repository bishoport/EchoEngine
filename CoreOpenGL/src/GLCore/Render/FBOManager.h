#pragma once

#include "Shader.h"
#include <string>
#include <unordered_map>


namespace GLCore::Render {

    class FBOManager {

    public:
        //static void CreateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, std::vector<GLuint*>& colorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT);
        static std::vector<GLuint> CreateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, unsigned int numColorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT);
        static void UpdateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, std::vector<GLuint> colorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT);

        static void CreateShadowMapFBO(GLuint* fbo, GLuint* shadowMap, GLuint SCR_WIDTH, GLuint SCR_HEIGHT);
    };
}
