/**
 * @file events.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-19
 * @copyright Copyright (c) 2025
 */
#pragma once

// defined here
class KeyboardEvent;
class MouseButtonEvent;

// extern
#include "base_event.hpp"
#include <SDL2/SDL.h>

/**
 * @brief Wrapper for SDL_KeyboardEvent. Created by keypresses.
 */
class KeyboardEvent : public Event
{
public:
    bool is_down;
    SDL_KeyboardEvent sdl_event;
    KeyboardEvent(SDL_Event e);
};

/**
 * @brief Wrapper for SDL_MouseButtonEvent. Created by mouse clicks.
 */
class MouseButtonEvent : public Event
{
public:
    bool is_down = true;
    SDL_MouseButtonEvent sdl_event;
    MouseButtonEvent(SDL_Event e);
};
