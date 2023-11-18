#pragma once

#include "../src/glpch.h"
#include "../src/GLCore/DataStruct.h"

#include "Entity.h"
#include "Component.h"
#include "Transform.h"



namespace ECS {

    class Transform : public Component {

    public:
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        glm::mat4 m_modelMatrix;
        bool m_isDirty;

        Entity* parent = nullptr;
        std::vector<Entity*> children;

        Transform();

        glm::mat4 getLocalModelMatrix() const;
        void SetTransform(const glm::mat4& transformMatrix);
        void setParent(Entity* newParent);
        bool isEntityChildOf(const Entity* potentialChild, const Entity* potentialParent);
        glm::vec3 GetEuler() const;
        void computeModelMatrix();
        glm::mat4 computeGlobalModelMatrix() const;
        void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix);
        void setLocalPosition(const glm::vec3& newPosition);
        void setLocalScale(const glm::vec3& newScale);
        const glm::vec3& getGlobalPosition() const;
        const glm::vec3& getLocalPosition() const;
        const glm::vec3& getLocalScale() const;
        const glm::mat4& getModelMatrix() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;
        glm::vec3 getBackward() const;
        glm::vec3 getForward() const;
        glm::vec3 getGlobalScale() const;
        bool isDirty() const;
        void drawGUI_Inspector() override;
        ComponentID getComponentID() const override;

        void serialize(YAML::Emitter& out) const;
        void deserialize(const YAML::Node& node);
    };
}
