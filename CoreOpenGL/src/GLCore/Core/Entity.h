#pragma once

#include "UUID.h"
#include "Components.h"
#include "Scene.h"

#include <entt.hpp>
namespace GLCore {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			T& component = RegistrySingleton::getRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = RegistrySingleton::getRegistry().emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			return RegistrySingleton::getRegistry().get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return RegistrySingleton::getRegistry().has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			RegistrySingleton::getRegistry().remove<T>(m_EntityHandle);
		}

		void RemoveAllComponents() {
			auto componentTypes = GLCore::GetAllComponentTypes();

			std::apply([this](auto... component) {
				// Expansión del paquete para cada tipo de componente
				([this] {
					using ComponentType = std::decay_t<decltype(component)>;
			if (HasComponent<ComponentType>()) {
				RemoveComponent<ComponentType>();
			}
					}(), ...); // La coma al final es un operador de secuencia para la expansión del paquete.
				}, componentTypes);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}