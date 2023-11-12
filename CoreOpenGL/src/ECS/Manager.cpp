#include "Manager.h"
#include "Transform.h"

namespace ECS {

    void Manager::update()
    {
        for (auto& e : entities) e->update();
    }

    void Manager::draw() {
        for (auto& e : entities) e->draw();
    }

    void Manager::drawGUI_Inspector() {
        for (auto& e : entities) e->drawGUI_Inspector();
    }

    void Manager::refresh() {
        for (size_t i = 0; i < entities.size();)
        {
            if (entities[i]->markedToDelete) {
                removeEntity(entities[i].get());
            }
            else {
                ++i;
            }
        }
    }

    ECS::Entity& Manager::addEntity()
    {
        ECS::Entity* e = new ECS::Entity(nextID);
        nextID++;
        std::unique_ptr<ECS::Entity> uPtr{ e };
        entities.emplace_back(std::move(uPtr));
        return *e;
    }

    void Manager::removeEntity(ECS::Entity* e, bool isRootCall) {
        if (!e) 
        {
            std::cout << "Si el puntero a la entidad es nulo, no hacer nada." << std::endl;
            return; // Si el puntero a la entidad es nulo, no hacer nada.
        }
        
        // Desactivar temporalmente la entidad
        e->active = false;
        
        // Intenta encontrar el componente Transform sin eliminar los componentes todavía.
        ECS::Transform* transform = nullptr;
        try {
        	transform = &(e->getComponent<ECS::Transform>());
        }
        catch (...) {
        	// Manejar la excepción
        }
        
        if (transform) {
        	auto childrenCopy = transform->children; // Hacer una copia de los hijos para iterar de manera segura.
        
        	for (auto it = childrenCopy.rbegin(); it != childrenCopy.rend(); ++it) {
        		removeEntity(*it, false); // Aquí es una llamada recursiva, así que no es la llamada "raíz".
        	}
        }
        
        e->removeAllComponents(); // Eliminar todos los componentes de la entidad.
        
        auto it = std::find_if(entities.begin(), entities.end(),
        	[e](const std::unique_ptr<Entity>& entity) { return entity.get() == e; });
        
        if (it != entities.end()) {
        	entities.erase(it); // Elimina la entidad del vector.
        }
    }

    std::vector<ECS::Entity*> Manager::getAllEntities() {
        std::vector<ECS::Entity*> rawEntities;
        for (auto& e : entities) {
            if (e->isActive()) rawEntities.push_back(e.get());
        }
        return rawEntities;
    }


    void Manager::serializeEntities(const std::string& filename) {
		YAML::Emitter out;
		out << YAML::BeginSeq;
		for (const auto& entity : entities) {
			if (entity->isActive()) {
				out << entity->serialize();
			}
		}
		out << YAML::EndSeq;

		std::ofstream fout(filename);
		fout << out.c_str();
	}

	void Manager::deserializeEntities(const std::string& filename) {
		std::ifstream fin(filename);
		YAML::Node root = YAML::Load(fin);
		for (const auto& node : root) {
			Entity& entity = addEntity();
			entity.deserialize(node);
		}
	}

}


