#pragma once
#include <bitset>
#include "ScriptableComponent.h"
#include <unordered_map>
#include <memory>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace ECS_SCRIPTING
{
    class ScriptableEntity {

    private:

        MonoDomain* m_ptrMonoDomain = nullptr;
        MonoAssembly* m_ptrCsEngineAssembly = nullptr;
        MonoImage* m_ptrCsEngineAssemblyImage = nullptr;

        std::unordered_map<std::string, MonoClass*> m_Classes;
        std::unordered_map<std::string, MonoObject*> m_Instances;

    public:
        int id;
        std::string name;
        bool markedToDelete = false;

        //-Componentes
        std::unordered_map<std::string, std::unique_ptr<ScriptableComponent>> components;

        void LoadDll()
        {
            if (m_ptrCsEngineAssemblyImage != nullptr)
                return;

            //Current directory as mono dir
            mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc");

            //Create mono domain
            m_ptrMonoDomain = mono_jit_init("CsEngine");
            //mono_domain_free();
            if (m_ptrMonoDomain)
            {
                //Load a mono assembly CsEngineScript.dll
                m_ptrCsEngineAssembly = mono_domain_assembly_open(m_ptrMonoDomain, "C:/Users/pdortegon/Documents/PROPIOS/EchoEngine/x64/Debug/CsEngineScript.dll");
                if (m_ptrCsEngineAssembly)
                {
                    //Loading mono image
                    m_ptrCsEngineAssemblyImage = mono_assembly_get_image(m_ptrCsEngineAssembly);
                    if (m_ptrCsEngineAssemblyImage)
                    {
                        std::cout << "Mono DLL Load Success" << std::endl;
                    }
                }
            }
        }

        ScriptableEntity(int nextID)
        {
            this->id = nextID;
            LoadDll();
        }

        int getID() const
        {
            return id;
        }

        void update(GLCore::Timestep deltaTime)
        {
            for (auto& pair : components) {
                std::string key = pair.first;
                ScriptableComponent* component = pair.second.get();
                component->update(deltaTime);
            }
        }
        void draw()
        {
            for (auto& pair : components) {
                std::string key = pair.first;
                ScriptableComponent* component = pair.second.get();
                component->draw();
            }
        }
        void drawGUI_Inspector()
        {
            for (auto& pair : components) {
                std::string key = pair.first;
                ScriptableComponent* component = pair.second.get();
                component->drawGUI_Inspector();
            }
        }
        bool isActive() const
        {
            return true;
        }
        void destroy(){}


        //--COMPONENTS
        void addComponentScriptableComponent(const std::string className)
        {
            ECS_SCRIPTING::ScriptableComponent* component = new ECS_SCRIPTING::ScriptableComponent(); // O una clase derivada de ScriptableComponent.
            component->ClassName = "Transform";
            components[component->ClassName] = std::unique_ptr<ECS_SCRIPTING::ScriptableComponent>(component);
            component->monoInstance = CreateCsComponent(component->ClassName);

            component->init();

        }

        MonoObject* CreateCsComponent(const std::string& className)
        {
            MonoClass* monoClass = mono_class_from_name(m_ptrCsEngineAssemblyImage, "CsEngineScript.ECS", className.c_str());
            if (monoClass)
            {
                MonoObject* component = mono_object_new(m_ptrMonoDomain, monoClass);
                if (component)
                {
                    mono_runtime_object_init(component);
                    m_Classes[className] = monoClass;
                    m_Instances[className] = component;
                    return component;
                }
            }

            return nullptr;
        }

        ScriptableComponent* getComponent(const std::string& key) {
            auto it = components.find(key);
            if (it != components.end()) {
                return it->second.get();
            }
            return nullptr; // O lanzar una excepción si prefieres.
        }

        std::vector<ScriptableComponent*> getComponents() {
            std::vector<ScriptableComponent*> componentPointers;

            for (auto& pair : components) {
                componentPointers.push_back(pair.second.get());
            }

            return componentPointers;
        }

        void removeComponent(const std::string& key) {
            auto it = components.find(key);
            if (it != components.end()) {
                // Llamar a onDestroy si es necesario.
                // it->second->onDestroy();
                components.erase(it);
            }
        }


        void removeAllComponents()
        {
            // Llamar a onDestroy para cada componente antes de eliminarlos si es necesario.
            for (auto& pair : components) {
                pair.second->onDestroy();
            }

            // Eliminar todos los elementos del mapa, lo que destruirá los componentes.
            components.clear();
        }
    };



}