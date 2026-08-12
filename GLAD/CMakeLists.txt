cmake_minimum_required(VERSION 4.4.2)
project(GLAD LANGUAGES C)

# ---------- 源文件 ----------
set(SOURCE_FILES
    src/glad.c
)

# ---------- 生成静态库 ----------
add_library(GLAD STATIC ${SOURCE_FILES})

# ---------- 对外暴露头文件路径 ----------
target_include_directories(GLAD
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)