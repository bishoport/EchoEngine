#pragma once
#include "../src/glpch.h"
#include "../src/GLCore/Core/Timestep.h"

#include "ScriptableEntity.h"
#include <vector>
#include <memory>
#include <fstream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace ECS_SCRIPTING
{
    class ScriptableManager {

    private:
        std::vector<std::unique_ptr<ScriptableEntity>> entities;
        int nextID = 0;

    public:
        void update(GLCore::Timestep deltaTime)
        {
            for (auto& e : entities) e->update(deltaTime);
        }

        void draw() {
           for (auto& e : entities) e->draw();
        }

        void drawGUI_Inspector() {
          for (auto& e : entities) e->drawGUI_Inspector();
        }

        void refresh() {
            /*for (size_t i = 0; i < entities.size();)
            {
                if (entities[i]->markedToDelete) {
                    removeEntity(entities[i].get());
                }
                else {
                    ++i;
                }
            }*/
        }

        ScriptableEntity& addEntity()
        {
            ScriptableEntity* e = new ScriptableEntity(nextID);
            nextID++;
            std::unique_ptr<ScriptableEntity> uPtr{ e };
            entities.emplace_back(std::move(uPtr));
            return *e;
        }

        void removeEntity(ScriptableEntity* e, bool isRootCall) {
            //if (!e)
            //{
            //    std::cout << "Si el puntero a la entidad es nulo, no hacer nada." << std::endl;
            //    return; // Si el puntero a la entidad es nulo, no hacer nada.
            //}

            //// Desactivar temporalmente la entidad
            //e->active = false;

            //// Intenta encontrar el componente Transform sin eliminar los componentes todavía.
            //ECS::Transform* transform = nullptr;
            //try {
            //    transform = &(e->getComponent<ECS::Transform>());
            //}
            //catch (...) {
            //    // Manejar la excepción
            //}

            //if (transform) {
            //    auto childrenCopy = transform->children; // Hacer una copia de los hijos para iterar de manera segura.

            //    for (auto it = childrenCopy.rbegin(); it != childrenCopy.rend(); ++it) {
            //        removeEntity(*it, false); // Aquí es una llamada recursiva, así que no es la llamada "raíz".
            //    }
            //}

            //e->removeAllComponents(); // Eliminar todos los componentes de la entidad.

            //auto it = std::find_if(entities.begin(), entities.end(),
            //    [e](const std::unique_ptr<ScriptableEntity>& entity) { return entity.get() == e; });

            //if (it != entities.end()) {
            //    entities.erase(it); // Elimina la entidad del vector.
            //}
        }

        std::vector<ScriptableEntity*> getAllEntities() {
            std::vector<ECS_SCRIPTING::ScriptableEntity*> activeEntities;
            for (auto& entity : entities) {
                if (entity->isActive()) {
                    activeEntities.push_back(entity.get());
                }
            }
            return activeEntities;
        }


        void serializeEntities(const std::string& filename) {
            //YAML::Emitter out;
            //out << YAML::BeginSeq;
            //for (const auto& entity : entities) {
            //    if (entity->isActive()) {
            //        out << entity->serialize();
            //    }
            //}
            //out << YAML::EndSeq;

            //std::ofstream fout(filename);
            //fout << out.c_str();
        }

        void deserializeEntities(const std::string& filename) {
            //std::ifstream fin(filename);
            //YAML::Node root = YAML::Load(fin);
            //for (const auto& node : root) {
            //    Entity& entity = addEntity();
            //    entity.deserialize(node);
            //}
        }
    };
}

