#pragma once

#include "../../glpch.h"
#include <stb_image/stb_image.h>
#include <filesystem>
#include <iostream>

namespace GLCore::Utils 
{

    class ImageLoader {

    private:
        static std::unordered_map<std::string, Image> loadedImages;
        static std::string buscarTextura(const std::string& nombreArchivo, const std::string& carpetaBase);

    public:
        static Image loadImage(const std::string& filepath, const std::string carpetaBase = "");
        static GLuint loadImagesForCubemap(std::vector<const char*> faces);
        static GLuint loadIcon(const char* filepath);
        static GLuint loadHDR(const char* filepath);
        static void freeImage(Image& img);
    };

}
