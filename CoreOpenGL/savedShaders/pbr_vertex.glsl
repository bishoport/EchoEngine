#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool useBones;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec4 totalPosition = vec4(pos, 1.0f); // Posicin por defecto

    if (useBones) {
        totalPosition = vec4(0.0f); // Reiniciar la posicin si usamos huesos
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1 || boneIds[i] >= MAX_BONES) 
                continue;
            
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
            totalPosition += localPosition * weights[i];
        }
    }
    
    FragPos = vec3(model * totalPosition);
    Normal = mat3(transpose(inverse(model))) * norm;
    TexCoords = tex;

    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
}







//#version 460 core
//#include "common.glsl"
//
//layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec2 aTexCoords;
//layout(location = 2) in vec3 aNormal;
//layout(location = 3) in vec4 boneIdsAsFloat; // Recibiendo boneIds como float
//layout(location = 4) in vec4 weights;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//uniform mat3 normalMatrix;
//
//out vec3 FragPos;
//out vec3 Normal;
//out vec2 TexCoords;
//
//const int MAX_BONES = 100;
//const int MAX_BONE_INFLUENCE = 4;
//uniform mat4 finalBonesMatrices[MAX_BONES];
//
//void main()
//{
//    vec4 totalPosition = vec4(0.0f);
//    ivec4 boneIds = ivec4(boneIdsAsFloat); // Convertir boneIds a enteros
//
//    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
//    {
//        if(boneIds[i] == -1) 
//            continue;
//
//        if(boneIds[i] >= MAX_BONES) 
//        {
//            totalPosition = vec4(aPos, 1.0f);
//            break;
//        }
//
//        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos, 1.0f);
//        totalPosition += localPosition * weights[i];
//        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
//    }
//
//    //FragPos = vec3(model * vec4(aPos, 1.0f));
//    FragPos = vec3(model * totalPosition);
//    Normal = mat3(transpose(inverse(model))) * aNormal;
//    TexCoords = aTexCoords;
//
//    mat4 viewModel = view * model;
//    gl_Position = projection * viewModel * totalPosition;
//}

