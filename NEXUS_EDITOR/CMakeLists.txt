cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_EDITOR VERSION 0.1.0 LANGUAGES C CXX)

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

# ---------- SDL3 及扩展库（image / mixer / ttf）路径 ----------
# 这些库都在 Dependencies 下，头文件在各自的 include/ 子目录，
# 链接库（.lib）在各自的 lib/ 子目录。
set(SDL_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL)
set(SDL_IMG_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_image)
set(SDL_MIX_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_mixer)
set(SDL_TTF_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL3_ttf)

# glm 的目录结构为 Dependencies/glm/glm.hpp，使用方以 <glm/glm.hpp> 方式包含，
# 故 include 路径需指向 glm 目录的父目录（Dependencies/），
# 这样 <glm/glm.hpp> 才能正确解析到 Dependencies/glm/glm.hpp。
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies)

# 头文件路径：使用方以 <SDL3_image/SDL_image.h> 等方式包含
target_include_directories(NEXUS_EDITOR
    PRIVATE
        ${GLM_DIR}
        ${SDL_IMG_DIR}/include
        ${SDL_MIX_DIR}/include
        ${SDL_TTF_DIR}/include
)

# 链接目录（.lib 所在）
target_link_directories(NEXUS_EDITOR
    PRIVATE
        ${SDL_DIR}/lib
        ${SDL_IMG_DIR}/lib
        ${SDL_MIX_DIR}/lib
        ${SDL_TTF_DIR}/lib
)

# ---------- 链接依赖 ----------
# 链接 NEXUS_WINDOW 即可级联获得 NEXUS_UTILITIES 与 SDL3 的头文件路径
# SDL3.lib 需要在这里显式链接（最终可执行文件需要符号）
target_link_libraries(NEXUS_EDITOR
    PRIVATE
        NEXUS_WINDOW
        NEXUS_UTILITIES
        NEXUS_RENDERING
        NEXUS_LOGGER
        SDL3
        SDL3_image
        SDL3_mixer
        SDL3_ttf
        GLAD
)

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
add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SDL_IMG_DIR}/lib/optional
        $<TARGET_FILE_DIR:NEXUS_EDITOR>/optional
)
add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SDL_MIX_DIR}/lib/optional
        $<TARGET_FILE_DIR:NEXUS_EDITOR>/optional
)

# ---------- 拷贝 assets 资源到输出目录 ----------
# 运行时按相对路径 assets/textures/... 加载，需保证 exe 同目录下有 assets/
add_custom_command(TARGET NEXUS_EDITOR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:NEXUS_EDITOR>/assets
)
