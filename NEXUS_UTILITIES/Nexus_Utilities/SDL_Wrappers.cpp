#include "SDL_Wrappers.h"
#include <iostream>

void NEXUS_UTIL::SDL_Destroyer::operator()(SDL_Window *window) const
{
    SDL_DestroyWindow(window);
    std::cout << "SLD窗口已销毁" << std::endl;
}

void NEXUS_UTIL::SDL_Destroyer::operator()(SDL_Gamepad *controller) const
{
    
}

void NEXUS_UTIL::SDL_Destroyer::operator()(SDL_Cursor *cursor) const
{
   
}

Controller make_shared_controller(SDL_Gamepad *controller)
{
    return Controller();
}

Cursor make_shared_cursor(SDL_Cursor *cursor)
{
    return Cursor();
}
