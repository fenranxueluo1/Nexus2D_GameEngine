cmake_minimum_required(VERSION 4.3)
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

# ---------- 平台相关系统库 ----------
# glad.c 的加载器在 Linux 下通过 dlopen("libGL.so.1") 获取 GL 函数指针，
# 需要链接 libdl（Windows 下走 LoadLibraryW，CMAKE_DL_LIBS 为空）。
target_link_libraries(GLAD
    PUBLIC
        ${CMAKE_DL_LIBS}
)