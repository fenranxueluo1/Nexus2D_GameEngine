#include "Window.h"


namespace NEXUS_WINDOWING {

    void Window::CreateNewWindow(Uint32 flags)
    {
        m_pWindow = WindowPtr(SDL_CreateWindow(
            m_sTitle.c_str(),
            m_Width, m_Height, flags
        ));

        if (m_pWindow)
        {
            SDL_SetWindowPosition(m_pWindow.get(), m_XPos, m_YPos);
        }

        if (!m_pWindow)
        {
            std::string error = SDL_GetError();
            std::cout << "窗口创建失败: " << error << std::endl;
        }
    }

    Window::Window(const std::string title, int width, int height, int x_pos, int y_pos, bool v_sync, Uint32 flags)
        : m_pWindow(nullptr), m_GLContext{}, m_sTitle(title)
        , m_Width(width), m_Height(height), m_XPos(x_pos), m_YPos(y_pos), m_WindowFlags(flags)
    {
        CreateNewWindow(flags);

        //启用垂直同步v_sync
		if (v_sync)
		{
			if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"))
			{
				std::cout << "启用垂直同步失败" << std::endl;
			}
		}
        std::cout << "窗口创建成功" << std::endl;
    }

    Window::~Window()
    {

    }

    void Window::SetWindowName(const std::string &name)
    {
        m_sTitle = name;
        SDL_SetWindowTitle(m_pWindow.get(), name.c_str());
    }
}


