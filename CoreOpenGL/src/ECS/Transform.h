#pragma once


//#include "ECS.h"
#include "Component.h"
namespace ECS
{
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
}