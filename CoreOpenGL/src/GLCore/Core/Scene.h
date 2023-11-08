#pragma once
//#include "../glpch.h"
#include "../Render/PrimitivesHelper.h"
#include "../Render/Shader.h"

#include "../../ECS/ECS.h"



#include "../Util/PerspectiveCameraController.h"
#include "../Util/PostProcessingManager.h"
#include "AssetsPanel.h"
#include "../Util/Skybox.h"
#include "../Util/DynamicSkybox.h"
#include "../Util/GridWorldReference.h"
#include "../../ECS/Camera.h"



namespace GLCore {

    class Scene {
    public:

        


        Scene();
        ~Scene();

        bool initialize();
        void update(Timestep deltaTime);
        void render();

        void drawAllEntities();
        

        void checkGizmo();
        void renderGUI();
        void shutdown();

        void createGameObject(MainMenuAction action);

        void getModelPathFromAssets(ImportOptions importOptions);

        static std::pair<glm::vec3, float> SceneBounds;

    private:

        bool isWireframe = false;

        //--ENTITIES
        ECS::Manager manager;
        ECS::Entity* m_SelectedEntity = nullptr;
        ECS::Entity* gridWorldReference = nullptr;

        void CalcSceneBundle();
        //---------------------------------------------------

        //OTHER NOT GAMEOBJECTS 
        Utils::Skybox* skybox = nullptr;
        Utils::DynamicSkybox* dynamicSkybox = nullptr;
        Utils::GridWorldReference* gridWorldRef = nullptr;
        //---------------------------------------------------

        
        //--GENERICS_ALL_PANELS
        ImVec2 currentPanelPos = ImVec2(0.0f, 0.0f);
        ImVec2 currentPanelSize = ImVec2(640.0f, 480.0f);
        //---------------------------------------------------

        //--SCENE_PANEL
        ImVec2 scenePos = ImVec2(0.0f, 0.0f);
        ImVec2 sceneSize = ImVec2(640.0f, 480.0f);
        bool mouseInScene = false;
        //---------------------------------------------------

        //--GAME_PANEL
        std::vector<ECS::Camera*> cameras;
        ImVec2 gamePos = ImVec2(0.0f, 0.0f);
        ImVec2 gameSize = ImVec2(640.0f, 480.0f);
        bool mouseInGame = false;
        //---------------------------------------------------

        //--FBO SCENE
        bool useSceneFBO = true;
        GLuint scene_FBO = 0;
        GLuint scene_depthBuffer = 0;
        std::vector<GLuint> scene_colorBuffers;

        void SetGenericsUniforms(glm::mat4 cameraProjectionMatrix, glm::mat4 cameraViewMatrix, glm::vec3 cameraPosition);
        void RenderPipeline(glm::mat4 cameraProjectionMatrix, glm::mat4 cameraViewMatrix, glm::vec3 cameraPosition, FBO_Data fboData);
        //---------------------------------------------------

        //--POST_PROCESSING
        Utils::PostProcessingManager* postproManager = nullptr;
        bool usePostprocessing = true;
        unsigned int quadVAO = 0;
        unsigned int quadVBO;
        void renderQuad();
        void useShader(const ImDrawList* asd, const ImDrawCmd* command);
        //---------------------------------------------------
        

        //IBL
        bool showSkybox = false;
        unsigned int IBL_FBO;
        unsigned int IBL_RBO;

        float mixFactor = 0.0f;

        unsigned int envCubemap;
        unsigned int irradianceMap = 0;
        unsigned int prefilterMap;
        unsigned int hdrTexture_daylight;
        unsigned int hdrTexture_nightlight;
        unsigned int brdfLUTTexture;

        unsigned int cubeVAO = 0;
        unsigned int cubeVBO = 0;
        void renderCube();
        void prepare_PBR_IBL();
        //----------------------------------------------------


        


        //--Illumination
        glm::vec3 globalAmbient = glm::vec3(0.055f, 0.055f, 0.055f);
        ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        bool useDirectionalLight = false;
        int totalPointLight = 0;
        int totalSpotLight = 0;
        //---------------------------------------------------

        

        //--CAMERA EDITOR
        GLCore::Utils::PerspectiveCameraController m_EditorCamera;
        float aspectRatio;
        //---------------------------------------------------

        //--GUI
        AssetsPanel assetsPanel;
        enum class GizmoOperation
        {
            Translate,
            Rotate2D, Rotate3D,
            Scale
        };
        GizmoOperation m_GizmoOperation;
        //---------------------------------------------------


        ModelParent loadFileModel(ImportOptions importOptions);
        bool pickingObj = false;
        bool selectingEntity = false;
        bool cursorOverSelectEntityDialog = false;
        std::vector<ECS::Entity*> entitiesInRay;
        void CheckIfPointerIsOverObject();
        bool rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax);
    };
}