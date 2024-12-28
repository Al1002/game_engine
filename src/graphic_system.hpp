#pragma once

#include <memory> // smart pointers

#include <string>
#include <set>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>

#include <objects.hpp>

using std::pair;
using std::set;
using std::string;

using std::shared_ptr;

class GraphicObject;

class Texture;

class GraphicSystem
{
    SDL_Renderer *render;
    SDL_Window *window;
public:
    Vect2i camera_pos;
    float camera_zoom = 1;
    /**
     * Bucket for graphic objects, stored along their draw height/z.
     * Lowest height values are drawn first and occluded by higher height vallues.
     */
    set<pair<int, shared_ptr<GraphicObject>>> bucket;

    GraphicSystem(Vect2i window_size);

    shared_ptr<Texture> loadTexture(string filepath);

    void registerObj(shared_ptr<GraphicObject> obj);

    void unregisterObj(shared_ptr<GraphicObject> obj);

    /**
     * @brief Transform position in world space to camera space
     * 
     * @param pos 
     * @return Vect2i 
     */
    Vect2i cameraTransform(Vect2i pos)
    {
        return Vect2i((pos.x - camera_pos.x) * camera_zoom, (pos.y - camera_pos.y) * camera_zoom);
    }

    void update();
};
