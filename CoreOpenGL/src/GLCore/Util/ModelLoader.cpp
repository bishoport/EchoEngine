#include "ModelLoader.h"
#include "IMGLoader.h"
#include "../Render/PrimitivesHelper.h"



namespace GLCore::Utils
{
    GLCore::ModelParent ModelLoader::LoadModel(ImportOptions importOptions) {
        
        Assimp::Importer importer;

        std::cout << "File ->" << importOptions.fileName << std::endl;
        std::cout << "File path ->" << importOptions.filePath << std::endl;

        std::string completePath = importOptions.filePath + importOptions.fileName;
        std::cout << "Complete Path ->" << completePath << std::endl;

        unsigned int flags = aiProcess_Triangulate;

        flags |= aiProcess_CalcTangentSpace;
        flags |= aiProcess_GenSmoothNormals;
        flags |= aiProcess_ValidateDataStructure;
        flags |= aiProcess_GenBoundingBoxes;

        if (importOptions.invertUV == true)
        {
            flags |= aiProcess_FlipUVs;
        }

        const aiScene* scene = importer.ReadFile(completePath, flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            throw std::runtime_error("Failed to load model");
        }


        aiMatrix4x4 nodeTransform = scene->mRootNode->mTransformation;


        ModelParent modelParent;
        modelParent.name = importOptions.fileName;

        ModelLoader::ProcessNode(scene->mRootNode, scene, modelParent, nodeTransform, importOptions);

        return modelParent;
    }

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, ModelParent& modelParent, aiMatrix4x4 _nodeTransform, ImportOptions importOptions)
    {
        // Convertir a glm
        glm::mat4 glmNodeTransform = aiMatrix4x4ToGlm(_nodeTransform);
        glm::mat4 glmNodeTransformation = aiMatrix4x4ToGlm(node->mTransformation);

        float globalRotationDeg_X = 0.0f;

        if (importOptions.rotate90)
        {
            globalRotationDeg_X = -90.0f;
        }
        // Crear una matriz de rotación para la rotación de -90 grados alrededor del eje X
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(globalRotationDeg_X), glm::vec3(1.0f, 0.0f, 0.0f));

        // Aplicar la rotación antes de la transformación del nodo
        glm::mat4 glmFinalTransform = rotationX * glmNodeTransform * glmNodeTransformation;

        // Convertir de nuevo a Assimp
        aiMatrix4x4 finalTransform = glmToAiMatrix4x4(glmFinalTransform);


        // Procesa todas las mallas (si las hay) en este nodo
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ModelInfo modelInfo;
            
            std::cout << "mNumBones->" << mesh->mNumBones << std::endl;

            modelInfo.meshData = ModelLoader::ProcessMesh(mesh, scene, finalTransform, importOptions);
            modelInfo.model_textures = ModelLoader::ProcessMaterials(mesh, scene, finalTransform, importOptions);
            
            modelParent.modelInfos.push_back(modelInfo);
        }

        // Luego haz lo mismo para cada uno de sus hijos
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ModelLoader::ProcessNode(node->mChildren[i], scene, modelParent, _nodeTransform, importOptions);
        }
    }

    Ref<GLCore::MeshData> ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions) 
    {
        auto meshData = std::make_shared<GLCore::MeshData>();

        meshData->meshLocalPosition = glm::vec3(finalTransform.a4, finalTransform.b4, finalTransform.c4);


        //Reset de la posicion original para que nos devuelva la matriz en la posicion 0,0,0
        finalTransform.a4 = 0.0;
        finalTransform.b4 = 0.0;
        finalTransform.c4 = 0.0;

        // Inicialización de minBounds y maxBounds con el primer vértice transformado
        if (mesh->mNumVertices > 0) {
            glm::vec4 firstVertex = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                mesh->mVertices[0].x,
                mesh->mVertices[0].y,
                mesh->mVertices[0].z,
                1);
            meshData->minBounds = glm::vec3(firstVertex.x, firstVertex.y, firstVertex.z);
            meshData->maxBounds = meshData->minBounds;
        }


        // Cargando los datos de los vértices y los índices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
        {
            
            auto vertex = std::make_shared<Vertex>();
            
            SetVertexBoneDataToDefault(vertex);

            //--Vertex Position
            glm::vec4 posFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z,
                1);
            vertex->Position = glm::vec3(posFixed.x, posFixed.y, posFixed.z);
            //vertex->Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
            meshData->vertexBuffer.push_back(posFixed.x);
            meshData->vertexBuffer.push_back(posFixed.y);
            meshData->vertexBuffer.push_back(posFixed.z);
            //--------------------------------------------------------------



            //--Texture Coords
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex->TexCoords = vec;
            }
            else
                vertex->TexCoords = glm::vec2(0.0f, 0.0f);

            meshData->vertexBuffer.push_back(vertex->TexCoords.x);
            meshData->vertexBuffer.push_back(vertex->TexCoords.y);
            //--------------------------------------------------------------


            //--Vertex Normal
            glm::vec4 normFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z,
                1);

            vertex->Normal = glm::vec3(normFixed.x, normFixed.y, normFixed.z);
            meshData->vertexBuffer.push_back(normFixed.x);
            meshData->vertexBuffer.push_back(normFixed.y);
            meshData->vertexBuffer.push_back(normFixed.z);
            //--------------------------------------------------------------

            meshData->vertices.push_back(vertex);


            // Actualización de minBounds y maxBounds
            glm::vec4 posFinal = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z,
                1);
            glm::vec3 transformedVertex = glm::vec3(posFinal.x, posFinal.y, posFinal.z);
            meshData->minBounds = glm::min(meshData->minBounds, transformedVertex);
            meshData->maxBounds = glm::max(meshData->maxBounds, transformedVertex);
        }


        //-INDICES
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                meshData->indices.push_back(face.mIndices[j]);
            }
        }
        meshData->indexCount = meshData->indices.size();


        //-MESH ID
        std::string meshNameBase = mesh->mName.C_Str();
        meshNameBase.append(" id:");
        meshData->meshName = meshNameBase + std::to_string(importOptions.modelID);

        meshData->SetupBuffers();

        //GLCore::Render::PrimitivesHelper::GenerateBuffers(*meshData);
        //GLCore::Render::PrimitivesHelper::SetupMeshAttributes(*meshData);

        meshData->PrepareAABB();

        ExtractBoneWeightForVertices(mesh, scene, meshData);

        return meshData;
    }


    Ref<MaterialData> ModelLoader::ProcessMaterials(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions)
    {
        //EMPIEZAN LOS MATERIALES
        Ref<MaterialData> materialData  = std::make_shared<GLCore::MaterialData>();
        

        //COLOR DIFUSSE
        aiColor3D color(0.f, 0.f, 0.f);
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        materialData->color.r = color.r;
        materialData->color.g = color.g;
        materialData->color.b = color.b;

        // Agregamos la carga de la textura ALBEDO aquí
        aiString texturePath;
        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();
            
            //Texture texture;
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            
            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str(), importOptions.filePath);
            
            if (loadedImage->pixels != nullptr) {
                texture->image = std::move(loadedImage);
                texture->image->path = completePathTexture;
                texture->hasMap = true;
                materialData->albedoMap = texture;
            }
            else
            {
                std::string defaultPathTexture = "assets/default/default_white.jpg";
                texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
                texture->image->path = defaultPathTexture;
                texture->hasMap = true;
                materialData->albedoMap = texture;
            }
        }
        else
        {
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            std::string defaultPathTexture = "assets/default/default_white.jpg";
            texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
            texture->image->path = defaultPathTexture;
            texture->hasMap = true;
            materialData->albedoMap = texture;
        }




        // Agregamos la carga de la textura NORMAL aquí
        if (mat->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Ref<Texture> texture = std::make_shared<GLCore::Texture>();

            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str(), importOptions.filePath);

            if (loadedImage->pixels != nullptr) {
                texture->image = std::move(loadedImage);
                texture->image->path = completePathTexture;
                texture->hasMap = true;
                materialData->normalMap = texture;
            }
            else 
            {
                std::string defaultPathTexture = "assets/default/default_normal.jpg";
                texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
                texture->image->path = defaultPathTexture;
                texture->hasMap = true;
                materialData->normalMap = texture;
            }
        }
        else
        {
            std::string defaultPathTexture = "assets/default/default_normal.jpg";
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
            texture->image->path = defaultPathTexture;
            texture->hasMap = true;
            materialData->normalMap = texture;
        }




        // Agregamos la carga de la textura METALLIC aquí
        if (mat->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str(), importOptions.filePath);
            if (loadedImage->pixels != nullptr) {
                texture->image = std::move(loadedImage);
                texture->image->path = completePathTexture;
                texture->hasMap = true;
                materialData->metallicMap = texture;
            }
            else
            {
                std::string defaultPathTexture = "assets/default/default_black.jpg";
                texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
                texture->image->path = defaultPathTexture;
                texture->hasMap = true;
                materialData->metallicMap = texture;
            }
        }
        else
        {
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            std::string defaultPathTexture = "assets/default/default_black.jpg";
            texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
            texture->image->path = defaultPathTexture;
            texture->hasMap = true;
            materialData->metallicMap = texture;
        }




        // Agregamos la carga de la textura ROUGHTNESS aquí
        if (mat->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str(), importOptions.filePath);
            if (loadedImage->pixels != nullptr) {
                texture->image = std::move(loadedImage);
                texture->image->path = completePathTexture;
                texture->hasMap = true;
                materialData->rougnessMap = texture;
            } 
            else
            {
                std::string defaultPathTexture = "assets/default/default_black.jpg";
                texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
                texture->image->path = defaultPathTexture;
                texture->hasMap = true;
                materialData->rougnessMap = texture;
            }
        }
        else
        {
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            std::string defaultPathTexture = "assets/default/default_black.jpg";
            texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
            texture->image->path = defaultPathTexture;
            texture->hasMap = true;
            materialData->rougnessMap = texture;
        }





        // Agregamos la carga de la textura AO aquí
        if (mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Ref<Texture> texture = std::make_shared<GLCore::Texture>();

            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str(), importOptions.filePath);

            if (loadedImage->pixels != nullptr) {
                texture->image = std::move(loadedImage);
                texture->image->path = completePathTexture;
                texture->hasMap = true;
                materialData->aOMap = texture;
            }  
            else
            {
                std::string defaultPathTexture = "assets/default/default_withe.jpg";
                texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
                texture->image->path = defaultPathTexture;
                texture->hasMap = true;
                materialData->aOMap = texture;
            }
        }
        else
        {
            Ref<Texture> texture = std::make_shared<GLCore::Texture>();
            std::string defaultPathTexture = "assets/default/default_white.jpg";
            texture->image = GLCore::Utils::ImageLoader::loadImage(defaultPathTexture);
            texture->image->path = defaultPathTexture;
            texture->hasMap = true;
            materialData->aOMap = texture;
        }

        materialData->albedoMap->typeString   = "ALBEDO";
        materialData->normalMap->typeString   = "NORMAL";
        materialData->metallicMap->typeString = "METALLIC";
        materialData->rougnessMap->typeString = "ROUGHNESS";
        materialData->aOMap->typeString       = "AO";

        materialData->albedoMap->type          = TEXTURE_TYPES::ALBEDO;
        materialData->normalMap->type          = TEXTURE_TYPES::NORMAL;
        materialData->metallicMap->type        = TEXTURE_TYPES::METALLIC;
        materialData->rougnessMap->type        = TEXTURE_TYPES::ROUGHNESS;
        materialData->aOMap->type              = TEXTURE_TYPES::AO;

        materialData->albedoMap->Bind();
        materialData->normalMap->Bind();
        materialData->metallicMap->Bind();
        materialData->rougnessMap->Bind();
        materialData->aOMap->Bind();


        return materialData;
    }



    //--Skekeletal
    void ModelLoader::SetVertexBoneDataToDefault(Ref<Vertex> vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex->m_BoneIDs[i] = -1;
            vertex->m_Weights[i] = 0.0f;
        }
    }


    void ModelLoader::SetVertexBoneData(Ref<Vertex> vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex->m_BoneIDs[i] < 0)
            {
                vertex->m_Weights[i] = weight;
                vertex->m_BoneIDs[i] = boneID;
                break;
            }
        }
    }


    void ModelLoader::ExtractBoneWeightForVertices(aiMesh* mesh, const aiScene* scene, Ref<GLCore::MeshData> meshData)
    {
        /*auto& boneInfoMap = meshData->m_BoneInfoMap;
        int& boneCount = meshData->m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= meshData->vertices.size());
                SetVertexBoneData(meshData->vertices[vertexId], boneID, weight);
            }
        }*/
    }

    //--Tools
    glm::mat4 ModelLoader::aiMatrix4x4ToGlm(const aiMatrix4x4& from)
    {
        glm::mat4 to;

        // Transponer y convertir a glm
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

        return to;
    }
    aiMatrix4x4 ModelLoader::glmToAiMatrix4x4(const glm::mat4& from)
    {
        aiMatrix4x4 to;

        // Transponer y convertir a Assimp
        to.a1 = from[0][0]; to.a2 = from[1][0]; to.a3 = from[2][0]; to.a4 = from[3][0];
        to.b1 = from[0][1]; to.b2 = from[1][1]; to.b3 = from[2][1]; to.b4 = from[3][1];
        to.c1 = from[0][2]; to.c2 = from[1][2]; to.c3 = from[2][2]; to.c4 = from[3][2];
        to.d1 = from[0][3]; to.d2 = from[1][3]; to.d3 = from[2][3]; to.d4 = from[3][3];

        return to;
    }
}
