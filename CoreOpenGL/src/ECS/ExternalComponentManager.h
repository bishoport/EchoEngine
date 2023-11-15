#include <Windows.h>
#include <string>
#include <iostream>

namespace ECS {

    class Component; // Declaraci�n adelantada

    class ExternalComponentManager {
    private:
        HMODULE hModule;
        typedef Component* (*CreateComponentFunc)();
        CreateComponentFunc createComponent;

    public:
        ExternalComponentManager() : hModule(nullptr), createComponent(nullptr) {}

        ~ExternalComponentManager() {
            if (hModule) {
                FreeLibrary(hModule);
                hModule = nullptr;
            }
        }

        bool loadComponentLibrary(const std::string& libraryPath) {
            // Convertir std::string a std::wstring
            std::wstring wideLibraryPath = std::wstring(libraryPath.begin(), libraryPath.end());

            // Cargar la DLL
            hModule = LoadLibraryW(wideLibraryPath.c_str());
            if (!hModule) {
                std::cerr << "Failed to load the DLL: " << libraryPath << std::endl;
                return false;
            }

            // Obtener la funci�n de f�brica
            createComponent = (CreateComponentFunc)GetProcAddress(hModule, "CreateTestComponent");
            if (!createComponent) {
                std::cerr << "Failed to locate the function: CreateTestComponent" << std::endl;
                FreeLibrary(hModule);
                hModule = nullptr;
                return false;
            }

            return true;
        }

        Component* createComponentInstance() {
            if (createComponent) {
                return createComponent(); // Llama a la funci�n de f�brica
            }
            return nullptr;
        }

        // Nueva funci�n para descargar la DLL
        void unloadComponentLibrary() {
            if (hModule) {
                std::cout << "Liberamos la DLL" << std::endl;
                FreeLibrary(hModule); // Libera la DLL
                hModule = nullptr;
                createComponent = nullptr; // Aseg�rate de resetear el puntero a la funci�n
            }
        }
    };

} // namespace ECS
