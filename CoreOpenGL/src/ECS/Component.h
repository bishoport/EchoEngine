#pragma once
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

#include "../GLCore/Core/Timestep.h"

namespace ECS {
    class Entity;

    class Component {
    public:
        ECS::Entity* entity;
        virtual void init() {}
        virtual void update(GLCore::Timestep deltaTime) {}
        virtual void draw() {}
        virtual void drawGUI_Inspector() {}
        virtual void onDestroy() {}
        virtual ~Component() { onDestroy(); }
    };
}

