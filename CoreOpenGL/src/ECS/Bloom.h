#pragma once
#include "ECS.h"



namespace ECS
{
    class Bloom : public ECS::Component
    {
    public:

        bool ready = false;
        GLuint FBO = 0;
        GLuint colorBuffers[2];
        GLuint depthBuffer = 0;

        // Tamaño de los buffers (puede ser el tamaño de la ventana o diferente para el postprocesado)
        GLuint SCR_WIDTH = 800;
        GLuint SCR_HEIGHT = 600;



        void prepare(GLuint SCR_WIDTH, GLuint SCR_HEIGHT) {

            // 1. Generar y configurar el framebuffer
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // 2. Crear múltiples color attachments
            glGenTextures(2, colorBuffers);

            for (GLuint i = 0; i < 2; i++) {
                glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                // Vincular la textura al framebuffer
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
            }

            // 3. Crear y adjuntar un renderbuffer de profundidad (depth buffer)
            glGenRenderbuffers(1, &depthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

            // 4. Especificar a qué color attachments se escribirá
            GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);

            // 5. Comprobar si el framebuffer es completo
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer no está completo!" << std::endl;

            // 6. Desvincular el framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            ready = true;
        }


        void update(GLuint SCR_WIDTH, GLuint SCR_HEIGHT) {
            // Vincular el framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // Actualizar las texturas de los color buffers
            for (int i = 0; i < 2; i++) {
                glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            }

            // Actualizar el renderbuffer de profundidad
            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

            // Desvincular el framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }



        void drawGUI_Inspector() override
        {
            ImGui::Text("Bloom");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
            for (size_t i = 0; i < 2; i++)
            {
                ImGui::Image((void*)(intptr_t)colorBuffers[i], ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
            }


            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    };
}
