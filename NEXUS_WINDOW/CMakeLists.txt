cmake_minimum_required(VERSION 4.3.0)
project(NEXUS_WINDOW
    LANGUAGES C CXX
)

# ---------- 编译选项 ----------
if (MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------- 引入 NEXUS_UTILITIES 子项目 ----------
# 这样可以通过 target_link_libraries(NEXUS_WINDOW PRIVATE NEXUS_UTILITIES)
# 同时获得其头文件路径（PUBLIC 会传递过来）
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../NEXUS_UTILITIES
                 ${CMAKE_CURRENT_BINARY_DIR}/NEXUS_UTILITIES)

# ---------- 目标（静态库） ----------
add_library(NEXUS_WINDOW STATIC)

# ---------- 源文件 ----------
target_sources(NEXUS_WINDOW
    PRIVATE
        Windowing/Window/Window.cpp
)

# ---------- 对外头文件 ----------
# PUBLIC：使用方通过 target_link_libraries(NEXUS_WINDOW) 即可获得这些头文件路径
target_include_directories(NEXUS_WINDOW
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ---------- C++ 标准 ----------
target_compile_features(NEXUS_WINDOW
    PUBLIC
        cxx_std_20
)

# ---------- SDL3（现代方式） ----------
# SDL 头文件对使用本库的代码也可见（PUBLIC），因为 Window.h 公开了 SDL 类型
target_include_directories(NEXUS_WINDOW
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/include
)

target_link_directories(NEXUS_WINDOW
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib
)

# ---------- 链接依赖 ----------
# NEXUS_UTILITIES 头文件通过 Window.h 间接公开 → 用 PUBLIC 传递其头文件路径
# SDL3：Window.h 直接使用了 SDL 类型（SDL_GLContext / WindowPtr 等）→ 用 PUBLIC 传递
target_link_libraries(NEXUS_WINDOW
    PUBLIC
        NEXUS_UTILITIES
        SDL3
)
