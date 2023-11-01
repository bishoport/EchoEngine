#include "PostProcessingManager.h"
#include "../Render/FBOManager.h"


namespace GLCore::Utils
{
    GLCore::Utils::PostProcessingManager::PostProcessingManager(GLuint screenWith, GLuint screenHeight)
    {

        //colorBuffers.push_back(&originalColorBuffer);
        //colorBuffers.push_back(&brightColorBuffer);
        //GLCore::Render::FBOManager::CreateFBO_Color_RGBA16F(&postproFBO, &postproRboDepth, colorBuffers, 800, 600);

        //this->SCR_WIDTH = screenWith;
        //this->SCR_HEIGHT = screenHeight;

        ////// Suscribir una función al evento de redimensionamiento de la ventana
        //EventManager::getWindowResizeEvent().subscribe([this](int width, int height)
        //{
        //    this->SCR_WIDTH = width;
        //    this->SCR_HEIGHT = height;
        //    GLCore::Render::FBOManager::UpdateFBO_Color_RGBA16F(&this->postproFBO, &this->postproRboDepth, this->colorBuffers, width, height);
        //});

        //// ping-pong-framebuffer for blurring
        //unsigned int pingpongFBO[2];
        //unsigned int pingpongColorbuffers[2];
        //glGenFramebuffers(2, pingpongFBO);
        //glGenTextures(2, pingpongColorbuffers);
        //for (unsigned int i = 0; i < 2; i++)
        //{
        //    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        //    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        //    // also check if framebuffers are complete (no need for depth buffer)
        //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        //        std::cout << "Framebuffer not complete!" << std::endl;
        //}
    }


    GLCore::Utils::PostProcessingManager::~PostProcessingManager()
    {

    }

    void PostProcessingManager::PrepareFX(GLuint screenWith, GLuint screenHeight)
    {
        
    }

    void GLCore::Utils::PostProcessingManager::RenderWithPostProcess()
    {
        


    }
}
