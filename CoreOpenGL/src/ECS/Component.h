#pragma once
#include "Entity.h"

namespace ECS {
    class Component {
    public:
        Entity* entity;
        virtual void init() {}
        virtual void update() {}
        virtual void draw() {}
        virtual void drawGUI_Inspector() {}
        virtual void onDestroy() {}
        virtual ~Component() { onDestroy(); }
    };
}

