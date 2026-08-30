cmake_minimum_required(VERSION 4.3)
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

# ---------- GLAD 与 glm ----------
# 两者都是仓库内提供的、与平台无关的依赖：
#   GLAD  —— glad/ 头文件与 GL 函数实现（静态库）
#   glm   —— 纯头文件，以 <glm/glm.hpp> 方式包含，故 include 路径指向其父目录
target_include_directories(NEXUS_RENDERING
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../GLAD/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
)

target_link_libraries(NEXUS_RENDERING
    PUBLIC
        GLAD
    PRIVATE
        NEXUS_LOGGER
)

# ==============================================================================
#  SDL3 / SDL3_image（Windows / Linux 分别配置）—— 仅内部实现使用，故为 PRIVATE
#
#  注意：导入目标只在调用 find_package 的目录及其子目录可见，
#  本库与 NEXUS_WINDOW 是兄弟目录，因此必须各自独立查找。
# ==============================================================================
if(WIN32)
    # Windows：使用仓库内 Dependencies/ 下随附的预编译库
    target_include_directories(NEXUS_RENDERING
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image/include
    )
    # MinGW 与 MSVC 使用不同的导入库文件名（.dll.a vs .lib）
    if(MINGW)
        set(NEXUS_SDL3_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib/libSDL3.dll.a)
        set(NEXUS_SDL3_IMAGE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image/lib/libSDL3_image.dll.a)
    else()
        set(NEXUS_SDL3_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib/SDL3.lib)
        set(NEXUS_SDL3_IMAGE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image/lib/SDL3_image.lib)
    endif()
    target_link_libraries(NEXUS_RENDERING
        PRIVATE
            ${NEXUS_SDL3_LIB}
            ${NEXUS_SDL3_IMAGE_LIB}
    )
else()
    # Linux：优先 CMake Config 包，回退 pkg-config
    find_package(SDL3 QUIET CONFIG)
    if(TARGET SDL3::SDL3)
        target_link_libraries(NEXUS_RENDERING PRIVATE SDL3::SDL3)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
        target_link_libraries(NEXUS_RENDERING PRIVATE PkgConfig::SDL3)
    endif()

    find_package(SDL3_image QUIET CONFIG)
    if(TARGET SDL3_image::SDL3_image)
        target_link_libraries(NEXUS_RENDERING PRIVATE SDL3_image::SDL3_image)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL3_image REQUIRED IMPORTED_TARGET sdl3-image)
        target_link_libraries(NEXUS_RENDERING PRIVATE PkgConfig::SDL3_image)
    endif()
endif()
