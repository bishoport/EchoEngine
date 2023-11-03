#pragma once

#include "../../glpch.h"

namespace GLCore::Utils
{
    class PostProcessingManager {
    public:
        bool isReady = false;
        GLuint FBO = 0;
        GLuint colorBuffers[2];
        GLuint depthBuffer = 0;


        bool hdr = false;
        bool bloom = false;


        PostProcessingManager();
        ~PostProcessingManager();

        void RenderWithPostProcess();

        void PrepareFBO(GLuint SCR_WIDTH, GLuint SCR_HEIGHT);

        void UpdateFBO_size(GLuint SCR_WIDTH, GLuint SCR_HEIGHT);

        void DrawGUI_Inspector();


    private:
        
    };
}

