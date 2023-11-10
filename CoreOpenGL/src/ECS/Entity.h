#pragma once
#include <vector>
#include <bitset>
#include <array>
#include <memory>
#include <string>

//#include "ECS.h"
#include "Component.h"
#include "Transform.h"


namespace ECS {

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

    class Entity {

    private:
        int id;             // ID de la entidad
        std::vector<std::unique_ptr<Component>> components;
        ComponentArray componentArray;
        ComponentBitSet componentBitSet;
    public:
        
	public:
		std::string name;
		bool active = true;

		bool markedToDelete = false;

		// Declaración de métodos que dependen de Transform
		YAML::Node serialize() const;
		void deserialize(const YAML::Node& node);


		Entity(int nextID);

		int getID() const;

		void update();

		void draw();

		void drawGUI_Inspector();

		const std::vector<std::unique_ptr<Component>>& getComponents() const;

		bool isActive() const { return active; }
		void destroy() { active = false; }

		void removeAllComponents();

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

        //----------------SERIALIZATION-------------------------------------
        YAML::Node serialize() const;
        void deserialize(const YAML::Node& node);
        // ...
    };
}

