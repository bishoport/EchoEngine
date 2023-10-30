#include "FBOManager.h"

namespace GLCore::Render 
{
#include <vector>

    void FBOManager::CreateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, std::vector<GLuint*>& colorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT) {
        
        // set up floating point framebuffer to render scene to
        glGenFramebuffers(1, fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

        for (unsigned int i = 0; i < colorBuffers.size(); i++) {
            glGenTextures(1, colorBuffers[i]);
            glBindTexture(GL_TEXTURE_2D, *colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // attach texture to framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, *colorBuffers[i], 0);
        }

        // create and attach depth buffer (renderbuffer)
        glGenRenderbuffers(1, rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, *rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *rboDepth);

        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        std::vector<GLenum> attachments(colorBuffers.size());
        for (unsigned int i = 0; i < colorBuffers.size(); ++i) {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(colorBuffers.size(), attachments.data());

        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FBOManager::UpdateFBO_Color_RGBA16F(GLuint* fbo, GLuint* rboDepth, std::vector<GLuint*>& colorBuffers, GLuint SCR_WIDTH, GLuint SCR_HEIGHT) {
        // Actualizar las texturas de los color buffers
        for (auto colorBuffer : colorBuffers) {
            glBindTexture(GL_TEXTURE_2D, *colorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        }

        // Actualizar el renderbuffer de profundidad
        glBindRenderbuffer(GL_RENDERBUFFER, *rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

        // Comprobar el estado del framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete after resizing!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
