#include "IMGLoader.h"

namespace GLCore::Utils {

    std::unordered_map<std::string, Image> ImageLoader::loadedImages;

    Image ImageLoader::loadImage(const std::string& filepath, const std::string carpetaBase)
    {
        // Verifica si la imagen ya ha sido cargada
        auto found = loadedImages.find(filepath);

        if (found != loadedImages.end()) {
            return found->second;
        }



        std::string finalFilepath = filepath;
        if (carpetaBase != "")
        {
            std::filesystem::path rutaPath(filepath);
            // Obtener el nombre del archivo con su extensión
            std::string nombreArchivo = rutaPath.filename().string();
            std::cout << "Nombre del archivo: " << nombreArchivo << std::endl;

            std::string rutaTextura = buscarTextura(nombreArchivo, carpetaBase);

            if (!rutaTextura.empty()) {
                std::cout << "Textura encontrada en: " << rutaTextura << std::endl;
                finalFilepath = rutaTextura;
            }
            else {
                std::cout << "Textura no encontrada." << std::endl;
            }
        }


        





        Image image;
        stbi_set_flip_vertically_on_load(false);
        image.pixels = stbi_load(finalFilepath.c_str(), &(image.width), &(image.height), &(image.channels), 0);

        if (!image.pixels)
        {
            std::cout << "Image failed to load at path: " << finalFilepath.c_str() << std::endl;
        }
        image.path = finalFilepath.c_str();
        loadedImages[finalFilepath.c_str()] = image;
        return image;
    }




    GLuint ImageLoader::loadImagesForCubemap(std::vector<const char*> faces)
    {
        stbi_set_flip_vertically_on_load(false);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrComponents;


        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i], &width, &height, &nrComponents, 0);

            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    GLuint ImageLoader::loadIcon(const char* filepath)
    {
        int width, height, channels;
        unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);
        if (data)
        {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
            return texture;
        }
        else
        {
            std::cout << "Failed to load Icon texture: " << filepath << std::endl;
            return 0;
        }
    }

    GLuint ImageLoader::loadHDR(const char* filepath)
    {
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float* data = stbi_loadf(filepath, &width, &height, &nrComponents, 0);

        unsigned int hdrTexture{};

        if (data)
        {
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);

            std::cout << "Image HDR loaded successfully" << std::endl;
        }
        else
        {
            std::cout << "Failed to load HDR image." << std::endl;
        }

        return hdrTexture;
    }


    std::string ImageLoader::buscarTextura(const std::string& nombreArchivo, const std::string& carpetaBase)
    {
        // Intentar cargar la textura desde la ruta completa
        if (std::filesystem::exists(nombreArchivo)) {
            return nombreArchivo;
        }

        // Intentar cargar la textura desde la carpeta base
        auto rutaCompleta = std::filesystem::path(carpetaBase) / nombreArchivo;
        if (std::filesystem::exists(rutaCompleta)) {
            return rutaCompleta.string();
        }

        // Buscar la textura en subcarpetas de la carpeta base
        for (const auto& entrada : std::filesystem::recursive_directory_iterator(carpetaBase)) {
            if (std::filesystem::is_regular_file(entrada) && entrada.path().filename() == nombreArchivo) {
                return entrada.path().string();
            }
        }

        // No se encontró la textura
        return "";
    }



    void ImageLoader::freeImage(Image& img) {
        if (img.pixels) {
            stbi_image_free(img.pixels);
            img.pixels = nullptr;

            // Puedes elegir eliminarla del unordered_map si lo deseas
            loadedImages.erase(img.path);
        }
    }
}

