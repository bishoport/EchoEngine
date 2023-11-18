#pragma once
#include <bitset>
#include "ScriptableComponent.h"
#include <vector>
#include <memory>


namespace Scripting::ECS
{
    using ComponentBitSet = std::bitset<maxComponents>;
    using ComponentArray = std::array<ScriptableComponent*, maxComponents>;


    class ScriptableEntity {

    private:


    public:

        int id;
        std::vector<std::unique_ptr<ScriptableComponent>> components;

        std::string name;
        bool active = true;
        bool markedToDelete = false;

        ScriptableEntity(int nextID);
        int getID() const;


        void update(GLCore::Timestep);
        void draw();
        void drawGUI_Inspector();
        const std::vector<std::unique_ptr<ScriptableComponent>>& getComponents() const;
        bool isActive() const;
        void destroy();
        void removeAllComponents();

       
        void addComponentByPointer(ScriptableComponent* component);

        template<typename T> bool hascomponent() const {
            auto componentID = getComponentTypeID<T>(); // Asumiendo que esto devuelve un ID único para cada tipo T
            if (componentID >= maxComponents) {
                // ID de tipo está fuera de rango, lo que significa que el componente no existe
                return false;
            }
            // Verifica si el bit correspondiente al tipo de componente está establecido
            return componentBitSet[componentID];
        }

        template <typename T, typename... TArgs> T& addComponent(TArgs&&... mArgs)
        {
            T* c(new T(std::forward<TArgs>(mArgs)...));
            c->entity = this;
            std::unique_ptr<Component> uPtr{ c };
            components.emplace_back(std::move(uPtr));
            componentArray[getComponentTypeID<T>()] = c;
            componentBitSet[getComponentTypeID<T>()] = true;

            c->init();
            return *c;
        }


        template<typename T> T& getComponent() const
        {
            auto ptr(componentArray[getComponentTypeID<T>()]);
            return *static_cast<T*>(ptr);
        }

        template<typename T> void removeComponent()
        {
            // Comprobar si el componente existe
            auto componentID = getComponentTypeID<T>();
            if (!componentBitSet[componentID]) {
                return; // No tiene el componente, así que no hay nada que eliminar
            }

            // Eliminar el puntero del vector 'components'
            auto it = std::find_if(components.begin(), components.end(),
                [](const std::unique_ptr<Component>& component) {
                    return dynamic_cast<T*>(component.get()) != nullptr;
                });
            if (it != components.end()) {
                // Llamar a onDestroy antes de eliminar el componente
                (*it)->onDestroy();

                // Realizar la eliminación efectiva del componente
                components.erase(it);
            }

            // Actualizar el 'componentArray' y 'componentBitSet'
            componentArray[componentID] = nullptr;
            componentBitSet[componentID] = false;
        }
    };



}