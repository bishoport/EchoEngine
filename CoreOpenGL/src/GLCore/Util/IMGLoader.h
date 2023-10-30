#pragma once

#include "../../glpch.h"
#include <stb_image/stb_image.h>

namespace GLCore::Utils 
{

    class ImageLoader {

    private:
        static std::unordered_map<std::string, Image> loadedImages;

    public:
        static Image loadImage(const std::string& filepath);
        static GLuint loadImagesForCubemap(std::vector<const char*> faces);
        static GLuint loadIcon(const char* filepath);
        static GLuint loadHDR(const char* filepath);
        static void freeImage(Image& img);
    };

}
