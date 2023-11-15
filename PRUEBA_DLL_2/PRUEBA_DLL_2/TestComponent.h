#pragma once

#include "Component.h"
#include <iostream>


namespace ECS
{
    class TestComponent : public ECS::Component
    {
    public:

        void init() override
        {
            std::cout << "Type ID en el Component TestComponent INIT" << getTypeID() << std::endl;
        }

        ComponentID getTypeID() const
        {
            return getComponentTypeID<TestComponent>();
        }

        void update(GLCore::Timestep timestamp) override {
            //std::cout << "HOLAAA update 3" << std::endl;
        }

        void drawGUI_Inspector() override;

    };
} // namespace ECS