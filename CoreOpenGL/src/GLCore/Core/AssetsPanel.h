#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <fstream>
#include <functional>
#include "../DataStruct.h"

namespace GLCore {


	class AssetsPanel
	{
	public:

		AssetsPanel();
		

		void OnImGuiRender();

		// Función para establecer el delegado
		void SetDelegate(std::function<void(ImportOptions importOptions)> delegado) {
			getModelPathFromAssetsDelegate = delegado;
		}



	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_DirectoryEntry;

		//Icons
		GLuint iconFolder;
		GLuint iconModel; 
		GLuint iconImage;

		ImportOptions importOptions;
		bool isDialogOpen = false;

		std::filesystem::path s_AssetPath = "assets";


		//callbacks:
		std::function<void(ImportOptions importOptions)> getModelPathFromAssetsDelegate;

		// Función que utiliza el delegado
		void UsarDelegado(const ImportOptions importOptions) {
			if (getModelPathFromAssetsDelegate) {  // Verifica que el delegado haya sido establecido
				getModelPathFromAssetsDelegate(importOptions);
			}
		}
	};
}