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

namespace NEXUS_EDITOR {

    bool Application::Initialize()
    {
		NEXUS_INIT_LOGS(true, true);

    // SDL3 已移除 SDL_INIT_EVERYTHING，需显式列出要初始化的子系统
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
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

    if (!assetManager->AddTexture("castle", "./assets/textures/tileset.png", true))
    {
    	NEXUS_ERROR("无法创建并添加纹理");
    	return false;
    }


    //添加临时纹理
    auto texture = assetManager->GetTexture("castle");
    
    NEXUS_LOG("加载纹理：[宽度 = {0}, 高度 = {1}]", texture.GetWidth(),  texture.GetHeight());
    NEXUS_WARN("加载纹理：[宽度 = {0}, 高度 = {1}]", texture.GetWidth(), texture.GetHeight());

    m_pRegistry = std::make_unique<NEXUS_CORE::ECS::Registry>();

	NEXUS_CORE::ECS::Entity entity1{*m_pRegistry, "Ent1", "Test"};
	
	auto& transform = entity1.AddComponent<NEXUS_CORE::ECS::TransformComponent>(NEXUS_CORE::ECS::TransformComponent{
				.position = glm::vec2{10.f, 10.f},
				.scale = glm::vec2{3.f, 3.f},
				.rotation = 0.f
		}
	);

	auto& sprite = entity1.AddComponent<NEXUS_CORE::ECS::SpriteComponent>(NEXUS_CORE::ECS::SpriteComponent{
				.width = 16.f,
				.height = 16.f,
				.color = NEXUS_RENDERING::Color{.r = 255, .g = 255, .b = 255, .a = 255},
				.start_x = 0,
				.start_y = 1,
				.layer = 0,
				.texture_name = "castle"
		}
	);
	
	sprite.generate_uvs(texture.GetWidth(), texture.GetHeight());

    auto& id = entity1.GetComponent<NEXUS_CORE::ECS::Identification>();

	NEXUS_LOG("名称: {}, 分类: {}, ID: {}", id.name, id.group, id.entity_id);

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
		
	if (!scriptSystem->LoadMainScript(lua.get()))
	{
		NEXUS_ERROR("无法加载主lua脚本!");
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

		auto view = m_pRegistry->GetRegistry().view<NEXUS_CORE::ECS::TransformComponent, NEXUS_CORE::ECS::SpriteComponent>();

		static float rotation{ 0.f };
		static float x_pos{ 10.f };
		static bool bMoveRight{ true };

		if (rotation >= 360.f)
			rotation = 0.f;

		if (bMoveRight && x_pos < 300.f)
			x_pos += 3;
		else if (bMoveRight && x_pos >= 300.f)
			bMoveRight = false;

		if (!bMoveRight && x_pos > 10.f)
			x_pos -= 3;
		else if (!bMoveRight && x_pos <= 10.f)
			bMoveRight = true;

		for (const auto& entity : view)
		{
			NEXUS_CORE::ECS::Entity ent{*m_pRegistry, entity};
			auto& transform = ent.GetComponent<NEXUS_CORE::ECS::TransformComponent>();

			transform.rotation = rotation;
			transform.position.x = x_pos;
		}
		
		rotation += bMoveRight ? 9 : -9;
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
