cmake_minimum_required(VERSION 4.3)
project(NEXUS_WINDOW)

# ---------- C++ 标准 ----------
# 注意：必须在 add_library 之前设置，CMake 只在目标创建时读取该变量，
# 否则目标会退回到编译器默认标准（MSVC 为 C++14，无法使用 std::format / std::source_location）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# ---------- 链接依赖 ----------
# NEXUS_UTILITIES 头文件通过 Window.h 间接公开 → 用 PUBLIC 传递其头文件路径
target_link_libraries(NEXUS_WINDOW
    PUBLIC
        NEXUS_UTILITIES
)

# ==============================================================================
#  SDL3（Windows / Linux 分别配置）
#
#  SDL 头文件需以 PUBLIC 暴露，因为 Window.h 公开了 SDL 类型
#  （SDL_GLContext / WindowPtr 等），使用方也要能看到。
#
#  注意：导入目标（SDL3::SDL3、PkgConfig::SDL3）只在调用 find_package 的目录
#  及其子目录可见。NEXUS_WINDOW / NEXUS_RENDERING / NEXUS_CORE 是平级的兄弟目录，
#  因此每个子项目都必须各自查找，不能依赖别处查找的结果。
#  重复调用 find_package 与 pkg_check_modules 是安全的（内部有缓存）。
# ==============================================================================
if(WIN32)
    # Windows：使用仓库内 Dependencies/ 下随附的预编译库
    target_include_directories(NEXUS_WINDOW
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/include
    )
    target_link_libraries(NEXUS_WINDOW
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib/SDL3.lib
    )
else()
    # Linux：优先用 SDL3 官方提供的 CMake Config 包（能正确处理非标准安装前缀），
    # 未安装 config 包时回退到 pkg-config
    find_package(SDL3 QUIET CONFIG)

    if(TARGET SDL3::SDL3)
        target_link_libraries(NEXUS_WINDOW PUBLIC SDL3::SDL3)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
        target_link_libraries(NEXUS_WINDOW PUBLIC PkgConfig::SDL3)
    endif()
endif()
