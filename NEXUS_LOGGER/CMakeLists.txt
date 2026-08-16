cmake_minimum_required(VERSION 4.4.2)
project(NEXUS_LOGGER
    LANGUAGES C CXX
)

# ---------- 目标（静态链接库） ----------
add_library(NEXUS_LOGGER STATIC)

# ---------- 源文件 ----------
# Logger.ini 是模板实现（被 Logger.h 末尾 #include），属于头文件，不参与独立编译。
target_sources(NEXUS_LOGGER
    PRIVATE
        Logger/Logger.cpp
)

# ---------- 对外头文件 ----------
# PUBLIC：使用方通过 target_link_libraries(NEXUS_LOGGER) 即可获得这些头文件路径，
# 这样可写 #include <Logger/Logger.h>
target_include_directories(NEXUS_LOGGER
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ---------- C++ 标准 ----------
target_compile_features(NEXUS_LOGGER
    PUBLIC
        cxx_std_20
)

# ---------- Windows API ----------
# Logger 使用 <Windows.h> / GetStdHandle / SetConsoleTextAttribute 等 Win32 API，
# 在 Windows 平台上由系统 SDK 直接提供，无需额外链接第三方库。
