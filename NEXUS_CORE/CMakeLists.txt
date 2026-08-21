cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_CORE)

# ---------- 目标（静态链接库） ----------
add_library(NEXUS_CORE STATIC)

# ---------- 源文件 ----------
target_sources(NEXUS_CORE
    PRIVATE
        Core/ECS/Entity.cpp
        Core/ECS/Registry.cpp
        Core/Resources/AssetManager.cpp
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

# ---------- 链接依赖 ----------
# NEXUS_RENDERING：SpriteComponent.h / AssetManager 依赖（PUBLIC，头文件路径级联传递）
# NEXUS_LOGGER：AssetManager.cpp 使用 <Logger/Logger.h>（NEXUS_ERROR 等日志宏）
# GLAD：允许在 NEXUS_CORE 源码中直接使用 <glad/gl.h> 及 GL 函数
# NEXUS_LOGGER 与 GLAD 目前仅在 NEXUS_CORE 内部（.cpp）使用，故以 PRIVATE 链接；
# 若未来 NEXUS_CORE 的公开头文件暴露 Logger 宏或 GL 类型，需将其改为 PUBLIC。
target_link_libraries(NEXUS_CORE
    PUBLIC
        NEXUS_RENDERING
    PRIVATE
        NEXUS_LOGGER
        GLAD
)
