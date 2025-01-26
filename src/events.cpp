#include <events.hpp>

KeyboardEvent::KeyboardEvent(SDL_Event e)
{
    if(e.type == SDL_KEYDOWN)
        is_down = true;
    else if(e.type == SDL_KEYUP)
        is_down = false;
    else
        throw std::runtime_error("KeyboardEvent instance initialized with non-keyboard SDL_Event");
    this->sdl_event = e.key;
}

MouseButtonEvent::MouseButtonEvent(SDL_Event e)
{
    if(e.type == SDL_MOUSEBUTTONDOWN)
        is_down = true;
    else if(e.type == SDL_MOUSEBUTTONUP)
        is_down = false;
    else
        throw std::runtime_error("MouseButtonEvent instance initialized with non-mouse SDL_Event");
    this->sdl_event = e.button;
}