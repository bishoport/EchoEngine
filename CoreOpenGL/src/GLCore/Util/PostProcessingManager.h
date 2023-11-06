#pragma once

#include "../../glpch.h"

namespace GLCore::Utils
{
    class PostProcessingManager {
    public:

        struct ACESParameters {
            std::string name;
            float ACES_a;
            float ACES_b;
            float ACES_c;
            float ACES_d;
            float ACES_e;
        };

        const ACESParameters presets[4] = {
            {" Natural Look",1.0f, 0.03f, 1.8f, 0.7f, 0.05f},  // Natural Look
            {"High Contrast & Dramatic",2.0f, 0.05f, 2.5f, 0.8f, 0.02f},  // High Contrast & Dramatic
            {"Soft & Low Contrast",1.2f, 0.02f, 1.2f, 0.5f, 0.1f},   // Soft & Low Contrast
            {"Bright & Airy",1.5f, 0.1f, 1.5f, 0.6f, 0.1f}     // Bright & Airy
        };
        // Asignar un preset.
        ACESParameters currentParameters = presets[0]; // Inicializa con el primer preset

        bool isReady = false;

        //--FBO, DEPTH, COLOR BUFFER
        GLuint FBO = 0;
        GLuint depthBuffer = 0;
        std::vector<GLuint> colorBuffers;
        //------------------------

        //--HDR
        bool hdr = false;
        float exposure = 1.0f;
        float gamma = 2.2f;
        //------------------------

        //--BLOOM
        bool bloom = false;
        //------------------------

        //--COLOR CURVE LUT
        bool colorCurveLUT = false;
        
        float sunPosition = 0.0f;

        GLuint lookupTableTexture0;
        GLuint lookupTableTexture1;

        float lutIntensity = 0.0f;  // Intensidad del efecto
        float lutGamma = 0.0f;  // Intensidad del efecto
        float shadowIntensity = 0.0f;  // Intensidad de la sombra
        float midtoneIntensity = 0.0f; // Intensidad del tono medio
        float highlightIntensity = 0.0f; // Intensidad de la luz alta
        //------------------------


        //--ACES
        bool ACES = false;
        float ACES_a; // Controla la intensidad de las luces. Valores m�s altos resultan en realces m�s brillantes.
        float ACES_b; // Afecta la transici�n de las medias tonalidades a las luces altas, actuando como un offset.
        float ACES_c; // Ajusta el contraste general de la imagen. Valores m�s altos aumentan el contraste.
        float ACES_d; // Afecta las sombras/midtones, desempe�ando un papel en el contraste de �reas m�s oscuras.
        float ACES_e; // Es un t�rmino de normalizaci�n para asegurarse de que los negros se mapean correctamente.
        //------------------------


        //--SATURATION
        bool SATURATION = false;
        glm::vec3 whiteBalanceColor = glm::vec3(0.0f);
        float saturationRed;   // Saturaci�n para el canal rojo
        float saturationGreen; // Saturaci�n para el canal verde
        float saturationBlue;  // Saturaci�n para el canal azul


        PostProcessingManager();
        ~PostProcessingManager();

        void RenderWithPostProcess();

        void Init(GLuint SCR_WIDTH, GLuint SCR_HEIGHT);

        void DrawGUI_Inspector();


    private:
        
    };
}

