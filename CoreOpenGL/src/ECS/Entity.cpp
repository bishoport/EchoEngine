#include "Entity.h"

#include "MeshFilter.h"

namespace ECS {

	Entity::Entity(int nextID) {
	 	id = nextID;
	}

	int Entity::getID() const {
	 	return id;
	}

	void Entity::update()
	{
	 	for (auto& c : components) c->update();
	}

	void Entity::draw()
	{
	 	for (auto& c : components) c->draw();
	}

	void Entity::drawGUI_Inspector()
	{
	 	for (auto& c : components) c->drawGUI_Inspector();
	}

	const std::vector<std::unique_ptr<Component>>& Entity::getComponents() const {
	 	return components;
	}

	void Entity::removeAllComponents() {
		while (!components.empty()) {
			components.back()->onDestroy(); // Llama a onDestroy antes de eliminar el componente
			components.pop_back(); // Elimina el último componente, llama al destructor
		}
		componentArray.fill(nullptr); // Resetear el array de componentes a nullptrs
		componentBitSet.reset(); // Resetear el bitset a 0
	}


	YAML::Node ECS::Entity::serialize() const {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "id" << YAML::Value << id;
		out << YAML::Key << "name" << YAML::Value << name;
		out << YAML::Key << "active" << YAML::Value << active;

		// Asumiendo que cada entidad tiene un componente Transform
		if (hascomponent<ECS::Transform>()) {
			out << YAML::Key << "transform";
			getComponent<ECS::Transform>().serialize(out); // Necesitamos pasar el Emitter por referencia
		}

		if (hascomponent<ECS::MeshFilter>()) {
			out << YAML::Key << "meshFilter";
			getComponent<ECS::MeshFilter>().serialize(out); // Serializa el componente MeshFilter
		}

		out << YAML::EndMap;

		// El Emitter de YAML convierte todo a una cadena,
		// así que necesitamos convertir esa cadena a un Node para devolverlo.
		return YAML::Load(out.c_str());
	}


	void ECS::Entity::deserialize(const YAML::Node& node) {
		id = node["id"].as<int>();
		name = node["name"].as<std::string>();
		active = node["active"].as<bool>();

		// Deserializar el componente Transform si existe
		if (node["transform"]) {
			// Aquí necesitas asegurarte de que el componente Transform ya ha sido añadido a la entidad
			// antes de intentar deserializarlo. Si no, necesitas crear uno nuevo.
			if (!hascomponent<ECS::Transform>()) {
				addComponent<ECS::Transform>();
			}
			getComponent<ECS::Transform>().deserialize(node["transform"]);
		}

		if (node["meshFilter"]) {
			// Asegúrate de que el componente MeshFilter ya ha sido añadido a la entidad
			// antes de intentar deserializarlo. Si no, necesitas crear uno nuevo.
			if (!hascomponent<ECS::MeshFilter>()) {
				addComponent<ECS::MeshFilter>();
			}
			getComponent<ECS::MeshFilter>().deserialize(node["meshFilter"]);
		}
	}
}
