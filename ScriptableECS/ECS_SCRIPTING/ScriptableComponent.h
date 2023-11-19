#pragma once
#include <cstddef>
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep
#include "../src/GLCore/Core/Timestep.h"

#include <mono/jit/jit.h>  // Para MonoObject
#include <iostream>

namespace ECS_SCRIPTING
{
    using ComponentID = std::size_t;

    inline ComponentID getNewComponentTypeID() {
        static ComponentID lastID = 0u;
        return lastID++;
    }

    template <typename T>
    inline ComponentID getComponentTypeID() noexcept {
        static ComponentID componentID = getNewComponentTypeID();
        return componentID;
    }

    constexpr std::size_t maxComponents = 32;





    class ScriptableEntity;

    class ScriptableComponent {

    private:
        ComponentID componentID; // Variable miembro para almacenar el ComponentID

    public:
        ScriptableEntity* entity;

        std::string ClassName;
        MonoObject* monoInstance = nullptr;

        ScriptableComponent() : componentID(getNewComponentTypeID()) {} // Constructor para inicializar el componentID

        void init()
        {
            std::cout << "Init Script Component" << std::endl;
            /*ScriptableGameObjectManager::GetInstance().InvokeMethod(ClassName, "Init");*/
        }

        void update(GLCore::Timestep deltaTime) {}

        void draw() 
        {
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

        void drawGUI_Inspector()
        {
            ImGui::Text("Script %s", ClassName.c_str());
            //ImGui::Text("Component ID: %i", getComponentID());
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            
        }


        void onDestroy() {}
        ~ScriptableComponent() { onDestroy(); }

        ComponentID getComponentID() const { return componentID; } // Getter para componentID

        void setComponentID(ComponentID id) { componentID = id; } // Setter para componentID
    };
}