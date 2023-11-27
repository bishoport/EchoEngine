#include "IMGLoader.h"

namespace GLCore::Utils {

    std::unordered_map<std::string, Ref<Image>> ImageLoader::loadedImages;

    Ref<Image> ImageLoader::loadImage(const std::string& filepath, const std::string carpetaBase)
    {
        // Verifica si la imagen ya ha sido cargada
        auto found = loadedImages.find(filepath);

        if (found != loadedImages.end()) {
            //std::cout << "No es necesario cargar " << filepath << " porque ya existe" << std::endl;
            return found->second;
        }

        std::string finalFilepath = filepath;

        if (!carpetaBase.empty())
        {
            std::filesystem::path rutaPath(filepath);
            std::string nombreArchivo = rutaPath.filename().string();
            std::string rutaTextura = buscarTextura(nombreArchivo, carpetaBase);

            std::cout << "nombreArchivo: " << nombreArchivo << std::endl;
            if (!rutaTextura.empty()) {
                std::cout << "Textura encontrada en: " << rutaTextura << std::endl;
                finalFilepath = rutaTextura;
            }
            else {
                std::cout << "Textura no encontrada." << std::endl;
            }
            std::cout << "-------------------------------------------------" << std::endl;
        }

        // Crear un nuevo Image en el heap
        auto image = std::make_shared<Image>();
        stbi_set_flip_vertically_on_load(false);
        image->pixels = stbi_load(finalFilepath.c_str(), &(image->width), &(image->height), &(image->channels), 0);

        if (!image->pixels)
        {
            std::cout << "Image failed to load at path: " << finalFilepath << std::endl;
            // Aquí podrías manejar el error como consideres necesario
            // Por ejemplo, podrías devolver un shared_ptr vacío: return nullptr;
        }

        image->path = finalFilepath;

        // Guardar en el mapa
        loadedImages[finalFilepath] = image;

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

    GLuint ImageLoader::load2DLUTTexture(const char* filepath) {

        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        // Forzar la carga con 3 canales (RGB)
        unsigned char* img = stbi_load(filepath, &width, &height, &channels, 3);

        if (img == NULL) {
            fprintf(stderr, "Error loading image: %s\n", filepath);
            return 0;
        }

        // Verifica que el tamaño de la imagen sea el esperado para la LUT 2D de 16x256
        if (width != 256 || height != 16) {
            fprintf(stderr, "2D LUT image must be 16x256 pixels.\n");
            stbi_image_free(img);
            return 0;
        }

        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        // Configura los parámetros de la textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Carga los datos de la imagen en la textura. Asume que la imagen es RGB.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

        // No generes MipMaps para la textura LUT, ya que normalmente no se requieren.
        // glGenerateMipmap(GL_TEXTURE_2D); // Esta línea se ha eliminado

        // Libera la memoria de la imagen
        stbi_image_free(img);

        return texID;
    }

    GLuint ImageLoader::load3DLUTTexture(const char* filepath) {
        int width, height, channels;
        unsigned char* img = stbi_load(filepath, &width, &height, &channels, 0);

        if (img == NULL) {
            fprintf(stderr, "Error loading image: %s\n", filepath);
            return 0;
        }

        // Asumimos que la imagen es cuadrada y que cada 'rebanada' también lo es.
       // Calculemos el gridSize (y por lo tanto el lutSize) basado en la imagen proporcionada
        int gridSize = std::round(std::sqrt(width)); // Esto dará el número de slices en una fila/columna
        int sliceSize = width / gridSize; // Esto dará el tamaño de un solo cuadrado (slice)
        int lutSize = gridSize; // Esto asume que la LUT es una cuadrícula perfecta y que la textura 3D es cúbica

        // Ahora verificamos si la imagen tiene la forma esperada
        if (width != height || (sliceSize * gridSize) != width) {
            fprintf(stderr, "Image does not conform to a square grid of square slices.\n");
            stbi_image_free(img);
            return 0;
        }

        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_3D, texID);

        // Set texture wrapping and filtering options.
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Allocate memory for the 3D texture.
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, sliceSize, sliceSize, lutSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        // Ahora debemos ajustar la carga de cada 'rebanada'.
        for (int z = 0; z < gridSize; ++z) {
            for (int y = 0; y < gridSize; ++y) {
                for (int x = 0; x < gridSize; ++x) {
                    int offset = ((z * gridSize + y) * width + x) * sliceSize * channels;
                    glTexSubImage3D(GL_TEXTURE_3D, 0, x * sliceSize, y * sliceSize, z, sliceSize, sliceSize, 1, GL_RGB, GL_UNSIGNED_BYTE, img + offset);
                }
            }
        }

        stbi_image_free(img);

        return texID;
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

