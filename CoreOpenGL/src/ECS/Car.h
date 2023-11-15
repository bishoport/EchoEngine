#pragma once
#include "../glpch.h"
#include "../GLCore/DataStruct.h"
#include "Entity.h"
#include "Transform.h"
#include "../GLCore/Core/Input/InputManager.h"


namespace ECS
{
    class Car : public ECS::Component
    {
    public:


        glm::vec3 forward = glm::vec3(0.0f,0.0f,-1.0f);   // La direcci�n hacia la que el coche est� mirando
        float speed = 0.0f;         // Velocidad actual del coche
        float maxSpeed = 10.0f;      // Velocidad m�xima
        float rotation = 0.0f;      // Rotaci�n actual del coche en radianes
        float turnRate = 1.5f;      // Qu� tan r�pido gira el coche
        glm::vec3 rotationOffset = glm::vec3(0.0f, 0.0f, -0.5f / 2.0f); // Desplazamiento del centro de rotaci�n respecto al centro del coche

        void init() override 
        {
            // Configuraci�n del VAO y VBO para el punto de debug
            glGenVertexArrays(1, &debugVAO);
            glGenBuffers(1, &debugVBO);
            glBindVertexArray(debugVAO);
            glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW); // Inicializar con NULL ya que la posici�n se actualizar� cada frame
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glBindVertexArray(0); // Desvincular para evitar cambios accidentales en otros lugares del c�digo
        }

        ComponentID getTypeID() const
        {
            return getComponentTypeID<Car>();
        }

        void update(GLCore::Timestep deltaTime) override {
            ECS::Transform& transform = entity->getComponent<ECS::Transform>();

            // Agregar inercia al movimiento
            float acceleration = 0.0f;
            if (InputManager::Instance().IsKeyPressed(GLFW_KEY_I)) {
                acceleration = 20.0f; // Acelerar hacia adelante
            }
            else if (InputManager::Instance().IsKeyPressed(GLFW_KEY_K)) {
                acceleration = -20.0f; // Desacelerar o ir en reversa
            }
            else {
                // Aplicar una desaceleraci�n natural si no se presiona ninguna tecla
                acceleration = -glm::sign(speed) * 10.0f;
            }
            speed += acceleration * deltaTime.GetSeconds();
            speed = glm::clamp(speed, -maxSpeed, maxSpeed);

            // Rotaci�n basada en la velocidad actual
            float turnVelocity = 0.0f; // Velocidad de giro actual
            if (speed != 0) {
                if (InputManager::Instance().IsKeyPressed(GLFW_KEY_J)) {
                    turnVelocity = turnRate * (speed / maxSpeed); // Girar a la izquierda
                }
                else if (InputManager::Instance().IsKeyPressed(GLFW_KEY_L)) {
                    turnVelocity = -turnRate * (speed / maxSpeed); // Girar a la derecha
                }
            }
            float turnAngle = turnVelocity * deltaTime.GetSeconds();

            // Aplicar rotaci�n
            if (turnAngle != 0.0f) {
                glm::quat deltaRotation = glm::angleAxis(turnAngle, glm::vec3(0, 1, 0));
                transform.rotation = transform.rotation * deltaRotation;
            }

            // Calcular la direcci�n hacia adelante basada en la rotaci�n actual del Transform
            glm::vec3 forwardDir = transform.getForward();
            // Mover el coche basado en la velocidad y la direcci�n hacia adelante
            transform.position += forwardDir * speed * deltaTime.GetSeconds();

            // Aseg�rate de que la matriz del modelo se recalcula si es necesario
            transform.computeModelMatrix();
        }



        void draw()
        {
            // Asumiendo que la rotaci�n del coche es relativa a su centro, el "rotationCenter" ser�a simplemente la "position" del coche.
            rotationCenter = entity->getComponent<ECS::Transform>().position + rotationOffset; // Suma el desplazamiento para obtener el centro de rotaci�n
            glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), &rotationCenter);

            // Obt�n la matriz de modelo del transform para pasarla al shader
            glm::mat4 model = entity->getComponent<ECS::Transform>().getLocalModelMatrix();

            // Usar el shader de debug

            GLCore::Render::ShaderManager::Get("debug")->use();
            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", model);
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Usamos rojo para el punto de debug

            // Dibujar el punto
            glPointSize(20.0f); // Puedes ajustar este valor seg�n sea necesario
            glBindVertexArray(debugVAO);
            glDrawArrays(GL_POINTS, 0, 1);
            glBindVertexArray(0); // Desvincular el VAO despu�s de usarlo
            glPointSize(1.0f); // Puedes ajustar este valor seg�n sea necesario
        }


        void drawGUI_Inspector() override
        {
            ImGui::Text("CarController");
            ImGui::SliderFloat("Speed", &speed, -maxSpeed/2.0f, maxSpeed); // El slider controlar� speed de 0 a maxSpeed

            // Control para la velocidad m�xima
            ImGui::SliderFloat("Max Speed", &maxSpeed, -100.0f, 100.0f); // Asumimos que 100 es un l�mite superior razonable para maxSpeed

            // Control de rotaci�n
            // La rotaci�n est� en radianes, por lo que podr�as querer mostrarla en grados para que sea m�s intuitivo
            float rotationDegrees = glm::degrees(rotation); // Convierte la rotaci�n a grados para la visualizaci�n
            ImGui::SliderFloat("Rotation (Degrees)", &rotationDegrees, -180.0f, 180.0f); // Los grados van de -180 a 180
            rotation = glm::radians(rotationDegrees); // Convierte de nuevo a radianes para el c�lculo

            // Control para la tasa de giro
            ImGui::SliderFloat("Turn Rate", &turnRate, 0.0f, 10.0f); // Suponemos que 10 es un l�mite superior razonable para turnRate
            ImGui::SliderFloat3("Rotation Offset", glm::value_ptr(rotationOffset), -100.0f, 100.0f, "%.5f");
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }


    private:
        unsigned int debugVAO, debugVBO;
        glm::vec3 rotationCenter; // Aseg�rate de actualizar esto con el centro de rotaci�n actual
    };
} // namespace ECS