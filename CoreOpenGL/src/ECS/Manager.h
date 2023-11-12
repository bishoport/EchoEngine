#pragma once

#include "../glpch.h"

#include "Entity.h"
#include <vector>
#include <memory>
#include <fstream>

namespace ECS {
    class Manager {
    private:
        std::vector<std::unique_ptr<ECS::Entity>> entities;
        int nextID = 0;

    public:
        void update();
        void draw();
        void drawGUI_Inspector();
        void refresh();
        ECS::Entity& addEntity();
        void removeEntity(ECS::Entity* e, bool isRootCall = true);
        std::vector<ECS::Entity*> getAllEntities();

        void serializeEntities(const std::string& filename);
        void deserializeEntities(const std::string& filename);
    };
}

