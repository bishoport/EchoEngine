#pragma once

#include "../glpch.h"
#include "../GLCore/DataStruct.h"

#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep


namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::quat> {
		static Node encode(const glm::quat& rhs) {
			Node node;
			node.push_back(rhs.w);
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs) {
			if (!node.IsSequence() || node.size() != 4) {
				return false;
			}
			rhs.w = node[0].as<float>();
			rhs.x = node[1].as<float>();
			rhs.y = node[2].as<float>();
			rhs.z = node[3].as<float>();
			return true;
		}
	};

	inline Emitter& operator<<(Emitter& out, const glm::vec3& v) {
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << EndSeq;
		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::quat& q) {
		out << Flow;
		out << BeginSeq << q.w << q.x << q.y << q.z << EndSeq;
		return out;
	}
} // Fin del espacio de nombres YAML




namespace ECS
{
	class Component;
	class Entity;
	class Transform; // Forward declaration

	using ComponentID = std::size_t;

	inline ComponentID getComponentTypeID()
	{
		static ComponentID lastID = 0;
		return lastID++;
	}

	template <typename T> inline ComponentID getComponentTypeID() noexcept
	{
		static ComponentID typeID = getComponentTypeID();
		return typeID;
	}

	constexpr std::size_t maxComponents = 32;

	using ComponentBitSet = std::bitset<maxComponents>;
	using ComponentArray = std::array<Component*, maxComponents>;


	class Component
	{
	public:
		Entity* entity;
		virtual void init() {}
		virtual void update() {}
		virtual void draw() {}
		virtual void drawGUI_Inspector() {}
		virtual void onDestroy() {}
		~Component() { onDestroy(); }
	};
	

	class Entity
	{
	private:
		int id;             // ID de la entidad
		std::vector<std::unique_ptr<Component>> components;
		ComponentArray componentArray;
		ComponentBitSet componentBitSet;

	public:
		std::string name;
		bool active = true;

		bool markedToDelete = false;

		// Declaración de métodos que dependen de Transform
		YAML::Node serialize() const;
		void deserialize(const YAML::Node& node);


		Entity(int nextID) {
			id = nextID;
		}

		int getID() const {
			return id;
		}

		void update()
		{
			for (auto& c : components) c->update();
		}

		void draw()
		{
			for (auto& c : components) c->draw();
		}

		void drawGUI_Inspector()
		{
			for (auto& c : components) c->drawGUI_Inspector();
		}

		const std::vector<std::unique_ptr<Component>>& getComponents() const {
			return components;
		}



		bool isActive() const { return active; }
		void destroy() { active = false; }

		template <typename T> bool hascomponent() const
		{
			if (&getComponent<T>() != NULL)
			{
				return true;
			}
			return false;
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

		void removeAllComponents() {
			while (!components.empty()) {
				components.back()->onDestroy(); // Llama a onDestroy antes de eliminar el componente
				components.pop_back(); // Elimina el último componente, llama al destructor
			}
			componentArray.fill(nullptr); // Resetear el array de componentes a nullptrs
			componentBitSet.reset(); // Resetear el bitset a 0
		}

		template<typename T> void removeComponent()
		{
			// Comprobar si el componente existe
			auto typeID = getComponentTypeID<T>();
			if (!componentBitSet[typeID]) {
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
			componentArray[typeID] = nullptr;
			componentBitSet[typeID] = false;
		}
	};


	class Transform : public Component
	{
	public:
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation;
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		//Global space information concatenate in matrix
		glm::mat4 m_modelMatrix = glm::mat4(1.0f);

		//Dirty flag
		bool m_isDirty = true;

		Entity* parent = nullptr;
		std::vector<Entity*> children;

		glm::mat4 getLocalModelMatrix() const
		{
			return glm::translate(glm::mat4(1.0f), position)
				* glm::toMat4(rotation)
				* glm::scale(glm::mat4(1.0f), scale);
		}


		void SetTransform(const glm::mat4& transformMatrix)
		{
			// Descomponer la matriz de transformación
			glm::vec3 skew;
			glm::vec4 perspective;

			glm::decompose(transformMatrix, scale, rotation, position, skew, perspective);
			rotation = glm::conjugate(rotation);
		}


		// Función para obtener los ángulos de Euler
		glm::vec3 GetEuler() const
		{
			return glm::eulerAngles(rotation);
		}

		void computeModelMatrix()
		{
			m_modelMatrix = getLocalModelMatrix();
			m_isDirty = false;
		}


		glm::mat4 computeGlobalModelMatrix() const {
			if (parent != nullptr) {
				// Si hay un padre, pre-multiplicamos la matriz de modelo local por la del padre
				return parent->getComponent<Transform>().computeGlobalModelMatrix() * getLocalModelMatrix();
			}
			else {
				// Si no hay un padre, la matriz global es simplemente la local
				return getLocalModelMatrix();
			}
		}


		void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
		{
			m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
			m_isDirty = false;
		}

		void setLocalPosition(const glm::vec3& newPosition)
		{
			position = newPosition;
			m_isDirty = true;
		}

		void setLocalScale(const glm::vec3& newScale)
		{
			scale = newScale;
			m_isDirty = true;
		}

		const glm::vec3& getGlobalPosition() const
		{
			return m_modelMatrix[3];
		}

		const glm::vec3& getLocalPosition() const
		{
			return position;
		}

		const glm::vec3& getLocalScale() const
		{
			return scale;
		}

		const glm::mat4& getModelMatrix() const
		{
			return m_modelMatrix;
		}

		glm::vec3 getRight() const
		{
			return m_modelMatrix[0];
		}

		glm::vec3 getUp() const
		{
			return m_modelMatrix[1];
		}

		glm::vec3 getBackward() const
		{
			return m_modelMatrix[2];
		}

		glm::vec3 getForward() const
		{
			return -m_modelMatrix[2];
		}

		glm::vec3 getGlobalScale() const
		{
			return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) };
		}

		bool isDirty() const
		{
			return m_isDirty;
		}

		void drawGUI_Inspector() override
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Text("Entity ID: %i", entity->getID());
			ImGui::Separator();

			ImGui::Text("Transform");
			ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);
			glm::vec3 eulers = glm::degrees(glm::eulerAngles(rotation));
			if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulers), 0.01f)) {
				rotation = glm::quat(glm::radians(eulers));
			}
			ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		}

		void serialize(YAML::Emitter& out) const {
			out << YAML::BeginMap;
			out << YAML::Key << "position" << YAML::Value << position;
			out << YAML::Key << "rotation" << YAML::Value << rotation;
			out << YAML::Key << "scale" << YAML::Value << scale;
			out << YAML::EndMap;
		}

		void deserialize(const YAML::Node& node) {
			position = node["position"].as<glm::vec3>();
			rotation = node["rotation"].as<glm::quat>();
			scale = node["scale"].as<glm::vec3>();
		}
	};

	

	//#include "ProjectManager.h"

	inline YAML::Node Entity::serialize() const {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "id" << YAML::Value << id;
		out << YAML::Key << "name" << YAML::Value << name;
		out << YAML::Key << "active" << YAML::Value << active;

		// Asumiendo que cada entidad tiene un componente Transform
		if (hascomponent<Transform>()) {
			out << YAML::Key << "transform";
			getComponent<Transform>().serialize(out); // Necesitamos pasar el Emitter por referencia
		}

		out << YAML::EndMap;

		// El Emitter de YAML convierte todo a una cadena,
		// así que necesitamos convertir esa cadena a un Node para devolverlo.
		return YAML::Load(out.c_str());
	}

	inline void Entity::deserialize(const YAML::Node& node) {
		id = node["id"].as<int>();
		name = node["name"].as<std::string>();
		active = node["active"].as<bool>();

		// Deserializar el componente Transform si existe
		if (node["transform"]) {
			// Aquí necesitas asegurarte de que el componente Transform ya ha sido añadido a la entidad
			// antes de intentar deserializarlo. Si no, necesitas crear uno nuevo.
			if (!hascomponent<Transform>()) {
				addComponent<Transform>();
			}
			getComponent<Transform>().deserialize(node["transform"]);
		}
	}


	



	class Manager
	{
	private:
		std::vector<std::unique_ptr<Entity>> entities;
		int nextID = 0;// Contador para generar IDs únicos
	public:
		void update()
		{
			for (auto& e : entities) e->update();
		}

		void draw()
		{
			for (auto& e : entities) e->draw();
		}

		void drawGUI_Inspector()
		{
			for (auto& e : entities) e->drawGUI_Inspector();
		}

		void refresh()
		{
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

		Entity& addEntity()
		{
			Entity* e = new Entity(nextID);
			nextID++;
			std::unique_ptr<Entity> uPtr{ e };
			entities.emplace_back(std::move(uPtr));

			float pi = 3.1415926535f;

			e->addComponent<Transform>();
			e->getComponent<Transform>().position = { 0.0f, 0.0f, 0.0f };
			e->getComponent<Transform>().scale = { 1.0f, 1.0f, 1.0f };

			// Inicializa el cuaternión de rotación con ángulos de Euler
			glm::vec3 eulers = { (0 * pi) / 180, (0 * pi) / 180, (0 * pi) / 180 };
			e->getComponent<Transform>().rotation = glm::quat(eulers);

			return *e;
		}

		void removeEntity(Entity* e, bool isRootCall = true) {
			if (!e) return; // Si el puntero a la entidad es nulo, no hacer nada.

			// Desactivar temporalmente la entidad
			e->active = false;

			// Intenta encontrar el componente Transform sin eliminar los componentes todavía.
			Transform* transform = nullptr;
			try {
				transform = &(e->getComponent<Transform>());
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


		// Devolver un vector de punteros crudos a las entidades
		std::vector<Entity*> getAllEntities()
		{
			std::vector<Entity*> rawEntities;
			for (auto& e : entities)
			{
				if (e.get()->active == true)
					rawEntities.push_back(e.get());
			}
			return rawEntities;
		}

		void serializeEntities(const std::string& filename) {
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

		void deserializeEntities(const std::string& filename) {
			std::ifstream fin(filename);
			YAML::Node root = YAML::Load(fin);
			for (const auto& node : root) {
				Entity& entity = addEntity();
				entity.deserialize(node);
			}
		}
	};
}





