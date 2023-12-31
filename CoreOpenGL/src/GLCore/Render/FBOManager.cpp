#include "FBOManager.h"

namespace GLCore::Render 
{
#include <vector>

    std::vector<GLuint> FBOManager::CreateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, const unsigned int numColorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT) {

        // set up floating point framebuffer to render scene to
        glGenFramebuffers(1, fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
        CheckGLError();

        std::vector<GLuint> colorBuffers(numColorBuffers);
        glGenTextures(numColorBuffers, colorBuffers.data());
        CheckGLError();

        // create and attach depth buffer (renderbuffer) first
        glGenRenderbuffers(1, rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, *rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *rboDepth);
        CheckGLError();

        for (unsigned int i = 0; i < numColorBuffers; i++) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            CheckGLError();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            CheckGLError();

            // attach texture to framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
            CheckGLError();
        }

        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        std::vector<GLenum> attachments(numColorBuffers);
        for (unsigned int i = 0; i < numColorBuffers; ++i) {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(numColorBuffers, attachments.data());
        CheckGLError();

        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CheckGLError();

        return colorBuffers;
    }




    void FBOManager::CreateShadowMapFBO(GLuint* shadowFBO, GLuint* shadowDepth, GLuint* shadowTex, GLuint shadowMapResolution)
    {
        glGenTextures(1, shadowTex);
        glBindTexture(GL_TEXTURE_2D, *shadowTex);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, shadowMapResolution, shadowMapResolution, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenTextures(1, shadowDepth);
        glBindTexture(GL_TEXTURE_2D, *shadowDepth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenFramebuffers(1, shadowFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, *shadowFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *shadowTex, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *shadowDepth, 0);
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("FBO Error\n");
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }



    void FBOManager::CheckGLError() {
        /*GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }*/
    }



    void FBOManager::UpdateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, std::vector<GLuint> colorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT) {
        // Vincular el framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
        CheckGLError();

        // Actualizar las texturas de los color buffers
        for (auto colorBuffer : colorBuffers) {
            glBindTexture(GL_TEXTURE_2D, colorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            CheckGLError();
        }

        // Actualizar el renderbuffer de profundidad
        glBindRenderbuffer(GL_RENDERBUFFER, *rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        CheckGLError();

        // Comprobar el estado del framebuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete after resizing!" << std::endl;
        }
        CheckGLError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CheckGLError();
    }








    //// configure floating point framebuffer
    //glGenFramebuffers(1, fbo);
    //// create floating point color buffer
    //glGenTextures(1, colorBuffer);
    //glBindTexture(GL_TEXTURE_2D, *colorBuffer);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    //// create depth buffer (renderbuffer)
    //glGenRenderbuffers(1, rboDepth);
    //glBindRenderbuffer(GL_RENDERBUFFER, *rboDepth);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);


    //// attach buffers
    //glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorBuffer, 0);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *rboDepth);
    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    std::cout << "Framebuffer not complete!" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
