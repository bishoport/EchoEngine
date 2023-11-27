#pragma once

#include "Entity.h"

namespace GLCore {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(GLCore::Scene* context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Entity m_SelectionContext;
		std::vector<Entity> m_EntitiesToBeDeleted;
		void HandleDragDropForTexture(Ref<Texture> texture, MaterialComponent& materialComponent, const char* payloadType);
	};

}
