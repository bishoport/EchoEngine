#pragma once

#include "../../glpch.h"
#include <stb_image/stb_image.h>
#include <unordered_map>  
#include <vector>
#include <string>
#include "../DataStruct.h"
#include <glm/glm.hpp>



#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace GLCore::Utils
{
    class ModelLoader {

    public:
        static GLCore::ModelParent LoadModel(ImportOptions importOptions);

    private:
        static void ProcessNode(aiNode* node, const aiScene* scene, ModelParent& modelParent, aiMatrix4x4 _nodeTransform, ImportOptions importOptions);

        static Ref<GLCore::MeshData> ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions);
        static Ref<MaterialData> ProcessMaterials(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions);

        static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
        static aiMatrix4x4 glmToAiMatrix4x4(const glm::mat4& from);

        static void SetVertexBoneDataToDefault(Ref<Vertex> vertex);
        static void SetVertexBoneData(Ref<Vertex> vertex, int boneID, float weight);
        static void ExtractBoneWeightForVertices(aiMesh* mesh, const aiScene* scene, Ref<GLCore::MeshData> meshData);

    };
}
