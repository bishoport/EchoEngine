#pragma once
//#include "../glpch.h"
#include "../Render/PrimitivesHelper.h"
#include "../Render/Shader.h"

#include "../../ECS/ECS.h"
#include "../../ECS/MeshFilter.h"
#include "../../ECS/MeshRenderer.h"
#include "../../ECS/Material.h"
#include "../../ECS/PointLight.h"

#include "../Util/PerspectiveCameraController.h"
#include "../Util/PostProcessingManager.h"
#include "AssetsPanel.h"
#include "../Util/Skybox.h"
#include "../Util/DynamicSkybox.h"
#include "../Util/GridWorldReference.h"



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
        std::vector<ECS::Entity*> entitiesInScene;
        ECS::Entity* m_SelectedEntity = nullptr;
        ECS::Entity* gridWorldReference = nullptr;
        //std::pair<glm::vec3, float> CalcSceneBundle();
        void CalcSceneBundle();
        //---------------------------------------------------

        //OTHER NOT GAMEOBJECTS 
        Utils::Skybox* skybox = nullptr;
        Utils::DynamicSkybox* dynamicSkybox = nullptr;
        Utils::GridWorldReference* gridWorldRef = nullptr;
        //---------------------------------------------------

        //--FBO
        bool useStandardFBO = false;
        std::vector<GLuint*> mainColorBuffers;
        GLuint mainFBO;
        GLuint mainRboDepth;
        GLuint mainColorBuffer;
        //----------------------------------------------------
        

        //IBL
        bool useHDRIlumination = false;
        unsigned int captureFBO;
        unsigned int captureRBO;

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

        //--POST_PROCESSING
        Utils::PostProcessingManager* postproManager = nullptr;
        bool usePostprocessing = false;

        float exposure = 1.0f;
        float gamma = 2.2f;
        unsigned int quadVAO = 0;
        unsigned int quadVBO;
        void renderQuad();
        //---------------------------------------------------

        //--Illumination
        glm::vec3 globalAmbient = glm::vec3(0.0f);
        ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        bool useDirectionalLight = false;
        int totalPointLight = 0;
        int totalSpotLight = 0;
        //---------------------------------------------------

        float aspectRatio;

        //--EDITOR
        GLCore::Utils::PerspectiveCameraController m_EditorCamera;
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