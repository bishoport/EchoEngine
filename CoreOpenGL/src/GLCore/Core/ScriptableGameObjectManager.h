#pragma once
#include "../src/glpch.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <string>
#include <unordered_map>

#include "ECS_SCRIPTING/ScriptableEntity.h"
#include "ECS_SCRIPTING/ScriptableManager.h"

#include "../DataStruct.h"


namespace Scripting
{
    class ScriptableGameObjectManager
    {
    public:

        /*static ScriptableGameObjectManager& GetInstance()
        {
            static ScriptableGameObjectManager instance;
            return instance;
        }*/

        ScriptableGameObjectManager();
        ~ScriptableGameObjectManager();

        //--VARIABLES
        ECS_SCRIPTING::ScriptableEntity* m_SelectedEntity = nullptr;

        std::vector<ECS_SCRIPTING::ScriptableEntity*> cameras;
        std::vector<ECS_SCRIPTING::ScriptableEntity*> entitiesInRay;

        bool CheckIfGameObjectInRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);

        ECS_SCRIPTING::ScriptableManager manager;

        bool useDirectionalLight = false;
        int totalPointLight = 0;
        int totalSpotLight = 0;

        void addComponentToSelectedGameObject(MainMenuAction action);

        //--SERIALIZACIONES
        void SaveSceneToFile(const std::string& filename);
        void LoadSceneFromFile(const std::string& filename, std::vector<ECS_SCRIPTING::ScriptableEntity*>& entitiesInScene, ECS_SCRIPTING::ScriptableManager& manager);


        //-DLL------------------------------------------------------------------------
        ScriptableGameObjectManager(const ScriptableGameObjectManager&) = delete;
        ScriptableGameObjectManager& operator=(const ScriptableGameObjectManager&) = delete;
        //MonoObject* CreateCsComponent(const std::string& className);
        void InvokeMethod(const std::string& className, const std::string& methodName, void** params = nullptr, int paramCount = 0);
       /* void LoadDll();
        void UnloadDLL();*/
        //----------------------------------------------------------------------------



        ECS_SCRIPTING::ScriptableEntity& CreateEmptyGameObject();

        void createPresetGameObject(MainMenuAction action);
        void loadFileModel(GLCore::ImportOptions importOptions);
        void drawHierarchy();


        MonoObject* GetCsComponentProperty(const std::string& className, const std::string& propertyName);
        void SetCsComponentProperty(const std::string& className, const std::string& propertyName, MonoObject* value);

    private:
       

        float pi = 3.1415926535f;
        bool rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax);
        int idGenerated = 0;
        

        MonoDomain* m_ptrMonoDomain = nullptr;
        MonoAssembly* m_ptrCsEngineAssembly = nullptr;
        MonoImage* m_ptrCsEngineAssemblyImage = nullptr;

        //std::unordered_map<std::string, MonoClass*> m_Classes;
        //std::unordered_map<std::string, MonoObject*> m_Instances;
    };
}


