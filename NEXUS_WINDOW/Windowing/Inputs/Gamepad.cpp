#include "Gamepad.h"
#include <Logger/Logger.h>

namespace NEXUS_WINDOWING::Inputs {

    Gamepad::Gamepad(Controller controller)
        : m_pController{std::move(controller)}
        , m_mapButtons{
            {NEXUS_GP_BTN_A, Button{}},
            {NEXUS_GP_BTN_B, Button{}},
            {NEXUS_GP_BTN_X, Button{}},
            {NEXUS_GP_BTN_Y, Button{}},
            {NEXUS_GP_BTN_BACK, Button{}},
            {NEXUS_GP_BTN_GUIDE, Button{}},
            {NEXUS_GP_BTN_START, Button{}},
            {NEXUS_GP_BTN_LSTICK, Button{}},
            {NEXUS_GP_BTN_RSTICK, Button{}},
            {NEXUS_GP_BTN_LSHOULDER, Button{}},
            {NEXUS_GP_BTN_RSHOULDER, Button{}},
            {NEXUS_GP_BTN_DPAD_UP, Button{}},
            {NEXUS_GP_BTN_DPAD_DOWN, Button{}},
            {NEXUS_GP_BTN_DPAD_LEFT, Button{}},
            {NEXUS_GP_BTN_DPAD_RIGHT, Button{}},
        }
        , m_InstanceID{0}
        , m_mapAxisValues{
            {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}
        }
        , m_JoystickHatValue{NEXUS_HAT_CENTERED}
    {
        // SDL3：SDL_GameControllerGetJoystick -> SDL_GetGamepadJoystick
        SDL_Joystick* joystick = SDL_GetGamepadJoystick(m_pController.get());
        if (!m_pController || !joystick)
            throw("Controller or Joystick were nullptr!");

        // SDL3：SDL_JoystickInstanceID -> SDL_GetJoystickID，SDL_JoystickName -> SDL_GetJoystickName
        m_InstanceID = SDL_GetJoystickID(joystick);
        m_sName = std::string{ SDL_GetJoystickName(joystick) };

        auto num_axis = SDL_GetNumJoystickAxes(joystick);
        auto num_balls = SDL_GetNumJoystickBalls(joystick);
        auto num_hats = SDL_GetNumJoystickHats(joystick);
        auto num_buttons = SDL_GetNumJoystickButtons(joystick);

        NEXUS_LOG("Gamepad name: {}", m_sName);
        NEXUS_LOG("Gamepad id: {}", m_InstanceID);
        NEXUS_LOG("Gamepad num axes: {}", num_axis);
        NEXUS_LOG("Gamepad num balls: {}", num_balls);
        NEXUS_LOG("Gamepad num hats: {}", num_hats);
        NEXUS_LOG("Gamepad num buttons: {}", num_buttons);
    }

    void Gamepad::Update()
    {
        for (auto& [btn, button] : m_mapButtons)
            button.Reset();
    }

    void Gamepad::OnBtnPressed(int btn)
    {
        if (btn == NEXUS_GP_BTN_UNKNOWN)
        {
            NEXUS_ERROR("Gamepad button [{}] is unknown!", btn);
            return;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Gamepad button [{}] does not exist!", btn);
            return;
        }

        btnItr->second.Update(true);
    }

    void Gamepad::OnBtnReleased(int btn)
    {
        if (btn == NEXUS_GP_BTN_UNKNOWN)
        {
            NEXUS_ERROR("Gamepad button [{}] is unknown!", btn);
            return;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Gamepad button [{}] does not exist!", btn);
            return;
        }

        btnItr->second.Update(false);
    }

    const bool Gamepad::IsBtnPressed(int btn) const
    {
        if (btn == NEXUS_GP_BTN_UNKNOWN)
        {
            NEXUS_ERROR("Gamepad button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Gamepad button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bIsPressed;
    }

    const bool Gamepad::IsBtnJustPressed(int btn) const
    {
        if (btn == NEXUS_GP_BTN_UNKNOWN)
        {
            NEXUS_ERROR("Gamepad button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Gamepad button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bJustPressed;
    }

    const bool Gamepad::IsBtnJustReleased(int btn) const
    {
        if (btn == NEXUS_GP_BTN_UNKNOWN)
        {
            NEXUS_ERROR("Gamepad button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            NEXUS_ERROR("Gamepad button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bJustReleased;
    }

    const bool Gamepad::IsGamepadPresent() const
    {
        return m_pController != nullptr && SDL_HasGamepad();
    }

    const Sint16 Gamepad::GetAxisPosition(Uint8 axis)
    {
        auto axisItr = m_mapAxisValues.find(axis);
        if (axisItr == m_mapAxisValues.end())
        {
            NEXUS_ERROR("Axis [{}] does not exist!", axis);
            return 0;
        }

        return axisItr->second;
    }

    void Gamepad::SetAxisPositionValue(Uint8 axis, Sint16 value)
    {
        auto axisItr = m_mapAxisValues.find(axis);
        if (axisItr == m_mapAxisValues.end())
        {
            NEXUS_ERROR("Axis [{}] does not exist!", axis);
            return;
        }

        axisItr->second = value;
    }
}
