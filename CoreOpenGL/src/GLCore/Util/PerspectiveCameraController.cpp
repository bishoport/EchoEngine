#include "../../glpch.h"
#include "PerspectiveCameraController.h"
#include "../Core/Input/InputManager.h"


namespace GLCore::Utils {

    GLCore::Utils::PerspectiveCameraController::PerspectiveCameraController(float aspectRatio) : m_AspectRatio(aspectRatio), m_Camera()
    {
        m_Camera.SetProjection(45.0f, m_AspectRatio, 0.1f, 100.0f);
    }

    void GLCore::Utils::PerspectiveCameraController::OnUpdate(Timestep ts)
    {

        float cameraSpeed = m_CameraSpeed * ts;

        glm::vec3 cameraFront = m_Camera.GetFront();
        glm::vec3 cameraRight = m_Camera.GetRight();


        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_W))
            m_Camera.SetPosition(m_Camera.GetPosition() + cameraFront * cameraSpeed);
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_S))
            m_Camera.SetPosition(m_Camera.GetPosition() - cameraFront * cameraSpeed);
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A))
            m_Camera.SetPosition(m_Camera.GetPosition() - cameraRight * cameraSpeed);
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D))
            m_Camera.SetPosition(m_Camera.GetPosition() + cameraRight * cameraSpeed);

        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_R))
            m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed);
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_F))
            m_Camera.SetPosition(m_Camera.GetPosition() - glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed);





        // Revisar si la tecla de barra espaciadora está presionada
        bool isSpacebarPressed = InputManager::Instance().IsKeyPressed(GLFW_KEY_SPACE);

        float mx, my;
        std::tie(mx, my) = InputManager::Instance().GetMousePosition();

        // Revisar si el mouse está moviéndose
        mouseIsMoving = glm::distance(lastMousePosition, glm::vec2(mx, my)) > 0.0f;

        if (isSpacebarPressed && !wasSpaceBarPressedLastFrame) {
            // Si la barra espaciadora ha sido presionada y no estaba presionada en el frame anterior
            // actualizamos la última posición del mouse y marcamos que el mouse está siendo usado por primera vez
            lastMousePosition = { mx, my };
            //m_FirstMouse = true;
        }
        // Actualizar la variable para la próxima iteración
        wasSpaceBarPressedLastFrame = isSpacebarPressed;


        // Si la barra espaciadora y el mouse están presionados/moviéndose, actualizamos la rotación de la cámara
        if (isSpacebarPressed && mouseIsMoving)
        {
            //if (m_FirstMouse)
            //{
            //    m_FirstMouse = false;

            //}
            //else
            //{
                float xOffset = mx - lastMousePosition.x;
                float yOffset = lastMousePosition.y - my;

                xOffset *= m_MouseSensitivity;
                yOffset *= m_MouseSensitivity;

                m_Camera.SetRotation(m_Camera.GetRotation() + glm::vec2(yOffset, xOffset));

                lastMousePosition = { mx, my };
            //}
        }

        
    }
} // namespace GLCore::Utils