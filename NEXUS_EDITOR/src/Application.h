#pragma once
#include <glad/glad.h>
#include <Windowing/Window/Window.h>
#include <Core/ECS/Registry.h>

namespace NEXUS_EDITOR {
	class Application
	{
	private:
		std::unique_ptr<NEXUS_WINDOWING::Window> m_pWindow;
		std::unique_ptr<NEXUS_CORE::ECS::Registry> m_pRegistry;

		SDL_Event m_Event;
		bool m_bIsRunning;
		
	private:
		bool Initialize();
		bool LoadShaders();

		void ProcessEvents();
		void Update();
		void Render();

		void CleanUp();

		Application();
	public:
		static Application& GetInstance();
		~Application();

		void Run();
	};
}
