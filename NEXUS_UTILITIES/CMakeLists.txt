cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_UTILITIES
    LANGUAGES C CXX
)

# ---------- 目标 ----------
add_library(NEXUS_UTILITIES STATIC)

# ---------- 源文件 ----------
target_sources(NEXUS_UTILITIES
    PRIVATE
        Nexus_Utilities/SDL_Wrappers.cpp
)

# ---------- 对外头文件 ----------
# 指向项目根目录，这样引用方可以写 #include "Nexus_Utilities/SDL_Wrappers.h"
target_include_directories(NEXUS_UTILITIES
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ---------- C++ 标准 ----------
target_compile_features(NEXUS_UTILITIES
    PUBLIC
        cxx_std_20
)

# ---------- SDL3（现代方式） ----------
target_include_directories(NEXUS_UTILITIES
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/include
)

target_link_directories(NEXUS_UTILITIES
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib
)

target_link_libraries(NEXUS_UTILITIES
    PRIVATE
        SDL3
)