#pragma once

#include <std_includes.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>

#include "vects.hpp"

// defined here
class GraphicSystem;

// extern
class GraphicObject;
class Texture;

class GraphicSystem
{
    SDL_Renderer *render;
    SDL_Window *window;
    Vect2i window_size;
public:
    Vect2i camera_pos;
    float camera_zoom = 1;
    /**
     * Bucket for graphic objects, stored along their draw height/z.
     * Lowest z values are drawn first and occluded by higher z vallues.
     */
    set<pair<int, shared_ptr<GraphicObject>>> bucket;

    GraphicSystem(Vect2i window_size);

    shared_ptr<Texture> loadTexture(string filepath);

    void registerObj(shared_ptr<GraphicObject> obj);

    void unregisterObj(shared_ptr<GraphicObject> obj);

    /**
     * @brief Transform position in world space to screen space
     * @param pos
     * @return Vect2i
     */
    Vect2i screenTransform(Vect2i pos)
    {
        return Vect2i((pos.x - camera_pos.x + window_size.x / 2) * camera_zoom, (pos.y - camera_pos.y + window_size.y / 2) * camera_zoom);
    }

    void update();
};
