#pragma once

#include <Glad/glad.h>
#include <vector>
#include "../DataStruct.h"

namespace GLCore::Render {
    class PrimitivesHelper {
    public:

        

        static MeshData CreateQuad();
        static MeshData CreatePlane();
        static MeshData CreateCube();
        static MeshData CreateSegmentedCube(int subdivisions = 1);
        static MeshData CreateSphere(float radius, unsigned int sectorCount, unsigned int stackCount);

        static void GenerateBuffers(MeshData& meshData);
        static void SetupMeshAttributes(MeshData& meshData);

    private:
        struct BoundingBox {
            glm::vec3 min;
            glm::vec3 max;
        };
        static BoundingBox CalculateBoundingBox(const std::vector<float>& vertices);
    };
}
