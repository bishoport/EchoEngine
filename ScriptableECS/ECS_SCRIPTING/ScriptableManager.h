#pragma once
#include "../src/glpch.h"
#include "../src/GLCore/Core/Timestep.h"

#include "ScriptableEntity.h"
#include <vector>
#include <memory>
#include <fstream>



namespace ECS_SCRIPTING
{
    class ScriptableManager {

    private:
        std::vector<std::unique_ptr<ScriptableEntity>> entities;
        int nextID = 0;

    public:
        void update(GLCore::Timestep deltaTime);
        void draw();
        void drawGUI_Inspector();
        void refresh();
        ScriptableEntity& addEntity();
        void removeEntity(ScriptableEntity* e, bool isRootCall = true);
        std::vector<ScriptableEntity*> getAllEntities();

        void serializeEntities(const std::string& filename);
        void deserializeEntities(const std::string& filename);
    };
}

