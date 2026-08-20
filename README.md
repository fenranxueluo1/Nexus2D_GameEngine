# Nexus2D

一个基于 C++23 的轻量级 2D 游戏引擎框架，使用 SDL3 作为窗口/事件系统，OpenGL 4.6 + GLAD 作为图形渲染后端，基于 entt 实现 ECS 实体组件系统。项目采用模块化分层设计，方便扩展与复用。

## 功能特性

- **模块化架构**：窗口、工具、渲染、核心 ECS、日志各成独立静态库，职责清晰、可独立复用
- **ECS 实体组件系统**：基于 [entt](https://github.com/skypjack/entt)，提供 `Entity` / `Registry` 封装与内置组件（变换、精灵、标识）
- **2D 渲染**：封装 OpenGL 4.6 着色器（Shader）、纹理（Texture）与 2D 正交相机（Camera2D），支持 UV 精灵绘制
- **日志系统**：提供 `NEXUS_LOG` / `NEXUS_WARN` / `NEXUS_ERROR` 宏，基于 C++20 `<format>`，错误日志自动携带源位置（`source_location`）
- **SDL3 现代化封装**：基于 `std::unique_ptr` 与自定义删除器管理 SDL 资源（`SDL_Window`、`SDL_Gamepad`、`SDL_Cursor`），杜绝手动释放遗漏
- **跨平台构建**：使用 CMake 统一管理，兼容 MSVC 与 GCC/Clang

## 技术栈

| 组件 | 技术 |
| --- | --- |
| 语言标准 | C++23 |
| 构建系统 | CMake ≥ 4.4.2 |
| 窗口/事件 | SDL3（含 image / mixer / ttf 扩展） |
| 图形 API | OpenGL 4.6 Core Profile |
| 函数加载 | GLAD |
| 数学库 | glm |
| ECS | entt（header-only） |

## 项目结构

```
Nexus2D/
├── CMakeLists.txt              # 顶层 CMake，引入 NEXUS_EDITOR 子项目
├── LICENSE                    # GPL-3.0
├── Dependencies/
│   ├── SDL/                   # SDL3 预编译库与头文件（include/ lib/）
│   ├── SDL3_image/            # SDL3 图像加载扩展（include/ lib/）
│   ├── SDL3_mixer/            # SDL3 音频混音扩展（include/ lib/）
│   ├── SDL3_ttf/              # SDL3 字体渲染扩展（include/ lib/）
│   ├── glm/                   # OpenGL 数学库（header-only）
│   └── entt/                  # ECS 实体组件系统（header-only）
├── GLAD/                      # GLAD 静态库（OpenGL 加载器）
│   ├── include/               # glad/、KHR/ 头文件
│   ├── src/glad.c
│   └── CMakeLists.txt
├── NEXUS_UTILITIES/           # 工具库（静态库）
│   ├── Nexus_Utilities/
│   │   ├── SDL_Wrappers.h     # SDL 资源智能指针封装
│   │   └── SDL_Wrappers.cpp
│   └── CMakeLists.txt
├── NEXUS_WINDOW/              # 窗口库（静态库）
│   ├── Windowing/
│   │   ├── Window/            # Window.h / Window.cpp（SDL3 窗口 + GL 上下文）
│   │   └── Iputs/             # 输入系统（预留目录）
│   └── CMakeLists.txt
├── NEXUS_LOGGER/              # 日志库（静态库）
│   ├── Logger/
│   │   ├── Logger.h           # NEXUS_LOG / NEXUS_WARN / NEXUS_ERROR 宏
│   │   ├── Logger.cpp
│   │   └── Logger.inl
│   └── CMakeLists.txt
├── NEXUS_RENDERING/           # 渲染库（静态库）
│   ├── Rendering/
│   │   ├── Essentials/        # Shader / ShaderLoader / Texture / TextureLoader / Vertex
│   │   ├── Core/              # Camera2D（2D 正交相机）
│   │   └── Buffers/           # GPU 缓冲（预留目录）
│   └── CMakeLists.txt
├── NEXUS_CORE/                # 核心库（静态库，ECS）
│   ├── Core/ECS/
│   │   ├── Entity.{h,cpp,inl} # 实体封装
│   │   ├── Registry.{h,cpp}   # 实体注册表
│   │   └── Components/        # TransformComponent / SpriteComponent / Identification
│   └── CMakeLists.txt
└── NEXUS_EDITOR/              # 可执行项目（编辑器入口）
    ├── src/
    │   └── main.cpp           # 主程序入口
    ├── assets/                # 运行时资源（构建时自动拷贝到 exe 同目录）
    │   ├── shaders/           # basicShader.vert / basicShader.frag
    │   └── textures/          # tileset.png 等
    └── CMakeLists.txt
```

## 模块依赖关系

NEXUS_EDITOR 作为顶层可执行项目，会自动级联引入所有依赖：

```
NEXUS_EDITOR (executable)
   ├── NEXUS_WINDOW  (static lib)
   │     └── NEXUS_UTILITIES ─────────┐
   ├── NEXUS_RENDERING (static lib)   │
   │     ├── GLAD                     │
   │     ├── NEXUS_LOGGER             ├── SDL3 (头文件 + lib)
   │     └── glm                      │
   ├── NEXUS_CORE  (static lib)       │
   │     └── NEXUS_RENDERING (见上)   │
   ├── NEXUS_LOGGER  (static lib)     │
   ├── GLAD  (static lib)             │
   ├── glm / entt (header-only)       │
   └── SDL3 (lib)
```

依赖以 `PUBLIC` 方式传递：链接 `NEXUS_CORE` 即自动获得 entt、glm、渲染与日志的头文件路径。

## 快速开始

### 环境要求

- **CMake** ≥ 4.4.2
- **C++23 编译器**：MSVC 19.40+ / GCC 13+ / Clang 16+
- **SDL3** 及扩展库：已随仓库附带在 `Dependencies/`（Windows）
- **OpenGL 4.6 驱动**：需硬件驱动支持

### 构建

#### Windows（PowerShell + MSVC）

```powershell
# 在仓库根目录执行
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
```

构建产物位于 `build/bin/`，静态库位于 `build/lib/`。构建后 SDL3 及扩展库的 dll、`assets/` 资源会被自动拷贝到可执行文件同目录。

#### Windows（GCC/MinGW）

```powershell
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### 运行

```powershell
./build/bin/NEXUS_EDITOR.exe
```

启动后会创建一个 640×480 的窗口，使用 ECS 实体 + 精灵组件从 `tileset.png` 裁剪并渲染一个紫色精灵，按 `ESC` 或关闭窗口即可退出。

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

### NEXUS_LOGGER

基于 C++20 `<format>` 的控制台日志系统，错误日志自动附加文件名、函数与行号（`std::source_location`）。

```cpp
NEXUS_INIT_LOGS(true, true);                                   // 初始化（控制台输出 + 保留历史）
NEXUS_LOG("加载纹理：[宽度 = {0}, 高度 = {1}]", w, h);          // 普通日志，支持格式化占位符
NEXUS_WARN("资源即将耗尽");                                     // 警告
NEXUS_ERROR("纹理创建失败！");                                  // 错误（自动带调用位置）
```

### NEXUS_RENDERING

封装 OpenGL 4.6 渲染相关组件：

- `Shader` / `ShaderLoader`：编译、链接顶点/片元着色器，提供 `Enable()` / `Disable()` / `SetUniformMat4` 等接口
- `Texture` / `TextureLoader`：纹理创建与加载（`TextureType::PIXEL` 等），支持 PNG 精灵图集
- `Camera2D`：2D 正交相机，`SetScale()` 缩放、`Update()` 刷新投影矩阵、`GetCameraMatrix()` 取矩阵
- `Vertex`：顶点结构（`position` / `uvs` / `color`）

### NEXUS_CORE

基于 entt 的 ECS 实体组件系统：

```cpp
auto pRegistry = std::make_unique<NEXUS_CORE::ECS::Registry>();

NEXUS_CORE::ECS::Entity entity{*pRegistry, "Ent1", "Test"};

auto& transform = entity.AddComponent<NEXUS_CORE::ECS::TransformComponent>(
    {.position = glm::vec2{10.f, 10.f}, .scale = glm::vec2{1.f}, .rotation = 0.f});

auto& sprite = entity.AddComponent<NEXUS_CORE::ECS::SpriteComponent>(
    {.width = 16.f, .height = 16.f, .color = {.r = 255, .g = 0, .b = 255, .a = 255},
     .start_x = 0, .start_y = 1});

auto& id = entity.GetComponent<NEXUS_CORE::ECS::Identification>();
NEXUS_LOG("名称: {}, 分类: {}, ID: {}", id.name, id.group, id.entity_id);
```

内置组件：
- `TransformComponent` —— 位置（`position`）/ 缩放（`scale`）/ 旋转（`rotation`）
- `SpriteComponent` —— 精灵尺寸（`width` / `height`）、颜色、图集裁剪（`start_x` / `start_y` / `generate_uvs`）
- `Identification` —— 实体标识（`name` / `group` / `entity_id`）

## 使用示例

创建窗口、初始化渲染并驱动一个 ECS 精灵的完整流程见 `NEXUS_EDITOR/src/main.cpp`，核心步骤：

1. `NEXUS_INIT_LOGS` 初始化日志系统
2. `SDL_Init` 初始化 VIDEO 与 EVENTS 子系统
3. `SDL_GL_SetAttribute` 配置 OpenGL 4.6 Core、缓冲区位深、双缓冲等
4. 构造 `NEXUS_WINDOWING::Window` 创建窗口
5. `SDL_GL_CreateContext` 创建并 `window.SetGLContext()` 绑定 GL 上下文
6. `gladLoadGL()` 加载 GL 函数指针
7. `TextureLoader::Create` 加载精灵图集纹理
8. 创建 `NEXUS_CORE::ECS::Registry` 与实体，添加 `TransformComponent` / `SpriteComponent` 并生成 UV
9. 构建顶点数据，创建 VAO / VBO / IBO 并配置顶点属性
10. 创建 `Camera2D` 与 `ShaderLoader`，进入事件循环渲染

## 开发路线

- [x] ECS 实体组件系统（entt）
- [x] 日志系统
- [x] 基础 2D 渲染（Shader / Texture / Camera2D）
- [ ] 输入系统（`NEXUS_WINDOW/Windowing/Iputs/`，目前为预留目录）
- [ ] GPU 缓冲封装（`NEXUS_RENDERING/Rendering/Buffers/`）
- [ ] 2D 批渲染（SpriteBatch）与场景图
- [ ] 资源管理（音频、字体加载）
- [ ] 编辑器 UI（ImGui 集成）

## 许可证

本项目基于 [GNU General Public License v3.0](./LICENSE) 开源。

SDL3 的许可证详见 `Dependencies/SDL/LICENSE.txt`。
