cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_RENDERING)

# ---------- C++ 标准 ----------
# 注意：必须在 add_library 之前设置，CMake 只在目标创建时读取该变量，
# 否则目标会退回到编译器默认标准（MSVC 为 C++14，无法使用 std::format / std::source_location）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- 目标（静态链接库） ----------
add_library(NEXUS_RENDERING STATIC)

# ---------- 源文件 ----------
target_sources(NEXUS_RENDERING
    PRIVATE
        Rendering/Essentials/Shader.cpp
        Rendering/Essentials/ShaderLoader.cpp
        Rendering/Essentials/Texture.cpp
        Rendering/Essentials/TextureLoader.cpp
        Rendering/Essentials/Vertex.h
        Rendering/Core/Camera2D.cpp
        Rendering/Core/BatchRenderer.cpp
)

# ---------- 对外头文件 ----------
# PUBLIC：使用方通过 target_link_libraries(NEXUS_RENDERING) 即可获得这些头文件路径，
# 这样可写 #include <Rendering/Essentials/ShaderLoader.h>
target_include_directories(NEXUS_RENDERING
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ---------- GLAD 与 glm 依赖 ----------
# glad/ 头文件与 GL 函数实现（GLAD 是静态库）
# glm 以 <glm/glm.hpp> 方式包含，include 路径指向 Dependencies/
target_include_directories(NEXUS_RENDERING
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../GLAD/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image/include
)

target_link_libraries(NEXUS_RENDERING
    PUBLIC
        GLAD
    PRIVATE
        NEXUS_LOGGER
        SDL3
        SDL3_image
)
