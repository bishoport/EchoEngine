#include "Transform.h"

#include "Entity.h"

namespace ECS {

    Transform::Transform()
        : position(glm::vec3(0.0f)), rotation(glm::quat()), scale(glm::vec3(1.0f)),
        m_modelMatrix(glm::mat4(1.0f)), m_isDirty(true) {}


    glm::mat4 Transform::getLocalModelMatrix() const {
        return glm::translate(glm::mat4(1.0f), position)
            * glm::toMat4(rotation)
            * glm::scale(glm::mat4(1.0f), scale);
    }

    void Transform::SetTransform(const glm::mat4& transformMatrix) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transformMatrix, scale, rotation, position, skew, perspective);
        rotation = glm::conjugate(rotation);
    }

    void Transform::setParent(Entity* newParent) {
        // Eliminar esta entidad de la lista de hijos del padre actual, si existe
        if (parent) {
            auto& siblings = parent->getComponent<Transform>().children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this->entity), siblings.end());
        }

        // Si el nuevo padre es diferente al actual, actualizar el padre y añadir a la lista de hijos
        if (newParent != parent) {
            parent = newParent;
            if (newParent) {
                newParent->getComponent<Transform>().children.push_back(this->entity);
            }
        }

        m_isDirty = true; // Marcar para recalcular la matriz del modelo
    }

    bool Transform::isEntityChildOf(const Entity* potentialChild, const Entity* potentialParent) {
        //if (potentialChild == nullptr || potentialParent == nullptr) {
        //    return false; // Si alguna de las entidades es nula, no pueden tener una relación padre-hijo.
        //}

        //// Obten el componente Transform del potencial hijo.
        //const Transform* childTransform = potentialChild->getComponent<Transform>();
        //while (childTransform != nullptr) { // Mientras haya un componente padre para verificar...
        //    if (childTransform->parent == potentialParent) {
        //        return true; // El potencial padre es un padre del hijo.
        //    }
        //    childTransform = childTransform->parent ? childTransform->parent->getComponent<Transform>() : nullptr;
        //}

        return false; // No se encontró el potencial padre en la jerarquía.
    }

    glm::vec3 Transform::GetEuler() const {
        return glm::eulerAngles(rotation);
    }

    void Transform::computeModelMatrix() {
        m_modelMatrix = getLocalModelMatrix();
        m_isDirty = false;
    }

    glm::mat4 Transform::computeGlobalModelMatrix() const {
        if (parent != nullptr) {
            return parent->getComponent<ECS::Transform>().computeGlobalModelMatrix() * getLocalModelMatrix();
        }
        else {
            return getLocalModelMatrix();
        }
    }

    void Transform::computeModelMatrix(const glm::mat4& parentGlobalModelMatrix) {
        m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
        m_isDirty = false;
    }

    void Transform::setLocalPosition(const glm::vec3& newPosition) {
        position = newPosition;
        m_isDirty = true;
    }

    void Transform::setLocalScale(const glm::vec3& newScale) {
        scale = newScale;
        m_isDirty = true;
    }

    const glm::vec3& Transform::getGlobalPosition() const {
        return m_modelMatrix[3];
    }

    const glm::vec3& Transform::getLocalPosition() const {
        return position;
    }

    const glm::vec3& Transform::getLocalScale() const {
        return scale;
    }

    const glm::mat4& Transform::getModelMatrix() const {
        return m_modelMatrix;
    }

    glm::vec3 Transform::getRight() const {
        return m_modelMatrix[0];
    }

    glm::vec3 Transform::getUp() const {
        return m_modelMatrix[1];
    }

    glm::vec3 Transform::getBackward() const {
        return m_modelMatrix[2];
    }

    glm::vec3 Transform::getForward() const {
        return -m_modelMatrix[2];
    }

    glm::vec3 Transform::getGlobalScale() const {
        return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) };
    }

    bool Transform::isDirty() const {
        return m_isDirty;
    }

    void Transform::drawGUI_Inspector() {
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Entity ID: %i", entity->getID());
        ImGui::Separator();

        ImGui::Text("Transform");
        ImGui::Text("Component ID: %i", getComponentID());
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);
        glm::vec3 eulers = glm::degrees(glm::eulerAngles(rotation));
        if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulers), 0.01f)) {
            rotation = glm::quat(glm::radians(eulers));
        }
        ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    ComponentID Transform::getComponentID() const
    {
        return getComponentTypeID<Transform>();
    }



    void Transform::serialize(YAML::Emitter& out) const {
        out << YAML::BeginMap;
        out << YAML::Key << "position" << YAML::Value << position;
        out << YAML::Key << "rotation" << YAML::Value << rotation;
        out << YAML::Key << "scale" << YAML::Value << scale;
        out << YAML::EndMap;
    }

    void Transform::deserialize(const YAML::Node& node) {
        position = node["position"].as<glm::vec3>();
        rotation = node["rotation"].as<glm::quat>();
        scale = node["scale"].as<glm::vec3>();
    }
}
