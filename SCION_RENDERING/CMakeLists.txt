cmake_minimum_required(VERSION 4.4.2)
project(SCION_RENDERING
    LANGUAGES C CXX
)

# ---------- 目标（静态链接库） ----------
add_library(SCION_RENDERING STATIC)

# ---------- 源文件 ----------
target_sources(SCION_RENDERING
    PRIVATE
        Rendering/Essentials/Shader.cpp
        Rendering/Essentials/ShaderLoader.cpp
)

# ---------- 对外头文件 ----------
# PUBLIC：使用方通过 target_link_libraries(SCION_RENDERING) 即可获得这些头文件路径，
# 这样可写 #include <Rendering/Essentials/ShaderLoader.h>
target_include_directories(SCION_RENDERING
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ---------- C++ 标准 ----------
target_compile_features(SCION_RENDERING
    PUBLIC
        cxx_std_20
)

# ---------- GLAD 与 glm 依赖 ----------
# glad/ 头文件与 GL 函数实现（GLAD 是静态库）
# glm 以 <glm/glm.hpp> 方式包含，include 路径指向 Dependencies/
target_include_directories(SCION_RENDERING
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../GLAD/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
)

target_link_libraries(SCION_RENDERING
    PUBLIC
        GLAD
)
