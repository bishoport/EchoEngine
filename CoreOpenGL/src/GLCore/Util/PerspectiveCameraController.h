#pragma once

#include "PerspectiveCamera.h"

#include "../Core/Timestep.h"



namespace GLCore::Utils {

    class PerspectiveCameraController
    {
    public:
        PerspectiveCameraController(float aspectRatio);

        void OnUpdate(Timestep ts);

        PerspectiveCamera& GetCamera() { return m_Camera; }
        const PerspectiveCamera& GetCamera() const { return m_Camera; }


    private:
        PerspectiveCamera m_Camera;
        float m_AspectRatio;

        float m_MouseSensitivity = 0.3f;
        float m_CameraSpeed = 10.5f;  // Añadir esto

        glm::vec2 lastMousePosition = { 0.0f, 0.0f };
        bool m_FirstMouse = true;

        bool mouseIsMoving = false;
        bool wasSpaceBarPressedLastFrame = false;
    };
}
