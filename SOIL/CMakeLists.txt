cmake_minimum_required(VERSION 4.3.0)
project(SOIL VERSION 0.1.0 LANGUAGES C)

# ---------- 源文件 ----------
# SOIL 以 C 语言源码构建为静态库。
# src/original/ 下是历史版本的 stb_image，不参与编译。
set(SOIL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_dxt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/soil.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_aug.c
)

add_library(SOIL STATIC ${SOIL_SOURCES})

# ---------- 头文件路径 ----------
# 暴露 include/ 目录，使用者可直接 #include <SOIL/SOIL.h>
target_include_directories(SOIL
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ---------- C 标准 ----------
set_target_properties(SOIL PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

# ---------- 编译选项 ----------
if(MSVC)
    target_compile_options(SOIL PRIVATE /W3 /utf-8)
else()
    target_compile_options(SOIL PRIVATE -Wall -Wextra)
endif()
