#include "Application.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Rendering/Essentials/TextureLoader.h>
#include <Rendering/Essentials/Vertex.h>
#include <Rendering/Core/Camera2D.h>
#include <Logger/Logger.h>
#include <Core/ECS/Entity.h>
#include <Core/ECS/Components/SpriteComponent.h>
#include <Core/ECS/Components/Identification.h>
#include <Core/ECS/Components/TransformComponent.h>
#include <Core/Resources/AssetManager.h>
#include <Core/Systems/ScriptingSystem.h>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/Systems/AnimationSystem.h>
#include <Core/Scripting/InputManager.h>
#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>

namespace NEXUS_EDITOR {

    bool Application::Initialize()
    {
		NEXUS_INIT_LOGS(true, true);

    // SDL3 已移除 SDL_INIT_EVERYTHING，需显式列出要初始化的子系统
    // GAMEPAD / JOYSTICK 子系统必须显式初始化，否则手柄插拔与按键事件不会投递
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK))
    {
        std::string error = SDL_GetError();
        NEXUS_ERROR("无法初始化SDL: {}", error);
        return false;
    }

    //设置opengl属性
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //设置每个通道的位数
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    //创建窗口
    m_pWindow = std::make_unique<NEXUS_WINDOWING::Window>("测试窗口", 640, 480, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, SDL_WINDOW_OPENGL);
    
    if (!m_pWindow->GetWindow())
    {
        NEXUS_ERROR("无法创建窗口！");
        return false;
    }

    //创建opengl上下文（SDL3 会自动加载 GL 库，无需手动 SDL_GL_LoadLibrary）
    SDL_GLContext glContext = SDL_GL_CreateContext(m_pWindow->GetWindow().get());
    if (glContext == nullptr)
    {
        std::string error = SDL_GetError();
        NEXUS_ERROR("无法创建Opengl上下文：{}",error);
        return false;
    }
    m_pWindow->SetGLContext(glContext);

    SDL_GL_MakeCurrent(m_pWindow->GetWindow().get(), m_pWindow->GetGLContext());
    SDL_GL_SetSwapInterval(1);

    //初始glad（GLAD 内置加载器会自动加载 opengl32.dll 并通过 wglGetProcAddress 获取函数指针）
    if (gladLoadGL() == 0)
    {
        NEXUS_ERROR("无法初始化glad!");
        return false;
    }

    //启用Alpha Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto assetManager = std::make_shared<NEXUS_RESOURCES::AssetManager>();
    if (!assetManager)
    {
    	NEXUS_ERROR("无法创建资产管理器!");
    	return false;
    }

	m_pRegistry = std::make_unique<NEXUS_CORE::ECS::Registry>();
	if (!m_pRegistry)
	{
		NEXUS_ERROR("无法创建注册表!");
		return false;
	}

	//创建lua状态（LuaBridge3 基于原生 lua_State）
	auto lua = std::shared_ptr<lua_State>(luaL_newstate(), [](lua_State* L) { if (L) lua_close(L); });

	if (!lua)
	{
		NEXUS_ERROR("无法创建lua状态!");
		return false;
	}

	//打开 Lua 标准库（base、math、os、table、io、string 等）
	luaL_openlibs(lua.get());

	//启用异常，脚本运行出错时抛出 luabridge::LuaException
	luabridge::enableExceptions(lua.get());

	if (!m_pRegistry->AddToContext<std::shared_ptr<lua_State>>(lua))
	{
		NEXUS_ERROR("无法将lua状态添加到注册表上下文中!");
		return false;
	}
		
	auto scriptSystem = std::make_shared<NEXUS_CORE::Systems::ScriptingSystem>(*m_pRegistry);
	if (!scriptSystem)
	{
		NEXUS_ERROR("无法创建脚本系统!");
		return false;
	}
		
	if (!m_pRegistry->AddToContext<std::shared_ptr<NEXUS_CORE::Systems::ScriptingSystem>>(scriptSystem))
	{
		NEXUS_ERROR("无法将脚本系统添加到注册表上下文中!");
		return false;
	}

	auto renderSystem = std::make_shared<NEXUS_CORE::Systems::RenderSystem>(*m_pRegistry);
	if (!renderSystem)
	{
		NEXUS_ERROR("无法创建渲染系统!");
		return false;
	}

	if (!m_pRegistry->AddToContext<std::shared_ptr<NEXUS_CORE::Systems::RenderSystem>>(renderSystem))
	{
		NEXUS_ERROR("无法将渲染系统添加到注册表上下文中!");
		return false;
	}

	auto animationSystem = std::make_shared<NEXUS_CORE::Systems::AnimationSystem>(*m_pRegistry);
		if (!animationSystem)
		{
			NEXUS_ERROR("无法创建动画系统!");
			return false;
		}

		if (!m_pRegistry->AddToContext<std::shared_ptr<NEXUS_CORE::Systems::AnimationSystem>>(animationSystem))
		{
			NEXUS_ERROR("无法将动画系统添加到注册表上下文中!");
			return false;
		}

    //创建临时相机
    auto camera = std::make_shared<NEXUS_RENDERING::Camera2D>();

	if (!m_pRegistry->AddToContext<std::shared_ptr<NEXUS_RESOURCES::AssetManager>>(assetManager))
	{
		NEXUS_ERROR("无法添加资产管理器到注册表上下文中！");
		return false;
	}

	if (!m_pRegistry->AddToContext<std::shared_ptr<NEXUS_RENDERING::Camera2D>>(camera))
	{
		NEXUS_ERROR("无法添加摄像机到注册表上下文中！");
		return false;
	}

	if (!LoadShaders())
	{
		NEXUS_ERROR("加载着色器失败！");
		return false;
	}

	NEXUS_CORE::Systems::ScriptingSystem::RegisterLuaBindings(lua.get(), *m_pRegistry);
	NEXUS_CORE::Systems::ScriptingSystem::RegisterLuaFunctions(lua.get());
		
	if (!scriptSystem->LoadMainScript(lua.get()))
	{
		NEXUS_ERROR("无法加载主lua脚本!");
		return false;
	}
	
    return true;
}

bool Application::LoadShaders()
{
	auto& assetManager = m_pRegistry->GetContext<std::shared_ptr<NEXUS_RESOURCES::AssetManager>>();

	if (!assetManager)
	{
		NEXUS_ERROR("无法从注册表中获取资产管理器！");
		return false;
	}

    if (!assetManager->AddShader("basic", "assets/shaders/basicShader.vert",  "assets/shaders/basicShader.frag"))
	{
		NEXUS_ERROR("无法添加着色器到资产管理器！");
		return false;
	}

	return true;
}

    void Application::ProcessEvents()
    {
		auto& inputManager = NEXUS_CORE::InputManager::GetInstance();
		auto& keyboard = inputManager.GetKeyboard();
		auto& mouse = inputManager.GetMouse();

		// 先清除上一帧遗留的 just_pressed / just_released 状态，再处理本帧事件。
		// 这一步不能放到 Update() 里（事件处理之后）：那样会把刚设置的 just* 标志
		// 立刻清掉，Lua 侧的 just_pressed 将永远读到 false。
		keyboard.Update();

		//处理事件
        while (SDL_PollEvent(&m_Event))
        {
            switch (m_Event.type)
            {
            case SDL_EVENT_QUIT:
                m_bIsRunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (m_Event.key.key == SDLK_ESCAPE)
                {
                    m_bIsRunning = false;
                }
				// SDL3 中按键码字段是 key，SDL2 的 key.keysym.sym 已移除
				keyboard.OnKeyPressed(m_Event.key.key);
				break;

			case SDL_EVENT_KEY_UP:
				// SDL3 已移除 SDL_KEYUP，改用 SDL_EVENT_KEY_UP
				keyboard.OnKeyReleased(m_Event.key.key);
                break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				mouse.OnBtnPressed(m_Event.button.button);
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				mouse.OnBtnReleased(m_Event.button.button);
				break;
			case SDL_EVENT_MOUSE_WHEEL:
				mouse.SetMouseWheelX(static_cast<int>(m_Event.wheel.x));
				mouse.SetMouseWheelY(static_cast<int>(m_Event.wheel.y));
				break;
			case SDL_EVENT_MOUSE_MOTION:
				mouse.SetMouseMoving(true);
				break;
			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
				inputManager.GamepadBtnPressed(m_Event);
				break;
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
				inputManager.GamepadBtnReleased(m_Event);
				break;
			case SDL_EVENT_GAMEPAD_ADDED:
				inputManager.AddGamepad(m_Event.gdevice.which);
				break;
			case SDL_EVENT_GAMEPAD_REMOVED:
				inputManager.RemoveGamepad(m_Event.gdevice.which);
				break;
			case SDL_EVENT_JOYSTICK_AXIS_MOTION:
				inputManager.GamepadAxisValues(m_Event);
				break;
			case SDL_EVENT_JOYSTICK_HAT_MOTION:
				inputManager.GamepadHatValues(m_Event);
				break;
            default:
                break;
            }
        }
    }

    void Application::Update()
    {
		auto& camera = m_pRegistry->GetContext<std::shared_ptr<NEXUS_RENDERING::Camera2D>>();
		if (!camera)
		{
			NEXUS_ERROR("无法从注册表上下文中获取摄像机!");
			return;
		}
		
		camera->Update();

		auto& scriptSystem = m_pRegistry->GetContext<std::shared_ptr<NEXUS_CORE::Systems::ScriptingSystem>>();
		scriptSystem->Update();

		auto& animationSystem = m_pRegistry->GetContext<std::shared_ptr<NEXUS_CORE::Systems::AnimationSystem>>();
		animationSystem->Update();

		auto& inputManager = NEXUS_CORE::InputManager::GetInstance();
		inputManager.GetMouse().Update();
		inputManager.UpdateGamepads();
    }

    void Application::Render()
    {
		auto& renderSystem = m_pRegistry->GetContext<std::shared_ptr<NEXUS_CORE::Systems::RenderSystem>>();
		
		glViewport(
			0, 0,
			m_pWindow->GetWidth(),
			m_pWindow->GetHeight()
		);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		auto& scriptSystem = m_pRegistry->GetContext<std::shared_ptr<NEXUS_CORE::Systems::ScriptingSystem>>();
		scriptSystem->Render();
		renderSystem->Update();

		SDL_GL_SwapWindow(m_pWindow->GetWindow().get());

    }

    void Application::CleanUp()
    {
		// 必须按此顺序释放资源：
		// 1) 先销毁注册表。它上下文中的 RenderSystem 持有 BatchRenderer，
		//    其析构函数会调用 glDeleteVertexArrays / glDeleteBuffers，
		//    这些 GL 调用要求上下文仍然有效。
		// 2) 再销毁窗口，让 SDL 一并清理其 GL 上下文。
		// 3) 最后才 SDL_Quit()。
		//
		// 若把注册表留到 Application 单例静态析构时才释放，则 SDL_Quit() 早已执行，
		// GL 上下文与 GLAD 获取到的函数指针均已失效。
		// Windows 下这通常是无害的空操作（opengl32.dll 仍在加载），
		// 但 Linux 下会直接段错误。
		m_pRegistry.reset();
		m_pWindow.reset();

		SDL_Quit();
    }

    Application::Application()
        : m_pWindow{nullptr}, m_pRegistry{nullptr}, m_Event{}, m_bIsRunning{true}
    {

    }

    Application& Application::GetInstance()
    {
		static Application app{};
		return app;
    }

    Application::~Application()
	{
    }

    void Application::Run()
    {
		if (!Initialize())
		{
			NEXUS_ERROR("初始化失败!");
			return;
		}

		while (m_bIsRunning)
		{
			ProcessEvents();
			Update();
			Render();
		}

		CleanUp();
    }
}
