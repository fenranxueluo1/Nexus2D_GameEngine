cmake_minimum_required(VERSION 4.3)
project(NEXUS_EDITOR)

# ---------- C++ 标准 ----------
# 注意：必须在 add_library 之前设置，CMake 只在目标创建时读取该变量，
# 否则目标会退回到编译器默认标准（MSVC 为 C++14，无法使用 std::format / std::source_location）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- 引入 NEXUS_WINDOW 子项目 ----------
# NEXUS_WINDOW 会自动通过 add_subdirectory 引入 NEXUS_UTILITIES，
# 同时以 PUBLIC 方式传递它们的头文件路径与 SDL3 头文件路径。
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../NEXUS_WINDOW
                 ${CMAKE_CURRENT_BINARY_DIR}/NEXUS_WINDOW)

# ---------- 引入 GLAD 子项目 ----------
# GLAD 是独立的静态库，其 include 目录以 PUBLIC 暴露（glad/、KHR/）
# 注意：必须在引入 NEXUS_RENDERING 之前先引入 GLAD，
# 因为其 CMakeLists 会直接引用 GLAD 目标（同一目录不可被 add_subdirectory 两次）。
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../GLAD
                 ${CMAKE_CURRENT_BINARY_DIR}/GLAD)

# ---------- 引入 NEXUS_LOGGER 子项目（静态库） ----------
# 提供日志功能（NEXUS_LOG / NEXUS_WARN / NEXUS_ERROR），其 CMakeLists 已 PUBLIC 暴露头文件路径。
# 注意：必须在引入 NEXUS_RENDERING 之前先引入，因为 NEXUS_RENDERING 的 CMakeLists
# 会直接引用 NEXUS_LOGGER 目标（同一目录不可被 add_subdirectory 两次）。
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../NEXUS_LOGGER
                 ${CMAKE_CURRENT_BINARY_DIR}/NEXUS_LOGGER)

# ---------- 引入 NEXUS_RENDERING 子项目（静态库） ----------
# 提供渲染相关功能（Shader / ShaderLoader），其 CMakeLists 已 PUBLIC 链接 GLAD、glm 与 NEXUS_LOGGER。
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../NEXUS_RENDERING
                 ${CMAKE_CURRENT_BINARY_DIR}/NEXUS_RENDERING)

# ---------- 引入 NEXUS_CORE 子项目（静态库） ----------
# 提供 ECS 相关功能（Entity / Registry / Components），其 CMakeLists 已 PUBLIC 链接
# NEXUS_RENDERING 并暴露 entt、glm 等头文件路径。
# 注意：必须在引入 NEXUS_RENDERING 之后引入，因为其 CMakeLists 会直接引用
# NEXUS_RENDERING 目标（同一目录不可被 add_subdirectory 两次）。
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../NEXUS_CORE
                 ${CMAKE_CURRENT_BINARY_DIR}/NEXUS_CORE)

# ---------- 目标 ----------
add_executable(NEXUS_EDITOR "src/main.cpp"
                            "src/Application.cpp")

# ---------- 对外头文件 ----------
target_include_directories(NEXUS_EDITOR
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------- 与平台无关的头文件依赖 ----------
# glm 是纯头文件库，两平台都使用仓库内的版本。
# 以 <glm/glm.hpp> 方式包含，故 include 路径指向其父目录 Dependencies/。
target_include_directories(NEXUS_EDITOR
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
)

# ---------- 链接依赖 ----------
# 链接 NEXUS_WINDOW 即可级联获得 NEXUS_UTILITIES 与 SDL3 的头文件路径
target_link_libraries(NEXUS_EDITOR
    PRIVATE
        NEXUS_WINDOW
        NEXUS_UTILITIES
        NEXUS_RENDERING
        NEXUS_CORE
        NEXUS_LOGGER
        GLAD
)

# ==============================================================================
#  SDL3 及扩展库（Windows / Linux 分别配置）
#
#  最终可执行文件需要这些符号，故在此显式链接。
#
#  注意：导入目标只在调用 find_package 的目录及其子目录可见，
#  这里与其它子项目是兄弟目录，必须各自独立查找。
#  重复调用 find_package 与 pkg_check_modules 是安全的（内部有缓存）。
# ==============================================================================
if(WIN32)
    # Windows：使用仓库内 Dependencies/ 下随附的预编译库
    target_include_directories(NEXUS_EDITOR
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_mixer/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_ttf/include
    )
    # MinGW 与 MSVC 使用不同的导入库文件名（.dll.a vs .lib）
    if(MINGW)
        set(NEXUS_SDL3_LIB       ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib/libSDL3.dll.a)
        set(NEXUS_SDL3_IMAGE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image/lib/libSDL3_image.dll.a)
        set(NEXUS_SDL3_MIXER_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_mixer/lib/libSDL3_mixer.dll.a)
        set(NEXUS_SDL3_TTF_LIB   ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_ttf/lib/libSDL3_ttf.dll.a)
    else()
        set(NEXUS_SDL3_LIB       ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib/SDL3.lib)
        set(NEXUS_SDL3_IMAGE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image/lib/SDL3_image.lib)
        set(NEXUS_SDL3_MIXER_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_mixer/lib/SDL3_mixer.lib)
        set(NEXUS_SDL3_TTF_LIB   ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_ttf/lib/SDL3_ttf.lib)
    endif()
    target_link_libraries(NEXUS_EDITOR
        PRIVATE
            ${NEXUS_SDL3_LIB}
            ${NEXUS_SDL3_IMAGE_LIB}
            ${NEXUS_SDL3_MIXER_LIB}
            ${NEXUS_SDL3_TTF_LIB}
    )
else()
    # Linux：优先 CMake Config 包，未安装时回退 pkg-config
    # （例如 SDL3_mixer 在 Fedora 上只提供 pkg-config，没有 config 包）
    find_package(PkgConfig QUIET)

    # --- SDL3 ---
    find_package(SDL3 QUIET CONFIG)
    if(TARGET SDL3::SDL3)
        target_link_libraries(NEXUS_EDITOR PRIVATE SDL3::SDL3)
    else()
        pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
        target_link_libraries(NEXUS_EDITOR PRIVATE PkgConfig::SDL3)
    endif()

    # --- SDL3_image ---
    find_package(SDL3_image QUIET CONFIG)
    if(TARGET SDL3_image::SDL3_image)
        target_link_libraries(NEXUS_EDITOR PRIVATE SDL3_image::SDL3_image)
    else()
        pkg_check_modules(SDL3_image REQUIRED IMPORTED_TARGET sdl3-image)
        target_link_libraries(NEXUS_EDITOR PRIVATE PkgConfig::SDL3_image)
    endif()

    # --- SDL3_mixer ---
    # 当前代码尚未使用音频功能，故缺失时仅跳过而不中断配置
    find_package(SDL3_mixer QUIET CONFIG)
    if(TARGET SDL3_mixer::SDL3_mixer)
        target_link_libraries(NEXUS_EDITOR PRIVATE SDL3_mixer::SDL3_mixer)
    else()
        pkg_check_modules(SDL3_mixer QUIET IMPORTED_TARGET sdl3-mixer)
        if(TARGET PkgConfig::SDL3_mixer)
            target_link_libraries(NEXUS_EDITOR PRIVATE PkgConfig::SDL3_mixer)
        else()
            message(STATUS "未找到 SDL3_mixer（可选依赖，已跳过）")
        endif()
    endif()

    # --- SDL3_ttf ---
    # 当前代码尚未使用字体功能，故缺失时仅跳过而不中断配置
    find_package(SDL3_ttf QUIET CONFIG)
    if(TARGET SDL3_ttf::SDL3_ttf)
        target_link_libraries(NEXUS_EDITOR PRIVATE SDL3_ttf::SDL3_ttf)
    else()
        pkg_check_modules(SDL3_ttf QUIET IMPORTED_TARGET sdl3-ttf)
        if(TARGET PkgConfig::SDL3_ttf)
            target_link_libraries(NEXUS_EDITOR PRIVATE PkgConfig::SDL3_ttf)
        else()
            message(STATUS "未找到 SDL3_ttf（可选依赖，已跳过）")
        endif()
    endif()
endif()

# ==============================================================================
#  以下为 Windows 专有部分：把随附的运行时 dll 拷贝到可执行文件目录
#  Linux 下 SDL3 等由系统包管理器提供，位于默认动态库搜索路径，无需拷贝。
# ==============================================================================
if(WIN32)
    # Dependencies 下各库的根目录：头文件在 include/，链接库与 dll 在 lib/
    set(SDL_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL)
    set(SDL_IMG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image)
    set(SDL_MIX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_mixer)
    set(SDL_TTF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_ttf)

    # ---------- 拷贝 SDL3 及扩展库 dll 到输出目录 ----------
    # SDL3_image / SDL3_mixer / SDL3_ttf 各自 lib/ 下都附带了运行时需要的 dll
    # （含它们内部依赖的格式解码 dll，如 libpng、libjpeg、libwebp、libogg 等）
    add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL_DIR}/lib/SDL3.dll
            $<TARGET_FILE_DIR:NEXUS_EDITOR>
    )
    add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL_IMG_DIR}/lib/SDL3_image.dll
            $<TARGET_FILE_DIR:NEXUS_EDITOR>
    )
    add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL_MIX_DIR}/lib/SDL3_mixer.dll
            $<TARGET_FILE_DIR:NEXUS_EDITOR>
    )
    add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL_TTF_DIR}/lib/SDL3_ttf.dll
            $<TARGET_FILE_DIR:NEXUS_EDITOR>
    )

    # SDL3_image / SDL3_mixer 的 lib/optional/ 下是可选格式解码库
    # （libpng/libwebp/libtiff/libogg/libopus 等）。核心 dll 已支持常见格式，
    # 但加载 webp/tiff/opus 等需这些 dll，统一拷贝以防运行时缺失。
    # 注意：该目录随版本包可选提供，可能不存在，需先判断再拷贝。
    if(EXISTS ${SDL_IMG_DIR}/lib/optional)
        add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SDL_IMG_DIR}/lib/optional
                $<TARGET_FILE_DIR:NEXUS_EDITOR>/optional
        )
    endif()
    if(EXISTS ${SDL_MIX_DIR}/lib/optional)
        add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SDL_MIX_DIR}/lib/optional
                $<TARGET_FILE_DIR:NEXUS_EDITOR>/optional
        )
    endif()
endif()

# ---------- 拷贝 assets 资源到输出目录 ----------
# 运行时按相对路径 assets/textures/... 加载，需保证 exe 同目录下有 assets/
#
# 注意：不能用 add_custom_command(TARGET ... POST_BUILD)。
# POST_BUILD 只在可执行文件重新链接时才触发，若只改动了 .lua / .png 等资源文件
# 而未改动 C++ 源码，则不会有任何目标被重建，资源也就不会同步，
# 运行时加载的仍是旧脚本（表现为"改了脚本却毫无反应"）。
#
# 这里改为用 stamp 文件建立文件级依赖：任一资源文件比 stamp 新就重新拷贝。
# CONFIGURE_DEPENDS 让新增/删除资源文件时能自动重新运行 CMake，
# 从而使 GLOB 结果保持最新。
file(GLOB_RECURSE NEXUS_ASSET_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/*
)

set(NEXUS_ASSETS_STAMP ${CMAKE_CURRENT_BINARY_DIR}/assets_copy.stamp)

add_custom_command(
    OUTPUT ${NEXUS_ASSETS_STAMP}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:NEXUS_EDITOR>/assets
    COMMAND ${CMAKE_COMMAND} -E touch ${NEXUS_ASSETS_STAMP}
    DEPENDS ${NEXUS_ASSET_FILES}
    COMMENT "同步 assets 到输出目录"
    VERBATIM
)

add_custom_target(NEXUS_EDITOR_Assets ALL DEPENDS ${NEXUS_ASSETS_STAMP})
add_dependencies(NEXUS_EDITOR NEXUS_EDITOR_Assets)
