#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <string>
#include <unordered_map>

#include "ECS/ScriptableEntity.h"
#include "ECS/ScriptableManager.h"
#include "../GLCore/DataStruct.h"

namespace Scripting
{
    class ScriptableGameObjectManager
    {
    public:

        static ScriptableGameObjectManager& GetInstance()
        {
            static ScriptableGameObjectManager instance;
            return instance;
        }

        //--VARIABLES
        ECS::ScriptableEntity* m_SelectedEntity = nullptr;

        //-DLL------------------------------------------------------------------------
        ScriptableGameObjectManager(const ScriptableGameObjectManager&) = delete;
        ScriptableGameObjectManager& operator=(const ScriptableGameObjectManager&) = delete;
        MonoObject* CreateCsComponent(const std::string& className);
        void InvokeMethod(const std::string& className, const std::string& methodName, void** params = nullptr, int paramCount = 0);
        void LoadDll();
        void UnloadDLL();
        //----------------------------------------------------------------------------


        ECS::ScriptableEntity* CreateEmptyGameObject();
        void createGameObject(MainMenuAction action);
        void loadFileModel(GLCore::ImportOptions importOptions);
        void drawHierarchy();


        MonoObject* GetCsComponentProperty(const std::string& className, const std::string& propertyName);
        void SetCsComponentProperty(const std::string& className, const std::string& propertyName, MonoObject* value);

    private:
        ScriptableGameObjectManager();
        ~ScriptableGameObjectManager();

        int idGenerated = 0;
        ECS::ScriptableManager manager;

        MonoDomain* m_ptrMonoDomain = nullptr;
        MonoAssembly* m_ptrCsEngineAssembly = nullptr;
        MonoImage* m_ptrCsEngineAssemblyImage = nullptr;

        std::unordered_map<std::string, MonoClass*> m_Classes;
        std::unordered_map<std::string, MonoObject*> m_Instances;
    };
}


