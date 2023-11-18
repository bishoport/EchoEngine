#include "Entity.h"
#include "Transform.h"




namespace ECS
{
    Entity::Entity(int nextID) : id(nextID) {}

    int Entity::getID() const {
        return id;
    }

    void Entity::update(GLCore::Timestep timestep) {
        for (auto& c : components) c->update(timestep);
    }

    void Entity::draw() {
        for (auto& c : components) c->draw();
    }

    void Entity::drawGUI_Inspector() {
        for (auto& c : components) c->drawGUI_Inspector();
    }

    const std::vector<std::unique_ptr<Component>>& Entity::getComponents() const {
        return components;
    }

    bool Entity::isActive() const {
        return active;
    }

    void Entity::destroy() {
        active = false;
    }


    void Entity::removeAllComponents() {
        while (!components.empty()) {
            components.back()->onDestroy(); // Llama a onDestroy antes de eliminar el componente
            components.pop_back(); // Elimina el �ltimo componente, llama al destructor
        }
        componentArray.fill(nullptr); // Resetear el array de componentes a nullptrs
        componentBitSet.reset(); // Resetear el bitset a 0
    }


    void Entity::addComponentByPointer(Component* component) {
        if (component == nullptr) {
            throw std::invalid_argument("Null component pointer passed to addComponentByPointer.");
        }

        auto componentID = getNewComponentTypeID();

        // A�adir el nuevo componente y tomar propiedad con unique_ptr.
        std::unique_ptr<Component> uPtr{ component };
        components.emplace_back(std::move(uPtr));
        componentArray[componentID] = components.back().get();
        componentBitSet[componentID] = true;
        component->setComponentID(componentID);
        // Inicializar el componente y establecer su entidad propietaria.
        component->entity = this;
        component->init();
    }

	//SERIALIZATION
    YAML::Node Entity::serialize() const {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "id" << YAML::Value << id;
		out << YAML::Key << "name" << YAML::Value << name;
		out << YAML::Key << "active" << YAML::Value << active;

		// Asumiendo que cada entidad tiene un componente Transform
		if (Entity::hascomponent<Transform>()) {
			out << YAML::Key << "Transform";
			getComponent<Transform>().serialize(out); // Necesitamos pasar el Emitter por referencia
		}

		//if (hascomponent<MeshFilter>()) {
		//	out << YAML::Key << "MeshFilter";
		//	getComponent<MeshFilter>().serialize(out); // Necesitamos pasar el Emitter por referencia
		//}

		//if (hascomponent<MeshRenderer>()) {
		//	out << YAML::Key << "MeshRenderer";
		//	getComponent<MeshRenderer>().serialize(out); // Necesitamos pasar el Emitter por referencia
		//}

		//if (hascomponent<Material>()) {
		//	out << YAML::Key << "Material";
		//	getComponent<Material>().serialize(out); // Necesitamos pasar el Emitter por referencia
		//}

		out << YAML::EndMap;

		return YAML::Load(out.c_str());
	}
	void Entity::deserialize(const YAML::Node& node) {
		id = node["id"].as<int>();
		name = node["name"].as<std::string>();
		active = node["active"].as<bool>();

		// Deserializar el componente Transform si existe
		if (node["transform"]) {
			// Aqu� necesitas asegurarte de que el componente Transform ya ha sido a�adido a la entidad
			// antes de intentar deserializarlo. Si no, necesitas crear uno nuevo.
			if (!hascomponent<Transform>()) {
				addComponent<Transform>();
			}
			getComponent<Transform>().deserialize(node["transform"]);
		}
	}

}


//void Entity::addComponentByPointer(Component* component) {
    //    if (component == nullptr) {
    //        throw std::invalid_argument("Null component pointer passed to addComponentByPointer.");
    //    }

    //    auto componentID = component->getComponentID(); // El componente ya debe tener un ID asignado.

    //    if (componentID >= maxComponents) {
    //        throw std::out_of_range("Component type ID out of range in addComponentByPointer.");
    //    }

    //    // Si ya hay un componente del mismo tipo, decide qu� acci�n tomar.
    //    if (componentBitSet[componentID]) {
    //        // Aqu� puedes decidir si sobrescribir el componente existente, 
    //        // lanzar una excepci�n o simplemente ignorar la adici�n.
    //        // Por ejemplo:
    //        throw std::logic_error("Component of this type already exists in the entity.");
    //        // Pero si quieres sobrescribirlo, necesitas primero eliminar el componente actual.
    //        //removeComponentByTypeID(componentID);
    //    }

    //    // A�adir el nuevo componente y tomar propiedad con unique_ptr.
    //    std::unique_ptr<Component> uPtr{ component };
    //    components.emplace_back(std::move(uPtr));
    //    componentArray[componentID] = component; // Usa el ID que el componente ya tiene.
    //    componentBitSet[componentID] = true; // Marca el bit correspondiente.

    //    // Inicializar el componente y establecer su entidad propietaria.
    //    component->entity = this; // Establece la entidad propietaria del componente.
    //    component->init();

    //}