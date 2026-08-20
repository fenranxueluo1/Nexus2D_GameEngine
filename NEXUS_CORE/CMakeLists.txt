cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_CORE)

# ---------- 目标（静态链接库） ----------
add_library(NEXUS_CORE STATIC)

# ---------- 源文件 ----------
target_sources(NEXUS_CORE
    PRIVATE
        Core/ECS/Entity.cpp
        Core/ECS/Registry.cpp
)

# ---------- 对外头文件 ----------
# PUBLIC：使用方通过 target_link_libraries(NEXUS_CORE) 即可获得这些头文件路径，
# 这样可写 #include <Core/ECS/Entity.h>
target_include_directories(NEXUS_CORE
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ---------- C++ 标准 ----------
# 设置 C++ 语言标准为 C++23
set(CMAKE_CXX_STANDARD 23)
# 强制要求 C++ 标准版本，若编译器不支持则报错
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- 第三方依赖 ----------
# Registry.h 使用 <entt.hpp>（位于 Dependencies/entt/）
# TransformComponent.h 使用 <glm/glm.hpp>（include 路径指向 Dependencies/）
# SpriteComponent.h 使用 <Rendering/Essentials/Vertex.h>（由 NEXUS_RENDERING 提供，
#   其会级联传递 GLAD 与 glm 的头文件路径）
# 上述均为公共头文件依赖，故以 PUBLIC 暴露，使用方链接 NEXUS_CORE 即可获得。
target_include_directories(NEXUS_CORE
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/entt
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
)

target_link_libraries(NEXUS_CORE
    PUBLIC
        NEXUS_RENDERING
)
