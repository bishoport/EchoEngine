#include "AssetsPanel.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <iostream>
#include "../Util/IMGLoader.h"

namespace GLCore {

	AssetsPanel::AssetsPanel()
		: m_BaseDirectory{},     // Inicialización predeterminada
		  m_CurrentDirectory{},  // Inicialización predeterminada
		  m_DirectoryEntry{},    // Inicialización predeterminada
		  iconFolder{ 0 },    // Inicialización predeterminada (0 es un valor común para inicializar texturas OpenGL)
		  iconModel{ 0 },     // Inicialización predeterminada
		  iconImage{ 0 },     // Inicialización predeterminada
		  importOptions{},       // Inicialización predeterminada (llama al constructor por defecto de ImportOptions)
		  isDialogOpen{ false }  // Inicialización predeterminada
	{
		m_CurrentDirectory = s_AssetPath;

		iconFolder = GLCore::Utils::ImageLoader::loadIcon("assets/default/icons/folder_icon.png");
		iconModel  = GLCore::Utils::ImageLoader::loadIcon("assets/default/icons/model_icon.png");
		iconImage  = GLCore::Utils::ImageLoader::loadIcon("assets/default/icons/picture_icon.png");
	}



	void AssetsPanel::OnImGuiRender()
    {
		ImGui::Begin("Assets", nullptr);

		ImGui::BeginChild("FileRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

		if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 0.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = thumbnailSize < 40 ? 1 : (int)(panelWidth / cellSize);

		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);



		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());

			GLuint iconTexture = 0; // Reemplazar con la ID de tu textura real


			if (directoryEntry.is_directory())
			{
				iconTexture = iconFolder; // Asumiendo que folderTexture es la ID de tu textura para carpetas
			}
			else if (path.extension() == ".fbx" || path.extension() == ".obj" || path.extension() == ".gltf")
			{
				iconTexture = iconModel; // Asumiendo que modelTexture es la ID de tu textura para modelos
			}
			else if (path.extension() == ".png" || path.extension() == ".jpg")
			{
				iconTexture = iconImage; // Asumiendo que imageTexture es la ID de tu textura para imágenes
			}


			if (iconTexture != 0)
			{
				ImGui::ImageButton((ImTextureID)(intptr_t)iconTexture, ImVec2(thumbnailSize - 15, thumbnailSize - 15));
			}
			else
			{
				ImGui::Button(filenameString.c_str(), { thumbnailSize, thumbnailSize });
			}


			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();
				}
				else
				{
					isDialogOpen = true;  // Abre el diálogo
					m_DirectoryEntry = directoryEntry;
				}
			};
			//------------------------------------------------------------------------


			// Si este elemento se está arrastrando...
			if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0, 0.0f))
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					const std::string payload_n = relativePath.string();
					ImGui::SetDragDropPayload("ASSET_DRAG", payload_n.c_str(), payload_n.length() + 1, ImGuiCond_Once);
					ImGui::EndDragDropSource();
				}
			}

			if (thumbnailSize < 40)
			{
				ImGui::SameLine();  // Asegura que el texto aparezca en la misma línea que el icono
			}

			ImGui::Text(filenameString.c_str());


			ImGui::NextColumn();

			ImGui::PopID();
		}

		
		if (isDialogOpen) {
			ImGui::OpenPopup("Import Options");
		}

		if (ImGui::BeginPopupModal("Import Options", &isDialogOpen)) {
			ImGui::Checkbox("Invert UV", &importOptions.invertUV);
			ImGui::Checkbox("Rotate 90 degrees on X-axis", &importOptions.rotate90);
			if (ImGui::Button("Accept")) {
				isDialogOpen = false;  // Cierra el diálogo

				std::filesystem::path fullPath = m_DirectoryEntry;
				std::string filePath = fullPath.parent_path().string();
				std::string fileName = fullPath.filename().string();
				if (filePath.back() != '\\') {
					filePath += '\\';
				}
				importOptions.filePath = filePath;
				importOptions.fileName = fileName;
				UsarDelegado(importOptions);  // Llama a UsarDelegado con las opciones
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				isDialogOpen = false;  // Cierra el diálogo
			}
			ImGui::EndPopup();
		}

		ImGui::Columns(1);

		ImGui::EndChild();
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 39, 512);
		ImGui::End();
    }

}