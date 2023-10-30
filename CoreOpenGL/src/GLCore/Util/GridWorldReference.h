#pragma once



#include "../../glpch.h"

namespace GLCore::Utils
{

    class GridWorldReference {
    public:

        int slices = 10;
        float sizeMultiplicator = 10.0f;
        bool showGrid = true;

        // Constructor que toma las rutas a las texturas de la caja del cielo (por ejemplo, derecha, izquierda, arriba, abajo, atrás, delante).
        GridWorldReference();

        // Destructor para limpiar los recursos.
        ~GridWorldReference();

        // Renderiza la caja del cielo.
        void Render();


    private:
        unsigned int axesVAO;
        unsigned int gridVAO;
        std::vector<float> vertexBuffer;
        GLuint lenght;
    };
}