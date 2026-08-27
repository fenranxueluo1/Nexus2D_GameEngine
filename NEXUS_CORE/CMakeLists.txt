cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_CORE)

# ---------- C++ 标准 ----------
# 注意：必须在 add_library 之前设置，CMake 只在目标创建时读取该变量，
# 否则目标会退回到编译器默认标准（MSVC 为 C++14，无法使用 std::format / std::source_location）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- 目标（静态链接库） ----------
add_library(NEXUS_CORE STATIC)

# ---------- 源文件 ----------
target_sources(NEXUS_CORE
    PRIVATE
        Core/ECS/Entity.cpp
        Core/ECS/Registry.cpp
        Core/ECS/MetaUtilities.cpp
        Core/Resources/AssetManager.cpp
        Core/Systems/ScriptingSystem.cpp
        Core/Systems/RenderSystem.cpp
        Core/ECS/Components/TransformComponent.cpp
        Core/ECS/Components/SpriteComponent.cpp
        Core/Scripting/GlmLuaBindings.cpp
        Core/ECS/Components/AnimationComponent.cpp
        Core/Systems/AnimationSystem.cpp
        
        
)

# ---------- 对外头文件 ----------
# PUBLIC：使用方通过 target_link_libraries(NEXUS_CORE) 即可获得这些头文件路径，
# 这样可写 #include <Core/ECS/Entity.h>
target_include_directories(NEXUS_CORE
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ---------- 第三方依赖 ----------
# Registry.h 使用 <entt.hpp>（位于 Dependencies/entt/）
# TransformComponent.h 使用 <glm/glm.hpp>（include 路径指向 Dependencies/）
# SpriteComponent.h 使用 <Rendering/Essentials/Vertex.h>（由 NEXUS_RENDERING 提供，
#   其会级联传递 GLAD 与 glm 的头文件路径）
# ScriptComponent.h 使用 <LuaBridge/LuaBridge.h> 与 <lua.hpp>（LuaBridge3 + Lua 5.5）
# 上述均为公共头文件依赖，故以 PUBLIC 暴露，使用方链接 NEXUS_CORE 即可获得。
target_include_directories(NEXUS_CORE
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/entt
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/LuaBridge3
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/Lua_5.5/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/include
)

# ---------- 链接依赖 ----------
# NEXUS_RENDERING：SpriteComponent.h / AssetManager 依赖（PUBLIC，头文件路径级联传递）
# NEXUS_LOGGER：AssetManager.cpp 使用 <Logger/Logger.h>（NEXUS_ERROR 等日志宏）
# GLAD：允许在 NEXUS_CORE 源码中直接使用 <glad/gl.h> 及 GL 函数
# lua55：LuaBridge3 是绑定库，需链接 Lua 5.5 静态库（liblua55.a）。
target_link_directories(NEXUS_CORE
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/Lua_5.5/lib
)

target_link_libraries(NEXUS_CORE
    PUBLIC
        NEXUS_RENDERING
        lua55
    PRIVATE
        NEXUS_LOGGER
        GLAD
)
