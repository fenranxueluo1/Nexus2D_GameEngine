# Nexus2D

一个基于 C++20 的轻量级 2D 游戏引擎框架，使用 SDL3 作为窗口/事件系统，OpenGL 4.6 + GLAD 作为图形渲染后端。项目采用模块化分层设计，方便扩展与复用。

## 功能特性

- **模块化架构**：窗口、工具、编辑器各成独立静态库/可执行项目，职责清晰
- **SDL3 现代化封装**：基于 `std::unique_ptr` 与自定义删除器管理 SDL 资源（`SDL_Window`、`SDL_Gamepad`、`SDL_Cursor`），杜绝手动释放遗漏
- **OpenGL 4.6 Core**：通过 GLAD 加载 OpenGL 函数指针，支持现代可编程管线
- **跨平台构建**：使用 CMake 统一管理，兼容 MSVC 与 GCC/Clang
- **C++20 标准**：充分利用现代 C++ 特性

## 技术栈

| 组件 | 技术 |
| --- | --- |
| 语言标准 | C++20 |
| 构建系统 | CMake ≥ 4.3.0 |
| 窗口/事件 | SDL3 |
| 图形 API | OpenGL 4.6 Core Profile |
| 函数加载 | GLAD |

## 项目结构

```
Nexus2D/
├── CMakeLists.txt              # 顶层 CMake，引入 NEXUS_EDITOR 子项目
├── LICENSE                    # GPL-3.0
├── Dependencies/
│   └── SDL/                   # SDL3 预编译库与头文件
│       ├── include/
│       ├── lib/               # SDL3.lib / SDL3.dll
│       └── LICENSE.txt
├── GLAD/                      # GLAD 静态库（OpenGL 加载器）
│   ├── include/
│   │   ├── glad/
│   │   └── KHR/
│   ├── src/glad.c
│   └── CMakeLists.txt
├── NEXUS_UTILITIES/           # 工具库（静态库）
│   ├── Nexus_Utilities/
│   │   ├── SDL_Wrappers.h     # SDL 资源智能指针封装
│   │   └── SDL_Wrappers.cpp
│   ├── NEXUS_UTILITIES.cpp
│   └── CMakeLists.txt
├── NEXUS_WINDOW/              # 窗口库（静态库）
│   ├── Windowing/
│   │   ├── Window/
│   │   │   ├── Window.h       # Window 类声明
│   │   │   └── Window.cpp
│   │   └── Iputs/             # 输入系统（预留目录）
│   └── CMakeLists.txt
└── NEXUS_EDITOR/              # 可执行项目（编辑器入口）
    ├── src/
    │   └── main.cpp           # 主程序入口
    └── CMakeLists.txt
```

## 模块依赖关系

NEXUS_EDITOR 作为顶层可执行项目，会自动级联引入所有依赖：

```
NEXUS_EDITOR (executable)
   ├── NEXUS_WINDOW (static lib)  ──┐
   │     └── NEXUS_UTILITIES ──┐    │
   ├── NEXUS_UTILITIES (static lib) │
   ├── GLAD (static lib)            ├── SDL3 (头文件 + lib)
   └── SDL3 (lib)
```

## 快速开始

### 环境要求

- **CMake** ≥ 4.3.0
- **C++20 编译器**：MSVC 19.40+ / GCC 10+ / Clang 12+
- **SDL3**：已随仓库附带在 `Dependencies/SDL/`（Windows）
- **OpenGL 4.6 驱动**：需硬件驱动支持

### 构建

#### Windows（PowerShell + MSVC）

```powershell
# 在仓库根目录执行
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
```

构建产物位于 `build/bin/`，构建后 `SDL3.dll` 会被自动拷贝到可执行文件同目录。

#### Windows（GCC/MinGW）

```powershell
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### 运行

```powershell
./build/bin/NEXUS_EDITOR.exe
```

启动后会创建一个 640×480 的蓝色测试窗口，按 `ESC` 或关闭窗口即可退出。

## 核心模块说明

### NEXUS_UTILITIES

提供 SDL3 资源的 RAII 封装，避免手动调用 `SDL_DestroyWindow` 等释放函数。

- `SDL_Destroyer`：自定义删除器，特化于 `SDL_Window*` / `SDL_Gamepad*` / `SDL_Cursor*`
- `WindowPtr`：`std::unique_ptr<SDL_Window, NEXUS_UTIL::SDL_Destroyer>` 的别名
- `Controller` / `Cursor`：基于 `std::shared_ptr` 的智能句柄

### NEXUS_WINDOW

封装 SDL3 窗口与 OpenGL 上下文管理。

```cpp
NEXUS_WINDOWING::Window window(
    "测试窗口",
    640, 480,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    true,                       // 启用 VSync
    SDL_WINDOW_OPENGL
);
```

主要接口：
- `GetWindow()` / `GetGLContext()` —— 获取原生 SDL 句柄
- `SetGLContext()` —— 绑定 OpenGL 上下文
- `GetWidth()` / `GetHeight()` / `GetXPos()` / `GetYPos()` —— 窗口几何信息
- `SetWindowName()` —— 动态修改窗口标题

### GLAD

静态库，负责加载 OpenGL 4.6 Core Profile 的函数指针。使用：

```cpp
if (gladLoadGL() == 0) {
    // 加载失败处理
}
```

## 使用示例

创建窗口并初始化 OpenGL 的完整流程见 `NEXUS_EDITOR/src/main.cpp`，核心步骤：

1. `SDL_Init` 初始化 VIDEO 与 EVENTS 子系统
2. `SDL_GL_SetAttribute` 配置 OpenGL 4.6 Core、缓冲区位深、双缓冲等
3. 构造 `NEXUS_WINDOWING::Window` 创建窗口
4. `SDL_GL_CreateContext` 创建并 `window.SetGLContext()` 绑定 GL 上下文
5. `gladLoadGL()` 加载 GL 函数指针
6. 进入事件循环，使用 `glClear` + `SDL_GL_SwapWindow` 渲染

## 开发路线

- [ ] 输入系统（`NEXUS_WINDOW/Windowing/Iputs/`，目前为预留目录）
- [ ] 资源管理（纹理、着色器、音频）
- [ ] 2D 渲染管线与场景图
- [ ] 编辑器 UI（ImGui 集成）
- [ ] ECS 实体组件系统

## 许可证

本项目基于 [GNU General Public License v3.0](./LICENSE) 开源。

SDL3 的许可证详见 `Dependencies/SDL/LICENSE.txt`。
