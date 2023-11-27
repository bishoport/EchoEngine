#pragma once

#include <iostream>
#include <memory>


template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}






#include <utility>
#include <algorithm>
#include <functional>
#include <bitset>
#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <stb_image/stb_image.h>

//IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



#include "GLCore/Core/Events/EventManager.h"



//OPENGL
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "GLCore/Core/Log.h"


#include "GLCore/Render/ShaderManager.h"
#include "GLCore/Render/FBOManager.h"

#include "GLCore/DataStruct.h"

#ifdef GLCORE_PLATFORM_WINDOWS
#include <Windows.h>
#endif


enum class MainMenuAction {
    None,                   // Representa ninguna acción
    AddDirectionalLight,    // Representa agregar una luz direccional
    AddPointLight,          // Representa agregar una luz puntual
    AddSpotLight,           // Representa agregar una luz foco
    AddCube,                // Representa agregar un cubo
    AddSegmentedCube,                // Representa agregar un cubo
    AddSphere,              // Representa agregar una esfera
    AddQuad,                // Representa agregar un quad
    AddPlane,                // Representa agregar un plane
    AddCharacterController,
    AddCarController,
    AddCamera,
    SaveProject,
    ReloadComponents,
    LiberateDLL
};

