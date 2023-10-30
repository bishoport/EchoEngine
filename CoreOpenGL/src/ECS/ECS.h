#pragma once

#include "../glpch.h"
#include "../GLCore/DataStruct.h"

namespace ECS
{
	class Component;
	class Entity;

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

		Entity* parent;
		std::vector<Entity*> children;

		glm::mat4 getLocalModelMatrix() const
		{
			return glm::translate(glm::mat4(1.0f), position)
				* glm::toMat4(rotation)
				* glm::scale(glm::mat4(1.0f), scale);
		}


		void SetTransform(const glm::mat4& transformMatrix)
		{
			// Descomponer la matriz de transformaci�n
			glm::vec3 skew;
			glm::vec4 perspective;

			glm::decompose(transformMatrix, scale, rotation, position, skew, perspective);
			rotation = glm::conjugate(rotation);
		}


		// Funci�n para obtener los �ngulos de Euler
		glm::vec3 GetVec() const
		{
			return glm::eulerAngles(rotation);
		}

		void computeModelMatrix()
		{
			m_modelMatrix = getLocalModelMatrix();
			m_isDirty = false;
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

		/*void addChild(Entity* childEntity)
		{
			childEntity->getComponent<Transform>().parent = this->entity;
			children.push_back(childEntity);
		}

		void removeChild(Entity* childEntity)
		{
			childEntity->getComponent<Transform>().parent = nullptr;
			children.erase(std::remove(children.begin(), children.end(), childEntity), children.end());
		}*/

		void drawGUI_Inspector() override
		{
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

	};






	class Entity
	{
	private:
		std::vector<std::unique_ptr<Component>> components;
		ComponentArray componentArray;
		ComponentBitSet componentBitSet;

	public:
		std::string name;
		bool active = true;


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

			//return ComponentBitSet[getComponentTypeID<T>];
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


	};





	class Manager
	{
	private:
		std::vector<std::unique_ptr<Entity>> entities;
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
			entities.erase(std::remove_if(std::begin(entities), std::end(entities),
				[](const std::unique_ptr<Entity>& mEntity)
				{
					return !mEntity->isActive();
				}),
				std::end(entities));
		}

		Entity& addEntity()
		{
			Entity* e = new Entity();
			std::unique_ptr<Entity> uPtr{ e };
			entities.emplace_back(std::move(uPtr));

			float pi = 3.1415926535f;

			e->addComponent<Transform>();
			e->getComponent<Transform>().position = { 0.0f, 0.0f, 0.0f };
			e->getComponent<Transform>().scale = { 1.0f, 1.0f, 1.0f };

			// Inicializa el cuaterni�n de rotaci�n con �ngulos de Euler
			glm::vec3 eulers = { (0 * pi) / 180, (0 * pi) / 180, (0 * pi) / 180 };
			e->getComponent<Transform>().rotation = glm::quat(eulers);

			return *e;
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
	};
}





