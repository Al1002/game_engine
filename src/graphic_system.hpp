#pragma once

#include <memory>           // smart pointers

#include <string>
#include <set>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>

#include <objects.hpp>

using std::string;
using std::set;
using std::pair;

using std::shared_ptr;

class GraphicObject;

class Texture;

class GraphicSystem
{
public:
    SDL_Renderer *render;
    SDL_Window *window;

    /**
     * Bucket for graphic objects, stored along their draw height/z.
     * Lowest height values are drawn first and occluded by higher height vallues.
     */
    set<pair<int, shared_ptr<GraphicObject>>> bucket;
    
    GraphicSystem(); 
    
    shared_ptr<Texture> loadTexture(string filepath);
    
    void addObj(shared_ptr<GraphicObject> obj);
    
    void removeObj(shared_ptr<GraphicObject> obj);

    void update();
};
