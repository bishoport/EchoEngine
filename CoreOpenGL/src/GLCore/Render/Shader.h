#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace GLCore::Render {

    class Shader
    {
    public:
        unsigned int ID;

        Shader() = default;

        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        Shader(const char* vertexPath, const char* fragmentPath = nullptr, const char* geometryPath = nullptr)
        {
            // 1. retrieve the vertex/fragment source code from filePath
            std::string vertexCode;
            std::string fragmentCode;
            std::string geometryCode;

            try
            {
                // load shaders with includes
                vertexCode = readFile(vertexPath);
                fragmentCode = readFile(fragmentPath);

                // if geometry shader path is present, also load a geometry shader
                if (geometryPath != nullptr)
                {
                    geometryCode = readFile(geometryPath);
                }
            }
            catch (std::exception& e)
            {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            }

            const char* vShaderCode = vertexCode.c_str();

            // 2. compile shaders
            unsigned int vertex, fragment;


            // vertex shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            checkCompileErrors(vertex, "VERTEX");


            // fragment Shader
            if (fragmentPath != nullptr) {
                const char* fShaderCode = fragmentCode.c_str();
                fragment = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragment, 1, &fShaderCode, NULL);
                glCompileShader(fragment);
                checkCompileErrors(fragment, "FRAGMENT");
            }

            // if geometry shader is given, compile geometry shader
            unsigned int geometry;
            if (geometryPath != nullptr)
            {
                const char* gShaderCode = geometryCode.c_str();
                geometry = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(geometry, 1, &gShaderCode, NULL);
                glCompileShader(geometry);
                checkCompileErrors(geometry, "GEOMETRY");
            }


            // shader Program
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            if (geometryPath != nullptr)
                glAttachShader(ID, geometry);
            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");
            // delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            if (geometryPath != nullptr)
                glDeleteShader(geometry);

        }

        void CompileFromString(const std::string& vertexCode, const std::string& fragmentCode, const std::string& geometryCode = "") {
            GLuint vertex, fragment, geometry;
            GLint success;
            GLchar infoLog[1024];

            // Compilar el vertex shader
            if (!vertexCode.empty()) {
                vertex = glCreateShader(GL_VERTEX_SHADER);
                const char* source = vertexCode.c_str();
                glShaderSource(vertex, 1, &source, NULL);
                glCompileShader(vertex);
                checkCompileErrors(vertex, "VERTEX");
            }

            // Compilar el fragment shader
            if (!fragmentCode.empty()) {
                fragment = glCreateShader(GL_FRAGMENT_SHADER);
                const char* source = fragmentCode.c_str();
                glShaderSource(fragment, 1, &source, NULL);
                glCompileShader(fragment);
                checkCompileErrors(fragment, "FRAGMENT");
            }

            // Compilar el geometry shader si se proporciona
            if (!geometryCode.empty()) {
                geometry = glCreateShader(GL_GEOMETRY_SHADER);
                const char* source = geometryCode.c_str();
                glShaderSource(geometry, 1, &source, NULL);
                glCompileShader(geometry);
                checkCompileErrors(geometry, "GEOMETRY");
            }

            // Vincular los shaders a un nuevo programa (el ID de clase)
            glUseProgram(0); // Desvincular cualquier programa actual
            if (ID != 0) {
                glDeleteProgram(ID); // Eliminar el programa anterior si existe
            }
            ID = glCreateProgram(); // Crear un nuevo ID de programa

            if (!vertexCode.empty()) glAttachShader(ID, vertex);
            if (!fragmentCode.empty()) glAttachShader(ID, fragment);
            if (!geometryCode.empty()) glAttachShader(ID, geometry);

            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");

            // Eliminar los shaders ya que están vinculados al programa y ya no son necesarios
            if (!vertexCode.empty()) glDeleteShader(vertex);
            if (!fragmentCode.empty()) glDeleteShader(fragment);
            if (!geometryCode.empty()) glDeleteShader(geometry);
        }


        // activate the shader
        // ------------------------------------------------------------------------
        void use()
        {
            glUseProgram(ID);
        }
        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string& name, bool value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }
        // ------------------------------------------------------------------------
        void setInt(const std::string& name, int value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void setFloat(const std::string& name, float value) const
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void setVec2(const std::string& name, const glm::vec2& value) const
        {
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec2(const std::string& name, float x, float y) const
        {
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        }
        // ------------------------------------------------------------------------
        void setVec3(const std::string& name, const glm::vec3& value) const
        {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec3(const std::string& name, float x, float y, float z) const
        {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }
        // ------------------------------------------------------------------------
        void setVec4(const std::string& name, const glm::vec4& value) const
        {
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec4(const std::string& name, float x, float y, float z, float w)
        {
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        }
        // ------------------------------------------------------------------------
        void setMat2(const std::string& name, const glm::mat2& mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat3(const std::string& name, const glm::mat3& mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat4(const std::string& name, const glm::mat4& mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }


    private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(GLuint shader, std::string type)
        {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM")
            {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
            else
            {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
        }
        std::string readFile(const std::string& filePath) {
            std::ifstream fileStream(filePath, std::ios::in);

            if (!fileStream.is_open()) {
                std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
                return "";
            }

            std::string content;
            std::string line = "";
            while (std::getline(fileStream, line)) {
                // Check for #include directive
                if (line.find("#include") != std::string::npos) {
                    size_t startQuotePos = line.find("\"");
                    size_t endQuotePos = line.find("\"", startQuotePos + 1);
                    if (startQuotePos != std::string::npos && endQuotePos != std::string::npos) {
                        std::string includeFilename = line.substr(startQuotePos + 1, endQuotePos - startQuotePos - 1);
                        content.append(readFile("assets/shaders/" + includeFilename)); // Recursively read the included file
                    }
                    else {
                        std::cerr << "Error processing #include directive in " << filePath << std::endl;
                    }
                }
                else {
                    content.append(line + "\n");
                }
            }

            fileStream.close();
            return content;
        } 
    };
}








        //Sin include system
        /*std::string readFile(const std::string& filePath) {
            std::ifstream fileStream(filePath, std::ios::in);

            if (!fileStream.is_open()) {
                std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
                return "";
            }

            std::string content;
            std::string line = "";
            while (!fileStream.eof()) {
                std::getline(fileStream, line);
                content.append(line + "\n");
            }

            fileStream.close();
            return content;
        }*/


        //void setVec4Array(const std::string& name, const std::array<glm::vec4, 2>& values) const
        //{
        //    glUniform4fv(glGetUniformLocation(ID, name.c_str()), values.size(), reinterpret_cast<const GLfloat*>(values.data()));
        //}


