#pragma once

#include "../../glpch.h"

namespace GLCore::Utils
{
    class PostProcessingManager {
    public:
        bool isActive = true;
       
        bool hdr = false;
        bool bloom = false;

        GLuint postproFBO;
        GLuint postproRboDepth;

        std::vector<GLuint*> colorBuffers;
        GLuint originalColorBuffer;
        GLuint brightColorBuffer;

        PostProcessingManager(GLuint screenWith, GLuint screenHeight);
        ~PostProcessingManager();


        void PrepareFX(GLuint screenWith, GLuint screenHeight);

        void RenderWithPostProcess();


    private:
        


        GLuint SCR_WIDTH = 800;
        GLuint SCR_HEIGHT = 600;
        
    };
}

