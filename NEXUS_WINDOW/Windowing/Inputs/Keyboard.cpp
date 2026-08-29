#include "Keyboard.h"
#include <Logger/Logger.h>

namespace NEXUS_WINDOWING::Inputs {

    Keyboard::Keyboard()
        : m_mapButtons{
        {NEXUS_KEY_BACKSPACE, Button{}}, {NEXUS_KEY_TAB, Button{}}, {NEXUS_KEY_CLEAR, Button{}},
        {NEXUS_KEY_RETURN, Button{}}, {NEXUS_KEY_PAUSE, Button{}}, {NEXUS_KEY_ESCAPE, Button{}}, {NEXUS_KEY_SPACE, Button{}},
        {NEXUS_KEY_EXCLAIM, Button{}}, {NEXUS_KEY_QUOTEDBL, Button{}}, {NEXUS_KEY_HASH, Button{}}, {NEXUS_KEY_DOLLAR, Button{}},
        {NEXUS_KEY_AMPERSAND, Button{}}, {NEXUS_KEY_QUOTE, Button{}}, {NEXUS_KEY_LEFTPAREN, Button{}}, {NEXUS_KEY_RIGHTPAREN, Button{}},
        {NEXUS_KEY_ASTERISK, Button{}}, {NEXUS_KEY_PLUS, Button{}}, {NEXUS_KEY_COMMA, Button{}},
        {NEXUS_KEY_MINUS, Button{}}, {NEXUS_KEY_PERIOD, Button{}},
        {NEXUS_KEY_SLASH, Button{}}, {NEXUS_KEY_0, Button{}}, {NEXUS_KEY_1, Button{}},
        {NEXUS_KEY_2, Button{}}, {NEXUS_KEY_3, Button{}},
        {NEXUS_KEY_4, Button{}}, {NEXUS_KEY_5, Button{}}, {NEXUS_KEY_6, Button{}}, {NEXUS_KEY_7, Button{}},
        {NEXUS_KEY_8, Button{}}, {NEXUS_KEY_9, Button{}}, {NEXUS_KEY_COLON, Button{}}, {NEXUS_KEY_SEMICOLON, Button{}},
        {NEXUS_KEY_LESS, Button{}}, {NEXUS_KEY_EQUALS, Button{}}, {NEXUS_KEY_GREATER, Button{}}, {NEXUS_KEY_QUESTION, Button{}},
        {NEXUS_KEY_AT, Button{}}, {NEXUS_KEY_LEFTBRACKET, Button{}}, {NEXUS_KEY_BACKSLASH, Button{}}, {NEXUS_KEY_RIGHTBRACKET, Button{}},
        {NEXUS_KEY_CARET, Button{}}, {NEXUS_KEY_UNDERSCORE, Button{}}, {NEXUS_KEY_BACKQUOTE, Button{}},
        {NEXUS_KEY_A, Button{}}, {NEXUS_KEY_B, Button{}},
        {NEXUS_KEY_C, Button{}}, {NEXUS_KEY_D, Button{}}, {NEXUS_KEY_E, Button{}}, {NEXUS_KEY_F, Button{}},
        {NEXUS_KEY_G, Button{}}, {NEXUS_KEY_H, Button{}}, {NEXUS_KEY_I, Button{}}, {NEXUS_KEY_J, Button{}},
        {NEXUS_KEY_K, Button{}}, {NEXUS_KEY_L, Button{}}, {NEXUS_KEY_M, Button{}}, {NEXUS_KEY_N, Button{}},
        {NEXUS_KEY_O, Button{}}, {NEXUS_KEY_P, Button{}}, {NEXUS_KEY_Q, Button{}}, {NEXUS_KEY_R, Button{}},
        {NEXUS_KEY_S, Button{}}, {NEXUS_KEY_T, Button{}}, {NEXUS_KEY_U, Button{}}, {NEXUS_KEY_V, Button{}},
        {NEXUS_KEY_W, Button{}}, {NEXUS_KEY_X, Button{}}, {NEXUS_KEY_Y, Button{}}, {NEXUS_KEY_Z, Button{}},
        {NEXUS_KEY_DELETE, Button{}}, {NEXUS_KEY_CAPSLOCK, Button{}}, {NEXUS_KEY_F1, Button{}}, {NEXUS_KEY_F2, Button{}},
        {NEXUS_KEY_F3, Button{}}, {NEXUS_KEY_F4, Button{}}, {NEXUS_KEY_F5, Button{}}, {NEXUS_KEY_F6, Button{}},
        {NEXUS_KEY_F7, Button{}}, {NEXUS_KEY_F8, Button{}}, {NEXUS_KEY_F9, Button{}}, {NEXUS_KEY_F10, Button{}},
        {NEXUS_KEY_F11, Button{}}, {NEXUS_KEY_F12, Button{}}, {NEXUS_KEY_SCROLLOCK, Button{}}, {NEXUS_KEY_INSERT, Button{}},
        {NEXUS_KEY_HOME, Button{}}, {NEXUS_KEY_PAGEUP, Button{}}, {NEXUS_KEY_PAGEDOWN, Button{}}, {NEXUS_KEY_END, Button{}},
        {NEXUS_KEY_RIGHT, Button{}}, {NEXUS_KEY_LEFT, Button{}}, {NEXUS_KEY_DOWN, Button{}}, {NEXUS_KEY_UP, Button{}},
        {NEXUS_KEY_NUMLOCK, Button{}}, {NEXUS_KEY_KP_DIVIDE, Button{}}, {NEXUS_KEY_KP_MULTIPLY, Button{}}, {NEXUS_KEY_KP_MINUS, Button{}},
        {NEXUS_KEY_KP_PLUS, Button{}}, {NEXUS_KEY_KP_ENTER, Button{}}, {NEXUS_KEY_KP1, Button{}}, {NEXUS_KEY_KP2, Button{}},
        {NEXUS_KEY_KP3, Button{}}, {NEXUS_KEY_KP4, Button{}}, {NEXUS_KEY_KP5, Button{}}, {NEXUS_KEY_KP6, Button{}},
        {NEXUS_KEY_KP7, Button{}}, {NEXUS_KEY_KP8, Button{}}, {NEXUS_KEY_KP9, Button{}}, {NEXUS_KEY_KP0, Button{}},
        {NEXUS_KEY_KP_PERIOD, Button{}}, {NEXUS_KEY_LCTRL, Button{}}, {NEXUS_KEY_LSHIFT, Button{}},
        {NEXUS_KEY_LALT, Button{}}, {NEXUS_KEY_RCTRL, Button{}}, {NEXUS_KEY_RSHIFT, Button{}}, {NEXUS_KEY_RALT, Button{}}
        }
    {
    }

    void Keyboard::Update()
    {
        for (auto& [key, button] : m_mapButtons)
            button.Reset();
    }

    void Keyboard::OnKeyPressed(int key)
    {
        if (key == NEXUS_KEY_UNKNOWN)
        {
            NEXUS_ERROR("Key [{}] is unknown!", key);
            return;
        }

        auto keyItr = m_mapButtons.find(key);
        if (keyItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Key [{}] does not exist!", key);
            return;
        }
        keyItr->second.Update(true);
    }

    void Keyboard::OnKeyReleased(int key)
    {
        if (key == NEXUS_KEY_UNKNOWN)
        {
            NEXUS_ERROR("Key [{}] is unknown!", key);
            return;
        }

        auto keyItr = m_mapButtons.find(key);
        if (keyItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Key [{}] does not exist!", key);
            return;
        }
        keyItr->second.Update(false);
    }

    bool Keyboard::IsKeyPressed(int key) const
    {
        if (key == NEXUS_KEY_UNKNOWN)
        {
            NEXUS_ERROR("Key [{}] is unknown!", key);
            return false;
        }

        auto keyItr = m_mapButtons.find(key);
        if (keyItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Key [{}] does not exist!", key);
            return false;
        }
        return keyItr->second.bIsPressed;
    }

    bool Keyboard::IsKeyJustPressed(int key) const
    {
        if (key == NEXUS_KEY_UNKNOWN)
        {
            NEXUS_ERROR("Key [{}] is unknown!", key);
            return false;
        }

        auto keyItr = m_mapButtons.find(key);
        if (keyItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Key [{}] does not exist!", key);
            return false;
        }

        return keyItr->second.bJustPressed;
    }

    bool Keyboard::IsKeyJustReleased(int key) const
    {

        if (key == NEXUS_KEY_UNKNOWN)
        {
            NEXUS_ERROR("Key [{}] is unknown!", key);
            return false;
        }

        auto keyItr = m_mapButtons.find(key);
        if (keyItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Key [{}] does not exist!", key);
            return false;
        }

        return keyItr->second.bJustReleased;
    }
}
