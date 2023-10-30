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
        static GLCore::MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions);
        static GLCore::Material ProcessMaterials(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 finalTransform, ImportOptions importOptions);

        static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
        static aiMatrix4x4 glmToAiMatrix4x4(const glm::mat4& from);

        

    };
}
