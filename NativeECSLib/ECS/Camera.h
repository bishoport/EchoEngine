#pragma once

#include "../src/glpch.h"
#include "../src/GLCore/DataStruct.h"
#include "Entity.h"
#include "Component.h"
#include "Transform.h"


namespace ECS
{
    class Camera : public ECS::Component
    {

    public:
        glm::vec3 m_Position;
        unsigned int m_camID = 0;
        bool m_active = true;
        bool m_debug = true;

        GLuint FBO = 0;
        GLuint depthBuffer = 0;
        std::vector<GLuint> colorBuffers;

    public:

        void init() override
        {
            m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
            m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
            m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
            m_Right = glm::vec3(0.0f, 0.0f, 0.0f);
            m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            m_Rotation = glm::vec2(0.0f, -90.0f);

            m_Fov = 45.0f;
            m_AspectRatio = 16.0f / 9.0f;
            m_NearClip = 0.1f;
            m_FarClip = 100.0;

            SetProjection(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
            RecalculateViewMatrix();

            //--FBO SCENE
            colorBuffers = GLCore::Render::FBOManager::CreateFBO_Color_RGBA16F(&FBO, &depthBuffer, 1, 800, 600);
            // ---------------------------------------


            //--RESIZE SCENE PANEL EVENT
            EventManager::getOnPanelResizedEvent().subscribe([this](const std::string name, const ImVec2& size, const ImVec2& position)
            {
                if (name == "GAME")
                {
                    GLCore::Render::FBOManager::UpdateFBO_Color_RGBA16F(&FBO, &depthBuffer, colorBuffers, size.x, size.y);
                }
            });
            //---------------------------------------------------
        }

        ComponentID getComponentID() const
        {
            return getComponentTypeID<Camera>();
        }



        void setCamId(unsigned int camID)
        {
            m_camID = camID;
        }

        void SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
        {
            m_Fov = fov;
            m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        void update(GLCore::Timestep timestamp) override
        {
            // Aquí solo necesitamos asegurarnos de que la escala de la entidad es la correcta.
            // Asumiendo que esto es necesario en cada frame. Si no, esto podría ir en la función init.
            ECS::Transform& transform = entity->getComponent<ECS::Transform>();
            transform.scale = glm::vec3(1.0f);  // Esto establece la escala a 1 en todas las direcciones.

            // Obtiene la posición local de la cámara y la matriz de modelo local.
            // La posición global y la rotación se manejarán en RecalculateViewMatrix.

            model_transform_matrix = transform.getLocalModelMatrix();
            if (entity->getComponent<ECS::Transform>().parent != nullptr) {
               model_transform_matrix = entity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().getLocalModelMatrix() * model_transform_matrix;
            }

            // Recalcular la matriz de vista ahora también manejará la posición y rotación del padre.
            RecalculateViewMatrix();
        }

        //void update() override 
        //{
        //    entity->getComponent<ECS::Transform>().scale.x = 1.0f;
        //    entity->getComponent<ECS::Transform>().scale.y = 1.0f;
        //    entity->getComponent<ECS::Transform>().scale.z = 1.0f;

        //    m_Position = entity->getComponent<ECS::Transform>().getLocalPosition();

        //    model_transform_matrix = entity->getComponent<ECS::Transform>().getLocalModelMatrix();

        //    //Check if has parent
        //    if (entity->getComponent<ECS::Transform>().parent != nullptr) {
        //       model_transform_matrix = entity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().getLocalModelMatrix() * model_transform_matrix;
        //       m_Position = entity->getComponent<ECS::Transform>().parent->getComponent<ECS::Transform>().position + m_Position;
        //    }

        //    RecalculateViewMatrix();
        //}




        void draw() override
        {
            if (m_debug)
            {
                drawDebug();
            }
        }
       

        void drawGUI_Inspector() override
        {
            ImGui::Text("Camera %i", m_camID);
            ImGui::Text("Component ID: %i", getComponentID());
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Checkbox("Active", &m_active);
            ImGui::Checkbox("Debug", &m_debug);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::SliderFloat("Field of View", &m_Fov, 1.0f, 120.0f))
            {
                SetProjection(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
            }
            if (ImGui::SliderFloat("Near Clip", &m_NearClip, 0.0f, 100.0f))
            {
                SetProjection(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
            }
            if (ImGui::SliderFloat("Far Clip", &m_FarClip, 100.0f, 0.0f))
            {
                SetProjection(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
            }
            if (ImGui::SliderFloat("Aspect Ratio", &m_AspectRatio, 0.0f, 10.0f))
            {
                SetProjection(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
            }
        }

        float GetFov()
        {
            return m_Fov;
        }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }



    private:
        glm::mat4 model_transform_matrix{ glm::mat4(1.0f) };

        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        
        glm::vec3 m_Front;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;

        glm::vec2 m_Rotation;

        float m_Fov;
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;

        //Debug
        GLuint frustumVBO = 0;


    private:

        void RecalculateViewMatrix() {
            ECS::Transform& cameraTransform = entity->getComponent<ECS::Transform>();
            glm::quat cameraRotation = cameraTransform.rotation;
            glm::vec3 cameraPosition; // Inicializada más adelante

            // Usar la matriz de modelo global para obtener la posición y rotación correctas
            glm::mat4 globalModelMatrix = cameraTransform.computeGlobalModelMatrix();
            cameraPosition = glm::vec3(globalModelMatrix[3]); // Extraer la posición global de la matriz de modelo
            cameraRotation = glm::quat_cast(globalModelMatrix); // Extraer la rotación global de la matriz de modelo

            // Actualizar los vectores de dirección de la cámara usando la rotación global
            m_Front = glm::rotate(cameraRotation, glm::vec3(0, 0, -1));
            m_Right = glm::rotate(cameraRotation, glm::vec3(1, 0, 0));
            m_Up = glm::rotate(cameraRotation, glm::vec3(0, 1, 0));

            // Utilizar glm::lookAt para crear la matriz de vista con los nuevos vectores de dirección y posición
            m_ViewMatrix = glm::lookAt(cameraPosition, cameraPosition + m_Front, m_Up);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        //void RecalculateViewMatrix()
        //{
        //    ECS::Transform& cameraTransform = entity->getComponent<ECS::Transform>();
        //    glm::quat cameraRotation = cameraTransform.rotation;
        //    glm::vec3 cameraPosition = cameraTransform.position;

        //    if (cameraTransform.parent != nullptr) {
        //        ECS::Transform& parentTransform = cameraTransform.parent->getComponent<ECS::Transform>();
        //        glm::quat parentRotation = parentTransform.rotation;
        //        glm::vec3 parentPosition = parentTransform.position;

        //        // Rotar la posición de la cámara alrededor del centro del padre
        //        glm::vec3 relativePosition = cameraPosition - parentPosition; // Posición relativa a la del padre
        //        relativePosition = glm::rotate(parentRotation, relativePosition); // Rotar la posición relativa
        //        cameraPosition = parentPosition + relativePosition; // Volver a aplicar la posición del padre

        //        // La rotación de la cámara debería ser la rotación del padre
        //        cameraRotation = parentRotation;
        //    }

        //    // Actualizar los vectores de dirección de la cámara usando la rotación combinada
        //    m_Front = glm::rotate(cameraRotation, glm::vec3(0, 0, -1));
        //    m_Right = glm::rotate(cameraRotation, glm::vec3(1, 0, 0));
        //    m_Up = glm::rotate(cameraRotation, glm::vec3(0, 1, 0));

        //    // Utilizar glm::lookAt para crear la matriz de vista con los nuevos vectores de dirección y posición
        //    m_ViewMatrix = glm::lookAt(cameraPosition, cameraPosition + m_Front, m_Up);
        //    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        //}
        //void RecalculateViewMatrix()
        //{
        //    ECS::Transform& cameraTransform = entity->getComponent<ECS::Transform>();
        //    glm::quat cameraRotation = cameraTransform.rotation;
        //    glm::vec3 cameraPosition = cameraTransform.position;

        //    // Verificar si la entidad tiene padre y ajustar la posición y rotación en consecuencia
        //    if (cameraTransform.parent != nullptr) {
        //        // Obtener la transformación del padre
        //        ECS::Transform& parentTransform = cameraTransform.parent->getComponent<ECS::Transform>();
        //        glm::quat parentRotation = parentTransform.rotation;
        //        glm::vec3 parentPosition = parentTransform.position;

        //        // Combinar la posición: posición del padre + posición de la cámara
        //        cameraPosition = parentPosition + cameraPosition; // Esto es en el espacio local del padre

        //        // Combinar la rotación: rotación del padre * rotación de la cámara
        //        cameraRotation = parentRotation * cameraRotation; // Orden de multiplicación es importante
        //    }

        //    // Actualizar los vectores de dirección de la cámara usando la rotación combinada
        //    m_Front = glm::rotate(cameraRotation, glm::vec3(0, 0, -1));
        //    m_Right = glm::rotate(cameraRotation, glm::vec3(1, 0, 0));
        //    m_Up = glm::rotate(cameraRotation, glm::vec3(0, 1, 0));

        //    // Actualizar la posición de la cámara
        //    m_Position = cameraPosition;

        //    // Utilizar glm::lookAt para crear la matriz de vista con los nuevos vectores de dirección y posición
        //    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        //    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        //}





        void drawDebug()
        {
            GLCore::Render::ShaderManager::Get("debug")->use();
            GLCore::Render::ShaderManager::Get("debug")->setMat4("model", model_transform_matrix);
            GLCore::Render::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            // Calcula los vértices del frustum
            std::vector<glm::vec3> frustumVertices = calculateFrustumVertices();

            // Aquí es donde dibujaremos las aristas reales, no solo los vértices
            std::vector<glm::vec3> frustumEdges = drawFrustumEdges(frustumVertices);

            // Crear o actualizar el buffer con todas las aristas del frustum
            // (cambio) Estamos utilizando frustumEdges en lugar de frustumVertices
            if (frustumVBO == 0) {
                glGenBuffers(1, &frustumVBO);
                glBindBuffer(GL_ARRAY_BUFFER, frustumVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * frustumEdges.size(), frustumEdges.data(), GL_STATIC_DRAW);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, frustumVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * frustumEdges.size(), frustumEdges.data());
            }

            // Configura el atributo de posición del vértice
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            // Dibuja las líneas del frustum
            glLineWidth(2);
            glDrawArrays(GL_LINES, 0, frustumEdges.size());  // frustumVertices ahora incluye los puntos para las aristas azules

            // Desvincula el buffer
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glUseProgram(0);
        }
        std::vector<glm::vec3> calculateFrustumVertices()
        {
            glm::mat4 invProjMatrix = glm::inverse(m_ProjectionMatrix);
            std::vector<glm::vec3> frustumVertices(8);

            std::vector<glm::vec4> ndcCorners = {
                glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),  // top-left-near
                glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),   // top-right-near
                glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // bottom-left-near
                glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),  // bottom-right-near
                glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),   // top-left-far
                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // top-right-far
                glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),  // bottom-left-far
                glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),   // bottom-right-far
            };

            for (int i = 0; i < ndcCorners.size(); ++i)
            {
                glm::vec4 worldSpaceCorner = invProjMatrix * ndcCorners[i];
                worldSpaceCorner /= worldSpaceCorner.w;
                frustumVertices[i] = glm::vec3(worldSpaceCorner);
            }

            return frustumVertices;
        }
        std::vector<glm::vec3> drawFrustumEdges(const std::vector<glm::vec3>& frustumVertices)
        {
            // Asumiendo que frustumVertices tiene 8 vértices en el siguiente orden:
            // 0: top-left-near, 1: top-right-near, 2: bottom-left-near, 3: bottom-right-near
            // 4: top-left-far,  5: top-right-far,  6: bottom-left-far,  7: bottom-right-far

            // Las líneas existentes del frustum (las que ya tienes)
            std::vector<glm::vec3> frustumEdges = {
                // Aristas del plano cercano
                frustumVertices[0], frustumVertices[1],
                frustumVertices[1], frustumVertices[3],
                frustumVertices[3], frustumVertices[2],
                frustumVertices[2], frustumVertices[0],
                // Aristas del plano lejano
                frustumVertices[4], frustumVertices[5],
                frustumVertices[5], frustumVertices[7],
                frustumVertices[7], frustumVertices[6],
                frustumVertices[6], frustumVertices[4],
            };

            // Añade las aristas azules (lados del frustum y las diagonales)
            std::vector<glm::vec3> additionalEdges = {
                // Aristas laterales
                frustumVertices[0], frustumVertices[4],
                frustumVertices[1], frustumVertices[5],
                frustumVertices[2], frustumVertices[6],
                frustumVertices[3], frustumVertices[7],
                //// Diagonales del plano cercano
                //frustumVertices[0], frustumVertices[3],
                //frustumVertices[1], frustumVertices[2],
                //// Diagonales del plano lejano
                //frustumVertices[4], frustumVertices[7],
                //frustumVertices[5], frustumVertices[6],
            };

            // Añade las aristas adicionales al conjunto de líneas a dibujar
            frustumEdges.insert(frustumEdges.end(), additionalEdges.begin(), additionalEdges.end());

            // Ahora puedes usar frustumEdges para crear/actualizar tu VBO y dibujar las líneas como lo hiciste anteriormente
            // Asegúrate de que el conteo de vértices que pasas a glDrawArrays() refleje la cantidad total de vértices en frustumEdges

            return frustumEdges;
        }
    };
} // namespace ECS


//PARA CALCULAR EL FRUSTRUM CON UN FAR FIJO a 10
//std::vector<glm::vec3> calculateFrustumVertices()
//{
//    glm::mat4 invViewMatrix = glm::inverse(m_ViewMatrix);
//    glm::mat4 invProjMatrix = glm::inverse(m_ProjectionMatrix);
//    std::vector<glm::vec3> frustumVertices(8);
//
//    // Estos son los NDC para el plano near
//    std::vector<glm::vec4> ndcNearCorners = {
//        glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),  // top-left-near
//        glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),   // top-right-near
//        glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // bottom-left-near
//        glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),  // bottom-right-near
//    };
//
//    // Calcular vértices del plano near usando la matriz de proyección
//    for (int i = 0; i < 4; ++i)
//    {
//        glm::vec4 worldSpaceCorner = invProjMatrix * ndcNearCorners[i];
//        worldSpaceCorner /= worldSpaceCorner.w;
//        frustumVertices[i] = glm::vec3(worldSpaceCorner);
//    }
//
//    // Para el plano far, calculamos la dirección y colocamos los puntos a una distancia fija
//    glm::vec3 cameraPos = glm::vec3(invViewMatrix[3]); // Posición de la cámara en el espacio del mundo
//    glm::vec3 cameraForward = glm::vec3(invViewMatrix[2]); // Dirección hacia la que la cámara está mirando
//
//    float farDistance = 10.0f; // Distancia fija para el plano far
//    // Calcular vértices del plano far a una distancia fija de 10
//    for (int i = 0; i < 4; ++i)
//    {
//        // Usamos la misma dirección que los vértices del plano near, pero extendemos a la distancia fija
//        glm::vec3 direction = glm::normalize(frustumVertices[i] - cameraPos);
//        frustumVertices[i + 4] = cameraPos - direction * farDistance; // Cambiar a '-' para dirección hacia adelante
//    }
//
//    return frustumVertices;
//}