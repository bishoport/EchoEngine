#pragma once

#include "../src/glpch.h"
#include <mono/jit/jit.h>  // Para MonoObject

#include "../src/GLCore/Core/ScriptableGameObjectManager.h"
#include "ScriptableComponent.h"
#include <string>
#include <iostream>

namespace ECS_SCRIPTING
{
    class ScriptComponent : public ScriptableComponent
    {
    public:

        std::string ClassName;
        MonoObject* monoInstance = nullptr;

        void init() override
        {   
            std::cout << "Init Script Component" << std::endl;
            /*ScriptableGameObjectManager::GetInstance().InvokeMethod(ClassName, "Init");*/
        }

        void update(GLCore::Timestep timestamp) override {
            
        }

        void draw() override
        {

        }

        void drawGUI_Inspector() override
        {
            ImGui::Text("Script %s", ClassName.c_str());
            ImGui::Text("Component ID: %i", getComponentID());
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (monoInstance)
            {
                // Obtener la clase Mono
                MonoClass* monoClass = mono_object_get_class(monoInstance);

                // Obtener el método Serialize
                MonoMethod* serializeMethod = mono_class_get_method_from_name(monoClass, "Serialize", 0);
                if (serializeMethod)
                {
                    MonoObject* serializedData = mono_runtime_invoke(serializeMethod, monoInstance, nullptr, nullptr);
                    if (serializedData)
                    {
                        // Aquí, necesitas manejar el objeto serializedData que es un diccionario.
                        // Esto puede ser complicado ya que necesitas invocar métodos del diccionario y convertir los MonoObjects a tipos C++.
                        // Por ejemplo, puedes obtener los pares clave-valor y mostrarlos.
                        // La implementación específica dependerá de cómo quieras mostrar o manejar estas propiedades en ImGui.
                    }
                }
            }
        }

        ComponentID getComponentID() const
        {
            return getComponentTypeID<ScriptComponent>();
        }
    };
}
