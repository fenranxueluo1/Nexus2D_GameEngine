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
				.scale = glm::vec2{1.f, 1.f},
				.rotation = 0.f
		}
	);

	auto& sprite = entity1.AddComponent<NEXUS_CORE::ECS::SpriteComponent>(NEXUS_CORE::ECS::SpriteComponent{
				.width = 16.f,
				.height = 16.f,
				.color = NEXUS_RENDERING::Color{.r = 255, .g = 0, .b = 255, .a = 255},
				.start_x = 0,
				.start_y = 1
		}
	);
	
	sprite.generate_uvs(texture.GetWidth(), texture.GetHeight());

    //创建顶点数据
    std::vector<NEXUS_RENDERING::Vertex> vertices{};
    NEXUS_RENDERING::Vertex vTL{}, vTR{}, vBL{}, vBR{};

    //左上 TL
    vTL.position = glm::vec2{ transform.position.x, transform.position.y + sprite.height};
	vTL.uvs = glm::vec2{ sprite.uvs.u, sprite.uvs.v + sprite.uvs.uv_height};

    //右上 TR
	vTR.position = glm::vec2{ transform.position.x + sprite.width, transform.position.y + sprite.height};
	vTR.uvs = glm::vec2{ sprite.uvs.u + sprite.uvs.uv_width, sprite.uvs.v + sprite.uvs.uv_height };

    //左下 BL
	vBL.position = glm::vec2{ transform.position.x, transform.position.y };
	vBL.uvs = glm::vec2{ sprite.uvs.u, sprite.uvs.v};

    //右下 BR
	vBR.position = glm::vec2{ transform.position.x + sprite.width, transform.position.y};
	vBR.uvs = glm::vec2{ sprite.uvs.u + sprite.uvs.uv_width, sprite.uvs.v};

	vertices.push_back(vTL);
	vertices.push_back(vBL);
	vertices.push_back(vBR);
	vertices.push_back(vTR);

    auto& id = entity1.GetComponent<NEXUS_CORE::ECS::Identification>();

	NEXUS_LOG("名称: {}, 分类: {}, ID: {}", id.name, id.group, id.entity_id);

    GLuint indices[] = 
    {
        0, 1, 2,
        2, 3, 0
    };

    //创建临时相机
    auto camera = std::make_shared<NEXUS_RENDERING::Camera2D>();
    camera->SetScale(5.f);

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
	
    
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    //绑定顶点数组对象
    glBindVertexArray(VAO);
    //绑定顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点数据复制到缓冲对象（sizeof(vertices) 已是总字节数，不要再乘 stride）
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(NEXUS_RENDERING::Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint), indices, GL_STATIC_DRAW);

    //设置顶点属性指针
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(NEXUS_RENDERING::Vertex), (void*)offsetof(NEXUS_RENDERING::Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(NEXUS_RENDERING::Vertex), (void*)offsetof(NEXUS_RENDERING::Vertex, uvs));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(NEXUS_RENDERING::Vertex), (void*)offsetof(NEXUS_RENDERING::Vertex, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

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
    }

    void Application::Render()
    {
		auto& assetManager = m_pRegistry->GetContext<std::shared_ptr<NEXUS_RESOURCES::AssetManager>>();
		auto& camera = m_pRegistry->GetContext<std::shared_ptr<NEXUS_RENDERING::Camera2D>>();

		auto& shader = assetManager->GetShader("basic");
		auto projection = camera->GetCameraMatrix();

		if (shader.ShaderProgramID() == 0)
		{
			NEXUS_ERROR("着色器程序没有正确创建!");
			return;
		}

		glViewport(
			0, 0,
			m_pWindow->GetWidth(),
			m_pWindow->GetHeight()
		);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		shader.Enable();
		glBindVertexArray(VAO);

		shader.SetUniformMat4("uProjection", projection);

		glActiveTexture(GL_TEXTURE0);
		const auto& texture = assetManager->GetTexture("castle");
		glBindTexture(GL_TEXTURE_2D, texture.GetID());


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		SDL_GL_SwapWindow(m_pWindow->GetWindow().get());

		shader.Disable();
    }

    void Application::CleanUp()
    {
		SDL_Quit();
    }

    Application::Application()
        : m_pWindow{nullptr}, m_pRegistry{nullptr}, m_Event{}, m_bIsRunning{true}
        , VAO{ 0 }, VBO{ 0 }, IBO{ 0 }
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
