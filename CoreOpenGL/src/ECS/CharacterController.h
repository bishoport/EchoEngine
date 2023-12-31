#pragma once
#include "../glpch.h"
#include "../GLCore/DataStruct.h"
#include "Entity.h"
#include "Transform.h"
#include "../GLCore/Core/Input/InputManager.h"


namespace ECS
{
    class CharacterController : public ECS::Component
    {
    public:
        glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };
        float speed = 5.0f;


        ComponentID getTypeID() const
        {
            return getComponentTypeID<CharacterController>();
        }


        void update(GLCore::Timestep timestamp) override {
            if (InputManager::Instance().IsKeyPressed(GLFW_KEY_W))
                velocity.z -= 1.0f;
            if (InputManager::Instance().IsKeyPressed(GLFW_KEY_S))
                velocity.z += 1.0f;
            if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A))
                velocity.x -= 1.0f;
            if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D))
                velocity.x += 1.0f;
            entity->getComponent<ECS::Transform>().position += velocity * speed;
        }
    };
} // namespace ECS
