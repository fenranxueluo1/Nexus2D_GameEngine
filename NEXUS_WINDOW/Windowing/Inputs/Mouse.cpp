#include "Mouse.h"
#include <SDL3/SDL.h>
#include <Logger/Logger.h>

namespace NEXUS_WINDOWING::Inputs {

    void Mouse::Update()
    {
        for (auto& [btn, button] : m_mapButtons)
            button.Reset();

        m_WheelX = 0;
        m_WheelY = 0;
        m_bMouseMoving = false;
    }

    void Mouse::OnBtnPressed(int btn)
    {
        if (btn == NEXUS_MOUSE_UNKNOWN)
        {
            NEXUS_ERROR("Mouse Button [{}] is unknown!", btn);
            return;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Mouse Button [{}] does not exist!", btn);
            return;
        }

        btnItr->second.Update(true);
    }

    void Mouse::OnBtnReleased(int btn)
    {
        if (btn == NEXUS_MOUSE_UNKNOWN)
        {
            NEXUS_ERROR("Mouse Button [{}] is unknown!", btn);
            return;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Mouse Button [{}] does not exist!", btn);
            return;
        }

        btnItr->second.Update(false);
    }

    const bool Mouse::IsBtnPressed(int btn) const
    {
        if (btn == NEXUS_MOUSE_UNKNOWN)
        {
            NEXUS_ERROR("Mouse Button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Mouse Button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bIsPressed;
    }

    const bool Mouse::IsBtnJustPressed(int btn) const
    {
        if (btn == NEXUS_MOUSE_UNKNOWN)
        {
            NEXUS_ERROR("Mouse Button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Mouse Button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bJustPressed;
    }

    const bool Mouse::IsBtnJustReleased(int btn) const
    {
        if (btn == NEXUS_MOUSE_UNKNOWN)
        {
            NEXUS_ERROR("Mouse Button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Mouse Button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bJustReleased;
    }

    const std::tuple<int, int> Mouse::GetMouseScreenPosition() 
    {
        // SDL3 中 SDL_GetMouseState 使用 float 输出参数
        float x{ 0.f }, y{ 0.f };
        SDL_GetMouseState(&x, &y);
        m_X = static_cast<int>(x);
        m_Y = static_cast<int>(y);
        return std::make_tuple(m_X, m_Y);
    }
}
