#pragma once

#include "../../glpch.h"
#include "../../ECS/Entity.h"


namespace GLCore::Render
{
    class RendererManager {
    public:
        RendererManager();
        ~RendererManager();

        std::vector<ECS::Entity*> entitiesInScene;

        void passLights();
        void passShadow();
        void passGeometry();

        static std::pair<glm::vec3, float> SceneBounds;
        void CalcSceneBundle();

    private:
        GLuint SCR_WIDTH = 800;
        GLuint SCR_HEIGHT = 600;
    };
}
