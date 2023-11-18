#pragma once

#ifdef ECS_EXPORTS // Define esta macro para el proyecto DLL
#define ECS_API __declspec(dllexport)
#else
#define ECS_API __declspec(dllimport)
#endif


#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep
#include "../GLCore/Core/Timestep.h"




namespace ECS {

    using ComponentID = std::size_t;

    inline ComponentID getNewComponentTypeID() {
        static ComponentID lastID = 0u;
        return lastID++;
    }

    template <typename T>
    inline ComponentID getComponentTypeID() noexcept {
        static ComponentID componentID = getNewComponentTypeID();
        return componentID;
    }

    constexpr std::size_t maxComponents = 32;

    class Entity;

    class Component {
    private:
        ComponentID componentID; // Variable miembro para almacenar el ComponentID

    public:
        ECS::Entity* entity;

        //Component() : componentID(getNewComponentTypeID()) {} // Constructor para inicializar el componentID

        virtual void init() {}
        virtual void update(GLCore::Timestep deltaTime) {}
        virtual void draw() {}
        virtual void drawGUI_Inspector() {}
        virtual void onDestroy() {}
        virtual ~Component() { onDestroy(); }

        virtual ComponentID getComponentID() const { return componentID; } // Getter para componentID

        void setComponentID(ComponentID id) { componentID = id; } // Setter para componentID
    };
}


