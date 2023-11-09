#pragma once

#include "../../glpch.h"

namespace GLCore::Utils
{
    class IBLManager {
    public:

        //IBL
        GLuint IBL_FBO= 0;
        GLuint IBL_RBO= 0;

        GLuint envCubemap = 0;
        GLuint irradianceMap = 0;
        GLuint prefilterMap = 0;
        GLuint brdfLUTTexture = 0;


        void prepare_PBR_IBL(int screenWidth, int screenHeight);
        //----------------------------------------------------


    private:

        GLuint cubeVAO = 0;
        GLuint cubeVBO = 0;
        GLuint quadVAO = 0;
        GLuint quadVBO = 0;

        GLuint m_hdrTexture;

        void renderCube();
        void renderQuad();
    };
}
