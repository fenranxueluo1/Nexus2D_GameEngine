cmake_minimum_required(VERSION 4.4.2)
project(Nexus2D)

# ---------- 全局输出目录 ----------
# 把所有构建产物集中到 build/bin，方便运行和调试
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------- 默认构建类型 ----------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---------- C++ 标准 ----------
# 设置 C++ 语言标准为 C++23
set(CMAKE_CXX_STANDARD 23)
# 强制要求 C++ 标准版本，若编译器不支持则报错
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ---------- 编译选项 ----------
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------- 引入顶层可执行项目 ----------
# NEXUS_EDITOR 会自动级联引入：
#   NEXUS_WINDOW  -> NEXUS_UTILITIES
#   GLAD
# 并链接 SDL3（位于 Dependencies/SDL）
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NEXUS_EDITOR
                 ${CMAKE_CURRENT_BINARY_DIR}/NEXUS_EDITOR)
