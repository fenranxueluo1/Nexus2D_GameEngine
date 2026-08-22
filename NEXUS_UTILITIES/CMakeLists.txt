cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_UTILITIES)

# ---------- C++ 标准 ----------
# 注意：必须在 add_library 之前设置，CMake 只在目标创建时读取该变量，
# 否则目标会退回到编译器默认标准（MSVC 为 C++14，无法使用 std::format / std::source_location）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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