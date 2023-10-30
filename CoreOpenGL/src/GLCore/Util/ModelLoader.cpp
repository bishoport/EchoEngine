#include "ModelLoader.h"
#include "IMGLoader.h"
#include "../Render/PrimitivesHelper.h"

namespace GLCore::Utils
{

    GLCore::ModelParent ModelLoader::LoadModel(ImportOptions importOptions) {
        
        Assimp::Importer importer;
        
        std::string completePath = importOptions.filePath + importOptions.fileName;
        std::cout << "File ->" << completePath << std::endl;

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
            
            modelInfo.meshData = ModelLoader::ProcessMesh(mesh, scene, finalTransform, importOptions);
            modelInfo.model_material = ModelLoader::ProcessMaterials(mesh, scene, finalTransform, importOptions);
            
            modelParent.modelInfos.push_back(modelInfo);
        }

        // Luego haz lo mismo para cada uno de sus hijos
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ModelLoader::ProcessNode(node->mChildren[i], scene, modelParent, _nodeTransform, importOptions);
        }
    }



    GLCore::MeshData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions) {

        MeshData meshData;

        meshData.meshLocalPosition = glm::vec3(finalTransform.a4, finalTransform.b4, finalTransform.c4);

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
            meshData.minBounds = glm::vec3(firstVertex.x, firstVertex.y, firstVertex.z);
            meshData.maxBounds = meshData.minBounds;
        }


        // Cargando los datos de los vértices y los índices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

            //-Vertices
            glm::vec4 pos = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z,
                1);

            meshData.vertexBuffer.push_back(pos.x);
            meshData.vertexBuffer.push_back(pos.y);
            meshData.vertexBuffer.push_back(pos.z);


            //-Coordenadas de textura
            glm::vec2 texcoord = glm::vec2(0.0f, 0.0f);
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                texcoord = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                };
            }
            meshData.vertexBuffer.push_back(texcoord.x);
            meshData.vertexBuffer.push_back(texcoord.y);


            //-Normales
            glm::vec4 norm = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z,
                1);

            meshData.vertexBuffer.push_back(norm.x);
            meshData.vertexBuffer.push_back(norm.y);
            meshData.vertexBuffer.push_back(norm.z);




            // Actualización de minBounds y maxBounds
            glm::vec4 posFinal = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z,
                1);
            glm::vec3 transformedVertex = glm::vec3(posFinal.x, posFinal.y, posFinal.z);
            meshData.minBounds = glm::min(meshData.minBounds, transformedVertex);
            meshData.maxBounds = glm::max(meshData.maxBounds, transformedVertex);
        }


        //-INDICES
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                meshData.indices.push_back(face.mIndices[j]);
            }
        }
        meshData.indexCount = meshData.indices.size();


        //-MESH ID
        std::string meshNameBase = mesh->mName.C_Str();
        meshNameBase.append(" id:");
        meshData.meshName = meshNameBase + std::to_string(importOptions.modelID);


        return meshData;
    }


    GLCore::Material ModelLoader::ProcessMaterials(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions)
    {
        //EMPIEZAN LOS MATERIALES
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        Material material;

        aiString texturePath;

        //COLOR DIFUSSE
        aiColor3D color(0.f, 0.f, 0.f);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material.albedoColor.r = color.r;
        material.albedoColor.g = color.g;
        material.albedoColor.b = color.b;


        // Agregamos la carga de la textura DIFFUSSE aquí
        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Texture texture;

            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str());

            if (loadedImage.pixels != nullptr) {
                texture.image = std::move(loadedImage);
                texture.image.path = texturePath.C_Str();
            }
            material.albedoMap = texture;
        }




        // Agregamos la carga de la textura NORMAL aquí
        if (mat->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Texture texture;

            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str());

            if (loadedImage.pixels != nullptr) {
                texture.image = std::move(loadedImage);
                texture.image.path = texturePath.C_Str();
            }
            else {
                std::string completePathTexture = "assets/textures/default/default_normal.jpg";
                texture.image = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str());
                texture.image.path = texturePath.C_Str();
            }
            material.normalMap = texture;
        }
        else
        {
            std::string completePathTexture = "assets/textures/default/default_normal.jpg";
            Texture texture;
            texture.image = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str());
            texture.image.path = texturePath.C_Str();
            material.normalMap = texture;
        }




        // Agregamos la carga de la textura METALLIC aquí
        if (mat->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Texture texture;

            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str());

            if (loadedImage.pixels != nullptr) {
                texture.image = std::move(loadedImage);
                texture.image.path = texturePath.C_Str();
                material.metallicMap = texture;
            }
            material.metallicMap = texture;
        }




        // Agregamos la carga de la textura ROUGHTNESS aquí
        if (mat->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Texture texture;

            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str());

            if (loadedImage.pixels != nullptr) {
                texture.image = std::move(loadedImage);
                texture.image.path = texturePath.C_Str();
                
            }
            material.rougnessMap = texture;
        }




        // Agregamos la carga de la textura AO aquí
        if (mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Texture texture;

            auto loadedImage = GLCore::Utils::ImageLoader::loadImage(completePathTexture.c_str());

            if (loadedImage.pixels != nullptr) {
                texture.image = std::move(loadedImage);
                texture.image.path = texturePath.C_Str();
                
            }
            material.aOMap = texture;
        }

        return material;
    }

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
