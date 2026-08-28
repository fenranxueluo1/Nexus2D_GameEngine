# Nexus2D

一个基于 C++23 的轻量级 2D 游戏引擎框架，使用 SDL3 作为窗口/事件系统，OpenGL 4.6 + GLAD 作为图形渲染后端，基于 entt 实现 ECS 实体组件系统，并通过 LuaBridge3 提供 Lua 脚本驱动能力。项目采用模块化分层设计，方便扩展与复用。

**支持 Windows 与 Linux 双平台**，CMake 会自动根据平台选择依赖来源：Windows 使用仓库内随附的预编译库，Linux 使用系统安装的库。

## 功能特性

- **跨平台构建**：Windows（MSVC / MinGW）与 Linux（GCC / Clang）双平台支持，各子项目的 CMakeLists 内以 `if(WIN32)` / `else()` 分别配置依赖，平台差异就地可见
- **模块化架构**：窗口、工具、渲染、核心 ECS、日志各成独立静态库，职责清晰、可独立复用
- **ECS 实体组件系统**：基于 [entt](https://github.com/skypjack/entt)，提供 `Entity` / `Registry` 封装与内置组件（变换、精灵、标识、动画）
- **Lua 脚本系统**：基于 [LuaBridge3](https://github.com/kunitoki/LuaBridge3)，可在 Lua 中创建实体、挂载组件、查询视图，并以 `update` / `render` 回调驱动逻辑
- **2D 批渲染**：`BatchRenderer` 按图层排序精灵并合并批次，封装 VAO / VBO / IBO
- **2D 渲染**：封装 OpenGL 4.6 着色器（Shader）、纹理（Texture）与 2D 正交相机（Camera2D），支持 UV 精灵图集裁剪
- **资产管理**：`AssetManager` 统一管理纹理与着色器，按名称索引
- **日志系统**：提供 `NEXUS_LOG` / `NEXUS_WARN` / `NEXUS_ERROR` 宏，基于 C++20 `<format>`，错误日志自动携带源位置（`source_location`），控制台输出按级别着色（Windows 用 Win32 控制台属性，Linux 用 ANSI 转义序列）
- **SDL3 现代化封装**：基于 `std::unique_ptr` 与自定义删除器管理 SDL 资源（`SDL_Window`、`SDL_Gamepad`、`SDL_Cursor`），杜绝手动释放遗漏

## 技术栈

| 组件 | 技术 |
| --- | --- |
| 语言标准 | C++23 |
| 构建系统 | CMake ≥ 4.3 |
| 窗口/事件 | SDL3（含 image / mixer / ttf 扩展） |
| 图形 API | OpenGL 4.6 Core Profile |
| 函数加载 | GLAD |
| 数学库 | glm |
| ECS | entt（header-only） |
| 脚本 | Lua + LuaBridge3（header-only 绑定层） |

## 平台支持

| 平台 | 编译器 | 依赖来源 | 状态 |
| --- | --- | --- | --- |
| Windows 10/11 | MSVC 19.40+ / MinGW | 仓库内 `Dependencies/` 预编译库（`.lib` / `.dll`） | 支持 |
| Linux | GCC 13+ / Clang 16+ | 系统库（CMake Config 包或 pkg-config） | 支持（Fedora 44 实测通过） |

### 依赖配置方式

平台差异不集中在单独文件，而是**就地写在每个子项目的 CMakeLists.txt 中**，用 `if(WIN32)` / `else()` 分支分别配置。这样打开任一子项目即可看清它依赖什么、各平台如何取得：

```cmake
if(WIN32)
    # Windows：仓库内 Dependencies/ 下随附的预编译库
    target_include_directories(NEXUS_WINDOW PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/include)
    target_link_libraries(NEXUS_WINDOW PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/SDL/lib/SDL3.lib)
else()
    # Linux：优先 CMake Config 包，未安装时回退 pkg-config
    find_package(SDL3 QUIET CONFIG)
    if(TARGET SDL3::SDL3)
        target_link_libraries(NEXUS_WINDOW PUBLIC SDL3::SDL3)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
        target_link_libraries(NEXUS_WINDOW PUBLIC PkgConfig::SDL3)
    endif()
endif()
```

各依赖的取得方式：

| 依赖 | Windows | Linux |
| --- | --- | --- |
| SDL3 / SDL3_image | `Dependencies/` 预编译库 | 系统库（必需） |
| SDL3_mixer / SDL3_ttf | `Dependencies/` 预编译库 | 系统库（可选，缺失时跳过） |
| Lua | `Dependencies/Lua_5.5`（5.5） | 系统库（Fedora 为 5.4） |
| entt / glm / LuaBridge3 | 仓库内 header-only，无平台差异 | 同左 |
| GLAD | 仓库内源码 | 仓库内源码（额外链接 `libdl`） |

> **为何每个子项目都要各自 `find_package`**：CMake 的导入目标（如 `SDL3::SDL3`、`PkgConfig::SDL3`）只在**调用查找命令的目录及其子目录**内可见。而 `NEXUS_WINDOW` / `NEXUS_RENDERING` / `NEXUS_CORE` / `NEXUS_EDITOR` 是通过 `NEXUS_EDITOR` 平级引入的**兄弟目录**，彼此看不到对方查到的目标，因此必须各自查找。重复调用 `find_package` 与 `pkg_check_modules` 是安全的（内部有缓存，不会重复创建目标）。

> **为何要 pkg-config 回退**：并非所有库都提供 CMake Config 包，例如 Fedora 上的 SDL3_mixer 只有 pkg-config。保留回退路径可避免因缺少 config 包而中断配置。

> **Lua 版本差异**：Windows 使用随附的 5.5，Linux 使用系统发行版提供的 5.4。项目仅使用 `luaL_newstate` / `luaL_openlibs` / `lua_close` / `lua_pop` / `lua_tostring` 等稳定 API，LuaBridge3 对两者均兼容，实测无差异。

## 项目结构

```
Nexus2D/
├── CMakeLists.txt              # 顶层 CMake，设置标准/输出目录并引入 NEXUS_EDITOR
├── LICENSE                     # GPL-3.0
├── Dependencies/
│   ├── SDL/                    # SDL3 预编译库与头文件（Windows）
│   ├── SDL3_image/             # SDL3 图像加载扩展（Windows）
│   ├── SDL3_mixer/             # SDL3 音频混音扩展（Windows）
│   ├── SDL3_ttf/               # SDL3 字体渲染扩展（Windows）
│   ├── Lua_5.5/                # Lua 5.5 预编译库与头文件（Windows）
│   ├── LuaBridge3/             # Lua 绑定层（header-only，两平台共用）
│   ├── glm/                    # OpenGL 数学库（header-only）
│   └── entt/                   # ECS 实体组件系统（header-only）
├── GLAD/                       # GLAD 静态库（OpenGL 加载器）
│   ├── include/                # glad/、KHR/ 头文件
│   ├── src/glad.c
│   └── CMakeLists.txt
├── NEXUS_UTILITIES/            # 工具库（静态库）
│   ├── Nexus_Utilities/
│   │   ├── SDL_Wrappers.h      # SDL 资源智能指针封装
│   │   └── SDL_Wrappers.cpp
│   └── CMakeLists.txt
├── NEXUS_WINDOW/               # 窗口库（静态库）
│   ├── Windowing/
│   │   ├── Window/             # Window.h / Window.cpp（SDL3 窗口 + GL 上下文）
│   │   └── Iputs/              # 输入系统（预留目录）
│   └── CMakeLists.txt
├── NEXUS_LOGGER/               # 日志库（静态库，跨平台控制台着色）
│   ├── Logger/
│   │   ├── Logger.h            # NEXUS_LOG / NEXUS_WARN / NEXUS_ERROR 宏
│   │   ├── Logger.cpp
│   │   └── Logger.inl
│   └── CMakeLists.txt
├── NEXUS_RENDERING/            # 渲染库（静态库）
│   ├── Rendering/
│   │   ├── Essentials/         # Shader / ShaderLoader / Texture / TextureLoader / Vertex
│   │   ├── Core/               # Camera2D（2D 正交相机）、BatchRenderer（批渲染）
│   │   └── Buffers/            # GPU 缓冲（预留目录）
│   └── CMakeLists.txt
├── NEXUS_CORE/                 # 核心库（静态库，ECS + 脚本）
│   ├── Core/ECS/
│   │   ├── Entity.{h,cpp,inl}  # 实体封装（含 Lua 元注册）
│   │   ├── Registry.{h,cpp,inl}# 实体注册表与上下文
│   │   └── Components/         # Transform / Sprite / Identification / Animation / Script
│   ├── Core/Resources/         # AssetManager（纹理与着色器管理）
│   ├── Core/Scripting/         # GlmLuaBindings（glm 类型的 Lua 绑定）
│   ├── Core/Systems/           # RenderSystem / ScriptingSystem / AnimationSystem
│   └── CMakeLists.txt
└── NEXUS_EDITOR/               # 可执行项目（编辑器入口）
    ├── src/
    │   ├── main.cpp            # 主程序入口
    │   └── Application.{h,cpp} # 应用生命周期（初始化 / 事件 / 更新 / 渲染 / 清理）
    ├── assets/                 # 运行时资源（构建时自动拷贝到可执行文件同目录）
    │   ├── scripts/main.lua    # Lua 主脚本
    │   ├── shaders/            # basicShader.vert / basicShader.frag
    │   └── textures/           # castle.png / player.png
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
   │     ├── NEXUS_LOGGER             ├── SDL3 (+ SDL3_image)
   │     └── glm                      │
   ├── NEXUS_CORE  (static lib)       │
   │     ├── NEXUS_RENDERING (见上)   │
   │     └── Lua + LuaBridge3         │
   ├── NEXUS_LOGGER  (static lib)     │
   ├── GLAD  (static lib)             │
   ├── glm / entt / LuaBridge3        │
   └── SDL3 + Lua
```

依赖以 `PUBLIC` 方式传递：链接 `NEXUS_CORE` 即自动获得 entt、glm、Lua 及渲染与日志的头文件路径。

## 快速开始

### 环境要求

- **CMake** ≥ 4.3
- **C++23 编译器**：MSVC 19.40+ / GCC 13+ / Clang 16+
- **OpenGL 4.6 驱动**：需硬件驱动支持

平台专属依赖：

**Windows** —— 已随仓库附带在 `Dependencies/`，无需额外安装。

**Linux** —— 需要安装 SDL3 与 Lua 开发包：

```bash
# Fedora / RHEL
sudo dnf install SDL3-devel SDL3_image-devel SDL3_mixer-devel SDL3_ttf-devel lua-devel

# Debian / Ubuntu
sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-mixer-dev libsdl3-ttf-dev liblua5.4-dev
```

其中 `SDL3_mixer` 与 `SDL3_ttf` 为可选依赖：当前代码尚未使用，缺失时 CMake 会跳过而不中断配置。

### 构建

#### Windows（Visual Studio + MSVC）

```powershell
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
```

#### Windows（MinGW）

```powershell
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

#### Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

构建产物位于 `build/bin/`，静态库位于 `build/lib/`。

- **Windows**：构建后 SDL3 及扩展库的 dll（`optional/` 下的格式解码库一并拷贝）与 `assets/` 资源会自动拷贝到可执行文件同目录
- **Linux**：SDL3 等由系统包管理器提供，位于默认动态库搜索路径，无需拷贝；仅 `assets/` 资源会被拷贝

### 运行

```bash
# Linux
./build/bin/NEXUS_EDITOR

# Windows
./build/bin/NEXUS_EDITOR.exe
```

启动后会创建一个 640×480 的窗口，加载 `assets/scripts/main.lua` 创建实体并挂载变换/精灵/动画组件，随后进入事件循环渲染。按 `ESC` 或关闭窗口即可退出。

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
- `BatchRenderer`：批渲染器，`Begin()` / `AddSprite()` / `End()` / `Render()` 流程，内部按图层（`layer`）排序精灵并合并批次
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
- `AnimationComponent` —— 帧动画：`numFrames`（总帧数）/ `frameRate`（播放速率）/ `frameOffset`（起始帧偏移）/ `currentFrame`（当前帧）/ `bVertical`（图集是否纵向排列）
- `ScriptComponent` —— 持有绑定到该实体的 Lua `update` / `render` 回调引用

系统：
- `RenderSystem` —— 驱动 `BatchRenderer` 绘制带 `SpriteComponent` + `TransformComponent` 的实体
- `ScriptingSystem` —— 加载并执行 Lua 脚本，注册 C++ → Lua 绑定
- `AnimationSystem` —— 推进 `AnimationComponent` 的帧索引

`AssetManager` 统一管理纹理与着色器，按名称索引并供实体引用：

```cpp
auto assetManager = std::make_shared<NEXUS_RESOURCES::AssetManager>();
assetManager->AddTexture("castle", "./assets/textures/castle.png", true);
assetManager->AddShader("basic", "assets/shaders/basicShader.vert",
                                "assets/shaders/basicShader.frag");
```

### Lua 脚本

脚本系统把 ECS 与 glm 类型暴露给 Lua（见 `Core/Scripting/GlmLuaBindings.cpp` 与 `Core/ECS/Entity.inl` 中的元注册），可直接用 Lua 搭建场景：

```lua
-- assets/scripts/main.lua
gEntity = Entity("TestEntity", "Groupy")

local transform = gEntity:add_component(
    Transform(vec2(100, 100), vec2(10, 10), 0)
)

local sprite = gEntity:add_component(
    Sprite("castle", 16.0, 16.0, 0, 1, 0)
)
sprite:generate_uvs()

-- 查询具备 Transform 的实体并遍历
local view = Registry.get_entities(Transform)
view:for_each(function (entity)
    print(entity:name())
end)

main = {
    [1] = { update = function() transform.rotation = transform.rotation + 9 end },
    [2] = { render = function() end },
}
```

脚本需定义全局 `main` 表：`main[1].update` 在每帧更新时调用，`main[2].render` 在每帧渲染时调用，两者缺失均会导致加载失败。

脚本路径为相对当前工作目录的 `./assets/scripts/main.lua`，因此请在 `build/bin/` 下运行程序。

## 使用示例

创建窗口、初始化渲染并驱动一个 ECS 精灵的完整流程见 `NEXUS_EDITOR/src/Application.cpp`，核心步骤：

1. `NEXUS_INIT_LOGS` 初始化日志系统
2. `SDL_Init` 初始化 VIDEO 与 EVENTS 子系统
3. `SDL_GL_SetAttribute` 配置 OpenGL 4.6 Core、缓冲区位深、双缓冲等
4. 构造 `NEXUS_WINDOWING::Window` 创建窗口
5. `SDL_GL_CreateContext` 创建并 `window.SetGLContext()` 绑定 GL 上下文
6. `gladLoadGL()` 加载 GL 函数指针
7. 创建 `AssetManager`，加载纹理与着色器
8. 创建 `NEXUS_CORE::ECS::Registry`，并注册 `RenderSystem` / `ScriptingSystem` / `AnimationSystem`
9. 创建 `lua_State` 与 `Camera2D`，一并存入 `Registry` 上下文供各系统共享
10. `ScriptingSystem::LoadMainScript` 加载 `main.lua`，进入事件循环渲染

> **资源释放顺序**：`Application::CleanUp()` 中必须先销毁 `Registry`（其 `RenderSystem` 持有 `BatchRenderer`，析构时会调用 `glDelete*`），再销毁窗口，最后 `SDL_Quit()`。若把 `Registry` 留到静态析构阶段释放，GL 上下文与 GLAD 函数指针已失效，Linux 下会直接段错误。

## 开发路线

- [x] ECS 实体组件系统（entt）
- [x] 日志系统
- [x] 基础 2D 渲染（Shader / Texture / Camera2D）
- [x] 2D 批渲染（BatchRenderer）
- [x] Lua 脚本系统与 glm 绑定
- [x] 资源管理（纹理、着色器）
- [x] 帧动画系统
- [x] Windows / Linux 双平台构建
- [ ] 输入系统（`NEXUS_WINDOW/Windowing/Iputs/`，目前为预留目录）
- [ ] GPU 缓冲封装（`NEXUS_RENDERING/Rendering/Buffers/`）
- [ ] 场景图与层级变换
- [ ] 音频与字体资源管理（SDL3_mixer / SDL3_ttf 已接入构建，尚未封装）
- [ ] 编辑器 UI（ImGui 集成）

## 许可证

本项目基于 [GNU General Public License v3.0](./LICENSE) 开源。

SDL3 的许可证详见 `Dependencies/SDL/LICENSE.txt`。
