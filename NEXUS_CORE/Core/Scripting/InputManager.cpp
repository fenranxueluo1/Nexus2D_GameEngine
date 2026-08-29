#include "InputManager.h"
#include <Logger/Logger.h>
#include <Nexus_Utilities/SDL_Wrappers.h>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

namespace NEXUS_CORE {

    InputManager::InputManager()
        : m_pKeyboard{ std::make_unique<Keyboard>() }, m_pMouse{ std::make_unique<Mouse>() }
    {
        
    }

	void InputManager::RegisterLuaKeyNames(lua_State* lua)
	{
        // ==================================================================
      // Register Typewriter Keys
      // ==================================================================
        luabridge::setGlobal(lua, NEXUS_KEY_A, "KEY_A");
        luabridge::setGlobal(lua, NEXUS_KEY_B, "KEY_B");
        luabridge::setGlobal(lua, NEXUS_KEY_C, "KEY_C");
        luabridge::setGlobal(lua, NEXUS_KEY_D, "KEY_D");
        luabridge::setGlobal(lua, NEXUS_KEY_E, "KEY_E");
        luabridge::setGlobal(lua, NEXUS_KEY_F, "KEY_F");
        luabridge::setGlobal(lua, NEXUS_KEY_G, "KEY_G");
        luabridge::setGlobal(lua, NEXUS_KEY_H, "KEY_H");
        luabridge::setGlobal(lua, NEXUS_KEY_I, "KEY_I");
        luabridge::setGlobal(lua, NEXUS_KEY_J, "KEY_J");
        luabridge::setGlobal(lua, NEXUS_KEY_K, "KEY_K");
        luabridge::setGlobal(lua, NEXUS_KEY_L, "KEY_L");
        luabridge::setGlobal(lua, NEXUS_KEY_M, "KEY_M");
        luabridge::setGlobal(lua, NEXUS_KEY_N, "KEY_N");
        luabridge::setGlobal(lua, NEXUS_KEY_O, "KEY_O");
        luabridge::setGlobal(lua, NEXUS_KEY_P, "KEY_P");
        luabridge::setGlobal(lua, NEXUS_KEY_Q, "KEY_Q");
        luabridge::setGlobal(lua, NEXUS_KEY_R, "KEY_R");
        luabridge::setGlobal(lua, NEXUS_KEY_S, "KEY_S");
        luabridge::setGlobal(lua, NEXUS_KEY_T, "KEY_T");
        luabridge::setGlobal(lua, NEXUS_KEY_U, "KEY_U");
        luabridge::setGlobal(lua, NEXUS_KEY_V, "KEY_V");
        luabridge::setGlobal(lua, NEXUS_KEY_W, "KEY_W");
        luabridge::setGlobal(lua, NEXUS_KEY_X, "KEY_X");
        luabridge::setGlobal(lua, NEXUS_KEY_Y, "KEY_Y");
        luabridge::setGlobal(lua, NEXUS_KEY_Z, "KEY_Z");

        luabridge::setGlobal(lua, NEXUS_KEY_0, "KEY_0");
        luabridge::setGlobal(lua, NEXUS_KEY_1, "KEY_1");
        luabridge::setGlobal(lua, NEXUS_KEY_2, "KEY_2");
        luabridge::setGlobal(lua, NEXUS_KEY_3, "KEY_3");
        luabridge::setGlobal(lua, NEXUS_KEY_4, "KEY_4");
        luabridge::setGlobal(lua, NEXUS_KEY_5, "KEY_5");
        luabridge::setGlobal(lua, NEXUS_KEY_6, "KEY_6");
        luabridge::setGlobal(lua, NEXUS_KEY_7, "KEY_7");
        luabridge::setGlobal(lua, NEXUS_KEY_8, "KEY_8");
        luabridge::setGlobal(lua, NEXUS_KEY_9, "KEY_9");

        luabridge::setGlobal(lua, NEXUS_KEY_RETURN, "KEY_ENTER");
        luabridge::setGlobal(lua, NEXUS_KEY_BACKSPACE, "KEY_BACKSPACE");
        luabridge::setGlobal(lua, NEXUS_KEY_ESCAPE, "KEY_ESC");
        luabridge::setGlobal(lua, NEXUS_KEY_SPACE, "KEY_SPACE");
        luabridge::setGlobal(lua, NEXUS_KEY_LCTRL, "KEY_LCTRL");
        luabridge::setGlobal(lua, NEXUS_KEY_RCTRL, "KEY_RCTRL");
        luabridge::setGlobal(lua, NEXUS_KEY_LALT, "KEY_LALT");
        luabridge::setGlobal(lua, NEXUS_KEY_RALT, "KEY_RALT");
        luabridge::setGlobal(lua, NEXUS_KEY_LSHIFT, "KEY_LSHIFT");
        luabridge::setGlobal(lua, NEXUS_KEY_RSHIFT, "KEY_RSHIFT");

        // ==================================================================
        //  Register Function Keys
        // ==================================================================
        luabridge::setGlobal(lua, NEXUS_KEY_F1, "KEY_F1");
        luabridge::setGlobal(lua, NEXUS_KEY_F2, "KEY_F2");
        luabridge::setGlobal(lua, NEXUS_KEY_F3, "KEY_F3");
        luabridge::setGlobal(lua, NEXUS_KEY_F4, "KEY_F4");
        luabridge::setGlobal(lua, NEXUS_KEY_F5, "KEY_F5");
        luabridge::setGlobal(lua, NEXUS_KEY_F6, "KEY_F6");
        luabridge::setGlobal(lua, NEXUS_KEY_F7, "KEY_F7");
        luabridge::setGlobal(lua, NEXUS_KEY_F8, "KEY_F8");
        luabridge::setGlobal(lua, NEXUS_KEY_F9, "KEY_F9");
        luabridge::setGlobal(lua, NEXUS_KEY_F10, "KEY_F10");
        luabridge::setGlobal(lua, NEXUS_KEY_F11, "KEY_F11");
        luabridge::setGlobal(lua, NEXUS_KEY_F12, "KEY_F12");

        // ==================================================================
        // Register Cursor Control Keys
        // ==================================================================
        luabridge::setGlobal(lua, NEXUS_KEY_UP, "KEY_UP");
        luabridge::setGlobal(lua, NEXUS_KEY_RIGHT, "KEY_RIGHT");
        luabridge::setGlobal(lua, NEXUS_KEY_DOWN, "KEY_DOWN");
        luabridge::setGlobal(lua, NEXUS_KEY_LEFT, "KEY_LEFT");

        // ==================================================================
        // Register Numeric Keypad Keys
        // ==================================================================
        luabridge::setGlobal(lua, NEXUS_KEY_KP0, "KP_KEY_0");
        luabridge::setGlobal(lua, NEXUS_KEY_KP1, "KP_KEY_1");
        luabridge::setGlobal(lua, NEXUS_KEY_KP2, "KP_KEY_2");
        luabridge::setGlobal(lua, NEXUS_KEY_KP3, "KP_KEY_3");
        luabridge::setGlobal(lua, NEXUS_KEY_KP4, "KP_KEY_4");
        luabridge::setGlobal(lua, NEXUS_KEY_KP5, "KP_KEY_5");
        luabridge::setGlobal(lua, NEXUS_KEY_KP6, "KP_KEY_6");
        luabridge::setGlobal(lua, NEXUS_KEY_KP7, "KP_KEY_7");
        luabridge::setGlobal(lua, NEXUS_KEY_KP8, "KP_KEY_8");
        luabridge::setGlobal(lua, NEXUS_KEY_KP9, "KP_KEY_9");
        luabridge::setGlobal(lua, NEXUS_KEY_KP_ENTER, "KP_KEY_ENTER");
	}

    void InputManager::RegisterMouseBtnNames(lua_State* lua)
    {
        luabridge::setGlobal(lua, NEXUS_MOUSE_LEFT, "LEFT_BTN");
        luabridge::setGlobal(lua, NEXUS_MOUSE_MIDDLE, "MIDDLE_BTN");
        luabridge::setGlobal(lua, NEXUS_MOUSE_RIGHT, "RIGHT_BTN");
    }

    void InputManager::RegisterGamepadBtnNames(lua_State* lua)
    {
        luabridge::setGlobal(lua, NEXUS_GP_BTN_A, "GP_BTN_A");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_B, "GP_BTN_B");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_X, "GP_BTN_X");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_Y, "GP_BTN_Y");

        luabridge::setGlobal(lua, NEXUS_GP_BTN_BACK, "GP_BTN_BACK");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_GUIDE, "GP_BTN_GUIDE");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_START, "GP_BTN_START");

        luabridge::setGlobal(lua, NEXUS_GP_BTN_LSTICK, "GP_LSTICK");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_RSTICK, "GP_RSTICK");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_LSHOULDER, "GP_LSHOULDER");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_RSHOULDER, "GP_RSHOULDER");

        luabridge::setGlobal(lua, NEXUS_GP_BTN_DPAD_UP, "DPAD_UP");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_DPAD_DOWN, "DPAD_DOWN");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_DPAD_LEFT, "DPAD_LEFT");
        luabridge::setGlobal(lua, NEXUS_GP_BTN_DPAD_RIGHT, "DPAD_RIGHT");

        luabridge::setGlobal(lua, 0, "AXIS_X1");
        luabridge::setGlobal(lua, 1, "AXIS_Y1");
        luabridge::setGlobal(lua, 2, "AXIS_X2");
        luabridge::setGlobal(lua, 3, "AXIS_Y2");

        // Bottom triggers
        luabridge::setGlobal(lua, 4, "AXIS_Z1");
        luabridge::setGlobal(lua, 5, "AXIS_Z2");
    }

	InputManager& InputManager::GetInstance()
	{
		static InputManager instance{};
		return instance;
	}

	void InputManager::CreateLuaInputBindings(lua_State* lua)
	{
        RegisterLuaKeyNames(lua);
        RegisterMouseBtnNames(lua);
        RegisterGamepadBtnNames(lua);

        auto& inputManager = GetInstance();
        auto& keyboard = inputManager.GetKeyboard();

        luabridge::getGlobalNamespace(lua)
            .beginClass<Keyboard>("Keyboard")
            .addStaticFunction("just_pressed", [&keyboard](int key) { return keyboard.IsKeyJustPressed(key); })
            .addStaticFunction("just_released", [&keyboard](int key) { return keyboard.IsKeyJustReleased(key); })
            .addStaticFunction("pressed", [&keyboard](int key) { return keyboard.IsKeyPressed(key); })
            .endClass();

        auto& mouse = inputManager.GetMouse();

        luabridge::getGlobalNamespace(lua)
            .beginClass<Mouse>("Mouse")
            .addStaticFunction("just_pressed", [&mouse](int btn) { return mouse.IsBtnJustPressed(btn); })
            .addStaticFunction("just_released", [&mouse](int btn) { return mouse.IsBtnJustReleased(btn); })
            .addStaticFunction("pressed", [&mouse](int btn) { return mouse.IsBtnPressed(btn); })
            .addStaticFunction("screen_position", [&mouse]() { return mouse.GetMouseScreenPosition(); })
            .addStaticFunction("wheel_x", [&mouse]() { return mouse.GetMouseWheelX(); })
            .addStaticFunction("wheel_y", [&mouse]() { return mouse.GetMouseWheelY(); })
            .endClass();

        luabridge::getGlobalNamespace(lua)
            .beginClass<Gamepad>("Gamepad")
            .addStaticFunction("just_pressed", [&inputManager](int index, int btn) {
                auto gamepad = inputManager.GetController(index);
                if (!gamepad)
                    return false;
                return gamepad->IsBtnJustPressed(btn);
            })
            .addStaticFunction("just_released", [&inputManager](int index, int btn) {
                auto gamepad = inputManager.GetController(index);
                if (!gamepad)
                    return false;
                return gamepad->IsBtnJustReleased(btn);
            })
            .addStaticFunction("pressed", [&inputManager](int index, int btn) {
                auto gamepad = inputManager.GetController(index);
                if (!gamepad)
                    return false;
                return gamepad->IsBtnPressed(btn);
            })
            .addStaticFunction("get_axis_position", [&inputManager](int index, int axis) {
                auto gamepad = inputManager.GetController(index);
                if (!gamepad)
                    return Sint16{ 0 };
                return gamepad->GetAxisPosition(static_cast<Uint8>(axis));
            })
            .addStaticFunction("get_hat_value", [&inputManager](int index) {
                auto gamepad = inputManager.GetController(index);
                if (!gamepad)
                    return Uint8{ 0 };
                return gamepad->GetJoystickHatValue();
            })
            .endClass();
	}

    std::shared_ptr<Gamepad> InputManager::GetController(int index)
    {
        auto gamepadItr = m_mapGameControllers.find(index);
        if (gamepadItr == m_mapGameControllers.end())
        {
            NEXUS_ERROR("Failed to get gamepad at index [{}] -- Does not exist!", index);
            return nullptr;
        }

        return gamepadItr->second;
    }

    bool InputManager::AddGamepad(Sint32 gamepadIndex)
    {
        if (m_mapGameControllers.size() >= MAX_CONTROLLERS)
        {
            NEXUS_ERROR("Trying to add too many controllers! Max Controllers allowed = {}", MAX_CONTROLLERS);
            return false;
        }

        std::shared_ptr<Gamepad> gamepad{nullptr};
        try
        {
            // SDL3 中 SDL_GameControllerOpen 已更名为 SDL_OpenGamepad
            gamepad = std::make_shared<Gamepad>(
                make_shared_controller(SDL_OpenGamepad(gamepadIndex)));
        }
        catch (...)
        {
            std::string error{SDL_GetError()};
            NEXUS_ERROR("Failed to Open gamepad device -- {}", error);
            return false;
        }

        for (int i = 1; i <= MAX_CONTROLLERS; i++)
        {
            if (m_mapGameControllers.contains(i))
                continue;

            m_mapGameControllers.emplace(i, std::move(gamepad));
            NEXUS_LOG("Gamepad [{}] was added at index [{}]", gamepadIndex, i);
            return true;
        }

        NEXUS_ASSERT(false && "Failed to add the new controller!");
        NEXUS_ERROR("Failed to add the new controller!");
        return false;
    }

    bool InputManager::RemoveGamepad(Sint32 gamepadID)
    {
        auto gamepadRemoved = std::erase_if(m_mapGameControllers,
            [&](auto& gamepad) {
                return gamepad.second->CheckJoystickID(static_cast<SDL_JoystickID>(gamepadID));
            }
        );

        if (gamepadRemoved > 0)
        {
            NEXUS_LOG("Gamepad Removed -- [{}]", gamepadID);
            return true;
        }

        NEXUS_ASSERT(false && "Failed to remove Gamepad must not have been mapped!");
        NEXUS_ERROR("Failed to remove Gamepad [{}] must not have been mapped!", gamepadID);
        return false;
    }

    void InputManager::GamepadBtnPressed(const SDL_Event& event)
    {
        for (const auto& [index, gamepad] : m_mapGameControllers)
        {
            if (gamepad && gamepad->CheckJoystickID(event.gbutton.which))
            {
                gamepad->OnBtnPressed(event.gbutton.button);
                break;
            }
        }
    }

    void InputManager::GamepadBtnReleased(const SDL_Event& event)
    {
        for (const auto& [index, gamepad] : m_mapGameControllers)
        {
            if (gamepad && gamepad->CheckJoystickID(event.gbutton.which))
            {
                gamepad->OnBtnReleased(event.gbutton.button);
                break;
            }
        }
    }

    void InputManager::GamepadAxisValues(const SDL_Event& event)
    {
        for (const auto& [index, gamepad] : m_mapGameControllers)
        {
            if (gamepad && gamepad->CheckJoystickID(event.jaxis.which))
            {
                gamepad->SetAxisPositionValue(event.jaxis.axis, event.jaxis.value);
                break;
            }
        }
    }

    void InputManager::GamepadHatValues(const SDL_Event& event)
    {
        for (const auto& [index, gamepad] : m_mapGameControllers)
        {
            if (gamepad && gamepad->CheckJoystickID(event.jhat.which))
            {
                gamepad->SetJoystickHatValue(event.jhat.value);
                break;
            }
        }
    }

    void InputManager::UpdateGamepads()
    {
        for (const auto& [index, gamepad] : m_mapGameControllers)
        {
            if (gamepad)
                gamepad->Update();
        }
    }
}
