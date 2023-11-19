#include "ScriptableEntity.h"

namespace ECS_SCRIPTING
{
    ScriptableEntity::ScriptableEntity(int nextID) : id(nextID) {}

    int ScriptableEntity::getID() const {
        return id;
    }

    void ScriptableEntity::update(GLCore::Timestep timestep) {
        for (auto& c : components) c->update(timestep);
    }

    void ScriptableEntity::draw() {
        for (auto& c : components) c->draw();
    }

    void ScriptableEntity::drawGUI_Inspector() {
        for (auto& c : components) c->drawGUI_Inspector();
    }

    const std::vector<std::unique_ptr<ScriptableComponent>>& ScriptableEntity::getComponents() const {
        return components;
    }

    bool ScriptableEntity::isActive() const {
        return active;
    }

    void ScriptableEntity::destroy() {
        active = false;
    }


    void ScriptableEntity::removeAllComponents() {
        while (!components.empty()) {
            components.back()->onDestroy(); // Llama a onDestroy antes de eliminar el componente
            components.pop_back(); // Elimina el último componente, llama al destructor
        }
    }


    void ScriptableEntity::addComponentByPointer(ScriptableComponent* component) {
       /* if (component == nullptr) {
            throw std::invalid_argument("Null component pointer passed to addComponentByPointer.");
       } */

        //auto componentID = getNewComponentTypeID();

        //// Añadir el nuevo componente y tomar propiedad con unique_ptr.
        //std::unique_ptr<ScriptableComponent> uPtr{ component };
        //components.emplace_back(std::move(uPtr));
        //componentArray[componentID] = components.back().get();
        //componentBitSet[componentID] = true;
        //component->setComponentID(componentID);
        //// Inicializar el componente y establecer su entidad propietaria.
        //component->entity = this;
        //component->init();
    }
}
