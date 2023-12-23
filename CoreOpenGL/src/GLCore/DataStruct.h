#pragma once
#include "../glpch.h"
#include <map>
#include <string>
#include "Util/SkeletalAnimation/bone.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>




namespace GLCore 
{
    enum MODEL_TYPES
    {
        NONE,
        PRIMIVITE_PLANE,
        PRIMIVITE_CUBE,
        PRIMIVITE_SEGMENTED_CUBE,
        PRIMIVITE_SPHERE,
        PRIMIVITE_QUAD,
        EXTERNAL_FILE,
    };

    inline std::string ModelTypeToString(MODEL_TYPES type) {
        switch (type) {
        case NONE: return "NONE";
        case PRIMIVITE_PLANE: return "PRIMIVITE_PLANE";
        case PRIMIVITE_CUBE: return "PRIMIVITE_CUBE";
        case PRIMIVITE_SEGMENTED_CUBE: return "PRIMIVITE_SEGMENTED_CUBE";
        case PRIMIVITE_SPHERE: return "PRIMIVITE_SPHERE";
        case PRIMIVITE_QUAD: return "PRIMIVITE_QUAD";
        case EXTERNAL_FILE: return "EXTERNAL_FILE";
        default: return "Unknown Type";
        }
    }

    enum TEXTURE_TYPES {
        ALBEDO,
        NORMAL,
        METALLIC,
        ROUGHNESS,
        AO
    };

    struct FBO_Data {
        GLuint* FBO = nullptr;
        GLuint* depthBuffer = nullptr;
        std::vector<GLuint> colorBuffers;
        ImVec2 drawPos = ImVec2(0.0f, 0.0f);
        ImVec2 drawSize = ImVec2(640.0f, 480.0f);
    };

    // Definir un struct para las opciones de importación
    struct ImportOptions {
        std::string filePath;
        std::string fileName;
        int modelID;
        bool invertUV;
        bool rotate90;
    };

    //LA VERSION EN RAM
    struct Image {
        unsigned char* pixels;
        int width, height, channels;
        std::string path = "NONE";
    };

    //LA VERSION EN GPU
    struct Texture {
        GLuint textureID = 0;
        TEXTURE_TYPES type;
        std::string typeString;
        Ref<Image> image;
        bool hasMap = false;

        void Bind() {
            if (hasMap) {

                if (textureID != 0) {
                    glDeleteTextures(1, &textureID);
                }

                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_2D, textureID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format{};
                if (image->channels == 1)
                    format = GL_RED;
                else if (image->channels == 3)
                    format = GL_RGB;
                else if (image->channels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->pixels);
                glGenerateMipmap(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    };

    struct MaterialData
    {
        std::string materialName = "none";

        //TEXTURES
        Ref<Texture> albedoMap   ;
        Ref<Texture> normalMap   ;
        Ref<Texture> metallicMap ;
        Ref<Texture> rougnessMap ;
        Ref<Texture> aOMap       ;
        //-------------------------------------------------------

        //VALUES
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

        float shininess;
        float hdrMultiply = 0.0f;
        float hdrIntensity = 0.3f;
        float exposure = 1.0f;
        float gamma = 2.2f;
        float max_reflection_lod = 4.0;
        float iblIntensity = 0.0;
        float normalIntensity = 0.5f;
        float metallicValue = 0.0f;
        float roughnessValue = 0.05f;
        float reflectanceValue = 0.04f;
        float fresnelCoefValue = 5.0f;
        //-------------------------------------------------------

        void ResetToDefaultValues()
        {
            color = glm::vec3(1.0f, 1.0f, 1.0f);

            shininess;
            hdrMultiply = 0.0f;
            hdrIntensity = 0.3f;
            exposure = 1.0f;
            gamma = 2.2f;
            max_reflection_lod = 4.0;
            iblIntensity = 0.0;
            normalIntensity = 0.5f;
            metallicValue = 0.0f;
            roughnessValue = 0.05f;
            reflectanceValue = 0.04f;
            fresnelCoefValue = 5.0f;
        }
    };


    //MODELS & SKELETAL ANIMATIONS

    #define MAX_BONE_INFLUENCE 4

    struct Vertex {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
        // tangent
        glm::vec3 Tangent;
        // bitangent
        glm::vec3 Bitangent;
        //bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        //weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];
    };

    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    struct Animation;

    struct BoneInfo
    {
        /*id is index in finalBoneMatrices*/
        int id;

        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;
    };

    struct SkeletalInfo
    {
        //Skeletal
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;
        std::vector<Ref<Vertex>> vertices;
        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }
    };

    struct MeshData 
    {
        // Constructor predeterminado
        MeshData() = default;
        ImportOptions importOptions;

        std::string meshName;

        std::vector<GLfloat> vertexBuffer;
        std::vector<Ref<Vertex>>  vertices;

        std::vector<GLuint> indices;
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        unsigned int indexCount;

        //glm::mat4 finalMatrixTransformer;
        glm::vec3 meshLocalPosition = glm::vec3(0.0f);

        glm::vec3 meshPosition;
        glm::vec3 meshRotation;
        glm::vec3 meshScale;

        glm::vec3 minBounds; // Esquina inferior delantero izquierda de la bounding box
        glm::vec3 maxBounds; // Esquina superior trasero derecha de la bounding box
        GLuint VBO_BB, VAO_BB, EBO_BB;

        void PrepareAABB()
        {
            //-----------------------------------PREPARE BOUNDING BOX LOCAL-----------------------------------
            glCreateVertexArrays(1, &VAO_BB);
            glCreateBuffers(1, &VBO_BB);
            glCreateBuffers(1, &EBO_BB);  // Crear EBO

            glVertexArrayVertexBuffer(VAO_BB, 0, VBO_BB, 0, 3 * sizeof(GLfloat));

            glm::vec3 min = minBounds;
            glm::vec3 max = maxBounds;

            glm::vec3 vertices[8] = {
                {min.x, min.y, min.z}, {max.x, min.y, min.z},
                {max.x, max.y, min.z}, {min.x, max.y, min.z},
                {min.x, min.y, max.z}, {max.x, min.y, max.z},
                {max.x, max.y, max.z}, {min.x, max.y, max.z}
            };

            glNamedBufferStorage(VBO_BB, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);  // Llenar VBO

            // Definir índices para las líneas de la Bounding Box
            GLuint indices[24] = {
                0, 1, 1, 2, 2, 3, 3, 0,  // base inferior
                4, 5, 5, 6, 6, 7, 7, 4,  // base superior
                0, 4, 1, 5, 2, 6, 3, 7   // aristas laterales
            };

            glNamedBufferStorage(EBO_BB, sizeof(indices), indices, GL_DYNAMIC_STORAGE_BIT);  // Llenar EBO

            // Asociar EBO con VAO
            glVertexArrayElementBuffer(VAO_BB, EBO_BB);

            glEnableVertexArrayAttrib(VAO_BB, 0);
            glVertexArrayAttribFormat(VAO_BB, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(VAO_BB, 0, 0);
            //-------------------------------------------------------------------------------------------------
        }
        void UpdateAABB(const glm::mat4& modelMatrix)
        {
            glm::vec3 originalVertices[8] = {
                {minBounds.x, minBounds.y, minBounds.z}, {maxBounds.x, minBounds.y, minBounds.z},
                {maxBounds.x, maxBounds.y, minBounds.z}, {minBounds.x, maxBounds.y, minBounds.z},
                {minBounds.x, minBounds.y, maxBounds.z}, {maxBounds.x, minBounds.y, maxBounds.z},
                {maxBounds.x, maxBounds.y, maxBounds.z}, {minBounds.x, maxBounds.y, maxBounds.z}
            };

            glm::vec3 transformedVertices[8];

            for (int i = 0; i < 8; ++i) {
                glm::vec4 updatedVertex = modelMatrix * glm::vec4(originalVertices[i], 1.0f);
                transformedVertices[i] = glm::vec3(updatedVertex);
            }

            glNamedBufferSubData(VBO_BB, 0, sizeof(transformedVertices), transformedVertices);
        }

        void SetupBuffers() {
            
            // Crear un vector contiguo para los datos de los vértices
            std::vector<Vertex> vertexData;
            vertexData.reserve(vertices.size());

            for (const auto& vertexPtr : vertices) {
                if (vertexPtr) {
                    vertexData.push_back(*vertexPtr);
                }
            }

            // Crear buffers/arrays
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            // Cargar datos en el buffer de vértices
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

            // Cargar datos en el buffer de elementos
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            // Configurar los atributos de los vértices
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

            glBindVertexArray(0);
        }


        void Draw()
        {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    };

    struct ModelInfo
    {
        // Constructor predeterminado
        ModelInfo() = default;

        Ref<GLCore::MeshData> meshData;
        Ref<GLCore::MaterialData> model_textures;

        MODEL_TYPES modelType;
    };

    struct ModelParent
    {
        std::string name;
        std::vector<ModelInfo> modelInfos;
        Ref<GLCore::SkeletalInfo> skeletalInfo;
    };









    struct Animation
    {
        float m_Duration;
        int m_TicksPerSecond;
        AssimpNodeData m_RootNode;

        std::vector<GLCore::Util::SkeletalAnimation::Bone> m_Bones;
        std::map<std::string, BoneInfo> m_BoneInfoMap;


        aiMatrix4x4 glmToAiMatrix4x4(const glm::mat4& from)
        {
            aiMatrix4x4 to;

            // Transponer y convertir a Assimp
            to.a1 = from[0][0]; to.a2 = from[1][0]; to.a3 = from[2][0]; to.a4 = from[3][0];
            to.b1 = from[0][1]; to.b2 = from[1][1]; to.b3 = from[2][1]; to.b4 = from[3][1];
            to.c1 = from[0][2]; to.c2 = from[1][2]; to.c3 = from[2][2]; to.c4 = from[3][2];
            to.d1 = from[0][3]; to.d2 = from[1][3]; to.d3 = from[2][3]; to.d4 = from[3][3];

            return to;
        }


        Animation(std::string animationPath, Ref<GLCore::MeshData> model)
        {
            this->meshData = model;
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            auto animation = scene->mAnimations[0];
            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
            globalTransformation = globalTransformation.Inverse();
            ReadHierarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, *model);
        }


        GLCore::Util::SkeletalAnimation::Bone* FindBone(const std::string& name)
        {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                [&](const GLCore::Util::SkeletalAnimation::Bone& Bone)
                {
                    return Bone.GetBoneName() == name;
                }
            );
            if (iter == m_Bones.end()) return nullptr;
            else return &(*iter);
        }


        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration; }
        inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
        inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
        {
            return m_BoneInfoMap;
        }



    private:

        Ref<GLCore::MeshData> meshData;



        void ReadMissingBones(const aiAnimation* animation, GLCore::MeshData& model)
        {
            //int size = animation->mNumChannels;

            //auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
            //int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

            ////reading channels(bones engaged in an animation and their keyframes)
            //for (int i = 0; i < size; i++)
            //{
            //    auto channel = animation->mChannels[i];
            //    std::string boneName = channel->mNodeName.data;

            //    if (boneInfoMap.find(boneName) == boneInfoMap.end())
            //    {
            //        boneInfoMap[boneName].id = boneCount;
            //        boneCount++;
            //    }
            //    m_Bones.push_back(GLCore::Util::SkeletalAnimation::Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
            //}

            //m_BoneInfoMap = boneInfoMap;
        }


        void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
        {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (int i = 0; i < src->mNumChildren; i++)
            {
                AssimpNodeData newData;
                ReadHierarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }
    };




}

