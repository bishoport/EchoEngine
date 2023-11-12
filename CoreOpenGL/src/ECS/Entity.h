#pragma once

#include "../glpch.h"
#include "Component.h"



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

    template<>
    struct convert<glm::ivec2> {
        static Node encode(const glm::ivec2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::ivec2& rhs) {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }
            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
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

    // Sobrecarga del operador << para glm::ivec2
    inline Emitter& operator<<(Emitter& out, const glm::ivec2& v) {
        out << Flow;
        out << BeginSeq << v.x << v.y << EndSeq;
        return out;
    }

} // Fin del espacio de nombres YAML

namespace ECS
{
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
    using ComponentArray = std::array<ECS::Component*, maxComponents>;




    class Entity {
    private:
        int id;
        std::vector<std::unique_ptr<ECS::Component>> components;
        ComponentArray componentArray;
        ComponentBitSet componentBitSet;

    public:
        std::string name;
        bool active = true;
        bool markedToDelete = false;

        Entity(int nextID);
        int getID() const;


        void update();
        void draw();
        void drawGUI_Inspector();
        const std::vector<std::unique_ptr<Component>>& getComponents() const;
        bool isActive() const;
        void destroy();
        void removeAllComponents();

        // Declaración de métodos que dependen de Transform
		YAML::Node serialize() const;
		void deserialize(const YAML::Node& node);


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

        

    };



}