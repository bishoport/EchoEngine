#pragma once
#include <cstddef>
#include "../src/glpch.h"
#include "../src/GLCore/Core/Timestep.h"

#include <mono/jit/jit.h>  // Para MonoObject
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>

#include <iostream>
#include <variant>
#include <algorithm> // Para std::copy_n

#include <nlohmann/json.hpp>

namespace ECS_SCRIPTING {
    using JsonVariant = std::variant<std::string, float, glm::vec3, glm::quat, int, bool>;

    class ScriptableEntity;

    class ScriptableComponent {

    private:
        std::unordered_map<std::string, JsonVariant> properties;

    public:
        ScriptableEntity* entity = nullptr;
        std::string ClassName;
        MonoObject* monoInstance = nullptr;

        ScriptableComponent() {}

        void processJson(const std::string& jsonString) {
            nlohmann::json json = nlohmann::json::parse(jsonString);

            for (auto& [key, valueObj] : json.items()) {
                std::string type = valueObj["Type"];
                nlohmann::json value = valueObj["Value"];

                // Procesamiento de tipos
                if (type == "String") {
                    properties[key] = value.get<std::string>();
                }
                else if (type == "Single") {
                    properties[key] = value.get<float>();
                }
                else if (type == "Int32") {
                    properties[key] = value.get<int>();
                }
                else if (type == "Boolean") {
                    properties[key] = value.get<bool>();
                }
                else if (type == "Vector3") {
                    glm::vec3 vec = {
                        value["X"].get<float>(),
                        value["Y"].get<float>(),
                        value["Z"].get<float>()
                    };
                    properties[key] = vec;
                }
                else if (type == "Quaternion") {
                    glm::quat quat = {
                        value["W"].get<float>(),
                        value["X"].get<float>(),
                        value["Y"].get<float>(),
                        value["Z"].get<float>()
                    };
                    properties[key] = quat;
                }
            }
        }
        
        
        // Estructura VariantVisitor para visitar cada tipo de variante y dibujarlo con ImGui
        struct VariantVisitor 
        {
            VariantVisitor(const std::string& key) : key(key) {}

            void operator()(std::string& value) {
                char buffer[256];
                std::copy_n(value.begin(), std::min(value.size(), sizeof(buffer) - 1), buffer);
                buffer[std::min(value.size(), sizeof(buffer) - 1)] = '\0';

                if (ImGui::InputText(key.c_str(), buffer, sizeof(buffer))) {
                    value = buffer;
                }
            }

            void operator()(float& value) {
                ImGui::InputFloat(key.c_str(), &value);
            }

            void operator()(glm::vec3& value) {
                ImGui::DragFloat3(key.c_str(), glm::value_ptr(value), 0.01f);
            }

            void operator()(glm::quat& value) {
                ImGui::DragFloat4(key.c_str(), glm::value_ptr(value), 0.01f);
            }

            void operator()(int& value) {
                ImGui::InputInt(key.c_str(), &value);
            }

            void operator()(bool& value) {
                ImGui::Checkbox(key.c_str(), &value);
            }

        private:
            const std::string& key;
        };
        


        void init() {
            std::cout << "Init Script Component" << std::endl;

            if (monoInstance) {
                // Obtener la clase Mono del objeto
                MonoClass* monoClass = mono_object_get_class(monoInstance);

                // Obtener el método 'Init'
                MonoMethod* initMethod = mono_class_get_method_from_name(monoClass, "Init", 0);

                if (initMethod) {
                    // Invocar el método 'Init'
                    mono_runtime_invoke(initMethod, monoInstance, nullptr, nullptr);

                    // Obtener el método 'SerializeToJson'
                    MonoMethod* serializeToJsonMethod = mono_class_get_method_from_name(monoClass, "SerializeToJson", 0);
                    if (serializeToJsonMethod) {
                        MonoObject* jsonMonoString = mono_runtime_invoke(serializeToJsonMethod, monoInstance, nullptr, nullptr);
                        if (jsonMonoString) {
                            char* jsonString = mono_string_to_utf8(mono_object_to_string(jsonMonoString, nullptr));
                            processJson(jsonString);
                            mono_free(jsonString); // Liberar la cadena después de usarla
                        }
                    }
                }
                else {
                    std::cerr << "No se pudo encontrar el método 'Init'." << std::endl;
                }
            }
        }
        void update(GLCore::Timestep deltaTime) 
        {
            MonoClass* monoClass = mono_object_get_class(monoInstance);
            MonoMethod* updateMethod = mono_class_get_method_from_name(monoClass, "Update", 1);

            if (updateMethod) {
                float deltaTimeMillis = deltaTime.GetMilliseconds();
                MonoObject* deltaTimeMillisBoxed = mono_value_box(mono_get_root_domain(), mono_get_single_class(), &deltaTimeMillis);

                // Preparar el array de argumentos
                void* args[1];
                args[0] = deltaTimeMillisBoxed;

                // Invocar el método 'Update'
                mono_runtime_invoke(updateMethod, monoInstance, args, nullptr);
            }
        }

        void draw() 
        {

        }

        void drawGUI_Inspector() {
            ImGui::Text("Script %s", ClassName.c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            for (auto& [key, value] : properties) {
                VariantVisitor visitor(key);
                std::visit(visitor, value);
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
            }
        }

        void onDestroy() {}


        ~ScriptableComponent() { onDestroy(); }
    };
}









/*void operator()(const std::string& value) {
ImGui::Text("String: %s", value.c_str());
}*/
//strncpy_s(buffer, value.c_str(), sizeof(buffer));


//MonoClass* monoClass = mono_object_get_class(monoInstance);
//// Obtener la propiedad 'Position' de la clase Transform
//MonoProperty* positionProperty = mono_class_get_property_from_name(monoClass, "Position");
//
//if (positionProperty) {
//    // Obtener el getter de la propiedad 'Position'
//    MonoMethod* getPositionMethod = mono_property_get_get_method(positionProperty);
//    MonoObject* positionObject = mono_runtime_invoke(getPositionMethod, monoInstance, nullptr, nullptr);
//
//    // Ahora tienes un objeto Mono que representa la posición, necesitas obtener el valor 'x'
//    if (positionObject) {
//        // Obtener la clase Mono de Vector3
//        MonoClass* vector3Class = mono_object_get_class(positionObject);
//
//        // Obtener el campo 'X' de la clase Vector3
//        MonoClassField* xField = mono_class_get_field_from_name(vector3Class, "X");
//        if (xField) {
//            float xValue;
//            mono_field_get_value(positionObject, xField, &xValue);
//
//            // Ahora puedes imprimir el valor de x
//            std::cout << "Transform.position.x: " << xValue << std::endl;
//        }
//    }
//}














//
//// A continuación, obtienes y procesas las propiedades serializadas, si es necesario
//MonoMethod* getSerializablePropertiesCopyMethod = mono_class_get_method_from_name(monoClass, "GetSerializablePropertiesCopy", 0);
//if (getSerializablePropertiesCopyMethod) {
//
//    MonoObject* serializedDataCopy = mono_runtime_invoke(getSerializablePropertiesCopyMethod, monoInstance, nullptr, nullptr);
//
//    if (serializedDataCopy) {
//        MonoClass* dictionaryClass = mono_object_get_class(serializedDataCopy);
//        MonoMethod* getEnumeratorMethod = mono_class_get_method_from_name(dictionaryClass, "GetEnumerator", 0);
//        MonoObject* enumerator = mono_runtime_invoke(getEnumeratorMethod, serializedDataCopy, nullptr, nullptr);
//        MonoClass* enumeratorClass = mono_object_get_class(enumerator);
//        MonoMethod* moveNextMethod = mono_class_get_method_from_name(enumeratorClass, "MoveNext", 0);
//        MonoMethod* getCurrentMethod = mono_class_get_method_from_name(enumeratorClass, "get_Current", 0);
//
//
//        std::mutex lock;
//
//        bool moved = true;
//        while (moved) {
//            std::lock_guard<std::mutex> guard(lock);
//
//            MonoObject* moveNextResult = mono_runtime_invoke(moveNextMethod, enumerator, nullptr, nullptr);
//            moved = *(bool*)mono_object_unbox(moveNextResult);
//
//            if (moved) {
//                MonoObject* current = mono_runtime_invoke(getCurrentMethod, enumerator, nullptr, nullptr);
//
//                auto pair = ProcessKeyValuePair(current, mono_get_root_domain());
//                properties.insert(pair);
//            }
//        }
//
//        // Aquí puedes hacer algo con 'properties', por ejemplo, imprimir las propiedades
//        for (const auto& [key, value] : properties) {
//            std::cout << "Property: " << key << " - Value: ";
//            // Aquí, podrías usar std::visit con un VariantVisitor para imprimir el valor
//            // Por ejemplo:
//            // std::visit(VariantVisitor{}, value);
//        }
//    }
//}