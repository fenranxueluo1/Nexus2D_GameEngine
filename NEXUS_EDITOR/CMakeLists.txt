cmake_minimum_required(VERSION 4.3.0)
project(NEXUS_EDITOR VERSION 0.1.0 LANGUAGES C CXX)

# ---------- 编译选项 ----------
if (MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------- 引入 NEXUS_WINDOW 子项目 ----------
# NEXUS_WINDOW 会自动通过 add_subdirectory 引入 NEXUS_UTILITIES，
# 同时以 PUBLIC 方式传递它们的头文件路径与 SDL3 头文件路径。
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../NEXUS_WINDOW
                 ${CMAKE_CURRENT_BINARY_DIR}/NEXUS_WINDOW)

# ---------- 引入 GLAD 子项目 ----------
# GLAD 是独立的静态库，其 include 目录以 PUBLIC 暴露（glad/、KHR/）
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../GLAD
                 ${CMAKE_CURRENT_BINARY_DIR}/GLAD)

# ---------- 目标 ----------
add_executable(NEXUS_EDITOR src/main.cpp)

# ---------- 对外头文件 ----------
target_include_directories(NEXUS_EDITOR
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------- C++ 标准 ----------
target_compile_features(NEXUS_EDITOR
    PRIVATE
        cxx_std_20
)

# ---------- SDL3 库路径 ----------
# NEXUS_WINDOW 只把 SDL 的 include 以 PUBLIC 传递过来，
# 但 lib 目录是 PRIVATE 的，所以这里需要再指定一次链接目录。
target_link_directories(NEXUS_EDITOR
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib
)

# ---------- 链接依赖 ----------
# 链接 NEXUS_WINDOW 即可级联获得 NEXUS_UTILITIES 与 SDL3 的头文件路径
# SDL3.lib 需要在这里显式链接（最终可执行文件需要符号）
target_link_libraries(NEXUS_EDITOR
    PRIVATE
        NEXUS_WINDOW
        NEXUS_UTILITIES
        SDL3
        GLAD
)

# ---------- 拷贝 SDL3.dll 到输出目录 ----------
add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib/SDL3.dll
        $<TARGET_FILE_DIR:NEXUS_EDITOR>
)
