// TestComponentFactory.cpp

#include "pch.h"
#include "TestComponent.h"

extern "C" ECS_API ECS::Component * CreateTestComponent() {
    auto* comp = new ECS::TestComponent();
    //comp->setComponentID(ECS::getComponentTypeID<ECS::TestComponent>()); // Esto asume que la DLL y la app principal comparten el mismo esquema de IDs
    return comp;
}
