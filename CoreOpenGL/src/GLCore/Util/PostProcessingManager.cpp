#include "PostProcessingManager.h"
#include "../Render/FBOManager.h"
#include "IMGLoader.h"

namespace GLCore::Utils
{
    PostProcessingManager::PostProcessingManager(){}
    PostProcessingManager::~PostProcessingManager(){}


    void PostProcessingManager::Init(GLuint SCR_WIDTH, GLuint SCR_HEIGHT) 
    {
        colorBuffers = GLCore::Render::FBOManager::CreateFBO_Color_RGBA16F(&FBO, &depthBuffer, 2, SCR_WIDTH, SCR_HEIGHT);

        //--RESIZE WINDOW EVENT
        //EventManager::getWindowResizeEvent().subscribe([this](GLuint width, GLuint height) {
        //    GLCore::Render::FBOManager::UpdateFBO_Color_RGBA16F(&FBO, &depthBuffer, colorBuffers, width, height);
        //});
        // ---------------------------------------

        EventManager::getOnPanelResizedEvent().subscribe([this](const std::string name, const ImVec2& size, const ImVec2& position)
            {
                if (name == "SCENE")
                {
                    GLCore::Render::FBOManager::UpdateFBO_Color_RGBA16F(&FBO, &depthBuffer, colorBuffers, size.x, size.y);
                }
            });


        lookupTableTexture0 = GLCore::Utils::ImageLoader::load2DLUTTexture("assets/default/LUT/lookup-table-0.png");
        lookupTableTexture1 = GLCore::Utils::ImageLoader::load2DLUTTexture("assets/default/LUT/lookup-table-1.png");
        

        isReady = true;
    }

    void PostProcessingManager::RenderWithPostProcess() 
    {
        if (isReady)
        {
            GLCore::Render::ShaderManager::Get("postprocessing")->use();

            //--HDR
            GLCore::Render::ShaderManager::Get("postprocessing")->setBool("hdr", hdr);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("exposure", exposure);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("gamma", gamma);
            //----------------------------------------------------------------------------------------------------------------


            //--BLOM
            GLCore::Render::ShaderManager::Get("postprocessing")->setBool("bloom", bloom);
            //----------------------------------------------------------------------------------------------------------------
       
            
            //--Color Curve LUT
            GLCore::Render::ShaderManager::Get("postprocessing")->setBool("colorCurvesLUT", colorCurveLUT);
            glActiveTexture(GL_TEXTURE0 + 8);
            glBindTexture(GL_TEXTURE_2D, lookupTableTexture0);
            glActiveTexture(GL_TEXTURE0 + 9);
            glBindTexture(GL_TEXTURE_2D, lookupTableTexture1);
            GLCore::Render::ShaderManager::Get("postprocessing")->setInt("lookupTableTexture0", 8);
            GLCore::Render::ShaderManager::Get("postprocessing")->setInt("lookupTableTexture1", 9);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("sunPosition", sunPosition);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("lutGamma", lutGamma);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("lutIntensity", lutIntensity);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("shadowIntensity", shadowIntensity);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("midtoneIntensity", midtoneIntensity);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("highlightIntensity", highlightIntensity);
            //----------------------------------------------------------------------------------------------------------------

            //--ACES
            GLCore::Render::ShaderManager::Get("postprocessing")->setBool("ACES", ACES);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("ACES_a", currentParameters.ACES_a);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("ACES_b", currentParameters.ACES_b);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("ACES_c", currentParameters.ACES_c);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("ACES_d", currentParameters.ACES_d);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("ACES_e", currentParameters.ACES_e);
            //----------------------------------------------------------------------------------------------------------------

            //--SATURATION
            GLCore::Render::ShaderManager::Get("postprocessing")->setBool("SATURATION", SATURATION);
            GLCore::Render::ShaderManager::Get("postprocessing")->setVec3("whiteBalanceColor", whiteBalanceColor);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("saturationRed", saturationRed);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("saturationGreen", saturationGreen);
            GLCore::Render::ShaderManager::Get("postprocessing")->setFloat("saturationBlue", saturationBlue);


            int textureIndex = 5;
            for (size_t i = 0; i < 2; i++)
            {
                glActiveTexture(GL_TEXTURE0 + textureIndex);
                glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
                std::string uniformName = "colorBuffer_" + std::to_string(i);
                GLCore::Render::ShaderManager::Get("postprocessing")->setInt(uniformName.c_str(), textureIndex);

                textureIndex++;
            }
        }
    }

    void PostProcessingManager::DrawGUI_Inspector()
    {
        if (isReady)
        {
            //--HDR
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("HDR", &hdr);
            if (hdr)
            {
                ImGui::SliderFloat("HDR EXPOSURE", &exposure, 0.0f, 10.0f, "%.5f");
                ImGui::SliderFloat("HDR GAMMA", &gamma, 0.0f, 3.0f, "%.5f");
            }
            ImGui::Separator();
            //-----------------------------------------------------------------------------

            //--BLOOM
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("BLOOM", &bloom);
            if (bloom)
            {
            }
            ImGui::Separator();
            //------------------------------------------------------------------------------

            //--COLOR CURVES LUT
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("COLOR CURVES LUT", &colorCurveLUT);
            if (colorCurveLUT)
            {
                ImGui::SliderFloat("Intensity", &lutIntensity, 0.0f, 1.0f, "%.3f");
                ImGui::SliderFloat("lut Gamma", &lutGamma, 0.0f, 1.0f, "%.3f");


                ImGui::SliderFloat("Shadow", &shadowIntensity, -1.0f, 1.0f, "%.3f");
                ImGui::SliderFloat("Midtone", &midtoneIntensity, -1.0f, 1.0f, "%.3f");
                ImGui::SliderFloat("Highlight", &highlightIntensity, -1.0f, 1.0f, "%.3f");

                ImGui::SliderFloat("Evolution Light", &sunPosition, -1.0f, 1.0f, "%.3f");

                ImGui::Image((void*)(intptr_t)lookupTableTexture0, ImVec2(256, 16), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
                ImGui::Image((void*)(intptr_t)lookupTableTexture1, ImVec2(256, 16), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
            }
            ImGui::Separator();
            //------------------------------------------------------------------------------


            //--ACES
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("ACES", &ACES);

            if (ACES)
            {

                for (int i = 0; i < sizeof(presets) / sizeof(presets[0]); ++i) {
                    std::string buttonLabel = presets[i].name;
                    if (ImGui::Button(buttonLabel.c_str())) {
                        currentParameters = presets[i];
                        // Aquí aplicarías el preset seleccionado a tu shader.
                    }
                }
                // Opcionalmente, podrías mostrar y editar los valores actuales.
                ImGui::SliderFloat("Intensidad de las luces", &currentParameters.ACES_a, 1.0f, 3.0f);
                ImGui::SliderFloat("Offset de las luces altas", &currentParameters.ACES_b, 0.0f, 0.1f);
                ImGui::SliderFloat("Contraste general", &currentParameters.ACES_c, 1.0f, 3.0f);
                ImGui::SliderFloat("Contraste en sombras/midtones", &currentParameters.ACES_d, 0.5f, 1.0f);
                ImGui::SliderFloat("Normalizacion para negros", &currentParameters.ACES_e, 0.0f, 0.2f);

            }

            ImGui::Separator();
            //------------------------------------------------------------------------------


            //--SATURATION
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("SATURATION", &SATURATION);

            if (SATURATION)
            {
                ImGui::ColorEdit3("White Balance", glm::value_ptr(whiteBalanceColor));
                ImGui::SliderFloat("Red Saturation", &saturationRed, 0.0f, 5.0f, "%.4f");
                ImGui::SliderFloat("Green Saturation", &saturationGreen, 0.0f, 5.0f, "%.4f");
                ImGui::SliderFloat("Blue Saturation", &saturationBlue, 0.0f, 5.0f, "%.4f");
            }
            ImGui::Separator();

            
            /*ImGui::Dummy(ImVec2(0.0f, 5.0f));
            for (int i = 0; i < 2; i++)
            {
                ImGui::Image((void*)(intptr_t)colorBuffers[i], ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
            }
            ImGui::Dummy(ImVec2(0.0f, 5.0f));*/
            

        }
    }
}
