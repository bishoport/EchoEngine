#pragma once
#include <bitset>
#include "ScriptableComponent.h"
#include <unordered_map>
#include <memory>


namespace ECS_SCRIPTING
{

    class ScriptableEntity {

    private:


    public:

        int id;
        std::string name;
        bool markedToDelete = false;

        //-Componentes
        std::unordered_map<std::string, std::unique_ptr<ScriptableComponent>> components;


        ScriptableEntity(int nextID)
        {
            this->id = nextID;
        }

        int getID() const
        {
            return id;
        }

        void update(GLCore::Timestep deltaTime)
        {
            for (auto& pair : components) {
                std::string key = pair.first;
                ScriptableComponent* component = pair.second.get();
                component->update(deltaTime);
            }
        }
        void draw()
        {
            for (auto& pair : components) {
                std::string key = pair.first;
                ScriptableComponent* component = pair.second.get();
                component->draw();
            }
        }
        void drawGUI_Inspector()
        {
            for (auto& pair : components) {
                std::string key = pair.first;
                ScriptableComponent* component = pair.second.get();
                component->drawGUI_Inspector();
            }
        }
        bool isActive() const
        {
            return true;
        }
        void destroy(){}


        //--COMPONENTS

        void addComponentByPointer(const std::string& className) {

            //// Crear un nuevo componente 'Transform', que se supone es un ScriptableComponent o una clase derivada.
            //ECS_SCRIPTING::ScriptableComponent* component = new ECS_SCRIPTING::ScriptableComponent(); // O una clase derivada de ScriptableComponent.
            //component->ClassName = className;

            //components[className] = std::unique_ptr<ScriptableComponent>(component);


        }

        ScriptableComponent* getComponent(const std::string& key) {
            auto it = components.find(key);
            if (it != components.end()) {
                return it->second.get();
            }
            return nullptr; // O lanzar una excepción si prefieres.
        }

        void removeComponent(const std::string& key) {
            auto it = components.find(key);
            if (it != components.end()) {
                // Llamar a onDestroy si es necesario.
                // it->second->onDestroy();
                components.erase(it);
            }
        }


        void removeAllComponents()
        {
            // Llamar a onDestroy para cada componente antes de eliminarlos si es necesario.
            for (auto& pair : components) {
                pair.second->onDestroy();
            }

            // Eliminar todos los elementos del mapa, lo que destruirá los componentes.
            components.clear();
        }
    };



}