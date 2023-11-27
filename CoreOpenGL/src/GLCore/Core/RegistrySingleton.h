#pragma once

#include "entt.hpp"

namespace GLCore 
{
    class RegistrySingleton {
    public:
        static entt::registry& getRegistry() {
            static entt::registry instance;
            return instance;
        }

        // Eliminar m�todos de copia y movimiento
        RegistrySingleton(const RegistrySingleton&) = delete;
        RegistrySingleton& operator=(const RegistrySingleton&) = delete;

    private:
        RegistrySingleton() {}
    };

}
