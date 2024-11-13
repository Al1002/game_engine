#pragma once

#include <iostream> // cout
// #include <err.h> // err stream
#include <memory> // smart pointers
#include <list>   // s.e.
#include <thread_pools.hpp>
#include <clock.cpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>
#include <colors.h>
#include <string>
#include <functional>

using std::list;
using std::string;

using std::move;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;

class Engine;

class Transform
{
    Vect2i offset;
    float scale;
};

class Object
{
    friend Engine;
protected:
    weak_ptr<Engine> engine_view;
    list<shared_ptr<Object>> children;
    std::function behaviour;
public:
    virtual void init() = 0;
    virtual void loop()
    {
        
    }
    void addChild(shared_ptr<Object> child) 
    {
        if(std::find(children.begin(), children.end(), child) != children.end());
            children.push_back(child);
    }
    shared_ptr<Object> getChild(int index)
    {
        if(children.size() >= index)
            throw 42;
        auto it = children.begin();
        std::advance(it, index);
        return *it;
    }
    /**
     * @brief A callable which is called in the object's loop
     * 
     * @param behaviour 
     */
    void attachBehaviour(std::function behaviour)
    {

    }
};

class Object2D : public Object
{
public:
    Vect2f pos;
    
};

class GraphicObject : public Object
{
public:
    enum RTTI
    {
        RECT,
        BACKGROUND,
        SPRITE
    };
    enum Color
    {
        RED,
        GREEN,
        BLUE
    };
    Color color = RED;
    static void setDrawColor(SDL_Renderer *render, Color c)
    {
        switch (c)
        {
        case RED:
            SDL_SetRenderDrawColor(render, RGB_RED, 255);
            break;
        case GREEN:
            SDL_SetRenderDrawColor(render, RGB_GREEN, 255);
            break;
        case BLUE:
            SDL_SetRenderDrawColor(render, RGB_BLUE, 255);
            break;
        default:
            SDL_SetRenderDrawColor(render, RGB_MAGENTA, 255);
        }
    }
    virtual void draw(SDL_Renderer *render)
    {
        setDrawColor(render, color);
        SDL_Rect r = {100, 100, 100, 100};
        SDL_RenderFillRect(render, &r);
    }
    void init() final
    {
    }
    void loop() final
    {
    }
};

/**
 * @brief Graphic object displaying a texture.
 * 
 */
class TextureObject : public GraphicObject
{
    Vect2i texture_size;
    SDL_Texture* texture;
public:
    Vect2i pos;
    Vect2i size;
    void setTexture(SDL_Renderer* render, string filepath)
    {
        texture = IMG_LoadTexture(render, filepath.c_str());
        if(!texture)
            std::cout<<IMG_GetError()<<'\n';
        SDL_QueryTexture(texture, NULL, NULL, &texture_size.x, &texture_size.y);
        size = texture_size;
    }
    /**
     * @brief Scale image to have a width of 'x'
     */
    void scaleX(int x)
    {
        size = texture_size / texture_size.x * x;
    }
    /**
     * @brief Scale image to have a height of 'y'
     */
    void scaleY(int y)
    {
        size = texture_size / texture_size.y * y;
    }
    virtual void draw(SDL_Renderer *render)
    {
        int w, h;
        SDL_Rect dest = {pos.x, pos.y, size.x, size.y};
        if(SDL_RenderCopy(render, texture, NULL, &dest))
            std::cout<<SDL_GetError()<<'\n';
    }
};

class GraphicSystem
{
public:
    SDL_Renderer *render;
    SDL_Window *window;
    list<shared_ptr<GraphicObject>> objs;
    GraphicSystem()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        
        }
        window = SDL_CreateWindow("Window Name", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 840, SDL_WINDOW_SHOWN);
        render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        //SDL_SetRenderDrawColor(render, RGB_WHITE, 255);
        //SDL_RenderClear(render);
        //SDL_RenderPresent(render);
    }
    unique_ptr<TextureObject> loadTexture(string filepath)
    {
        unique_ptr<TextureObject> texture = make_unique<TextureObject>();
        texture->setTexture(render, filepath);
        return move(texture);
    }
    shared_ptr<GraphicObject> getObj()
    {
        return *objs.begin();
    }
    void addObj(std::shared_ptr<GraphicObject> obj)
    {
        objs.push_back(obj);
    }
    void update()
    {
        SDL_SetRenderDrawColor(render, RGB_WHITE, 255);
        SDL_RenderClear(render);
        for (auto iter = objs.begin(); iter != objs.end(); iter++)
        {
            GraphicObject &obj = *iter->get();
            obj.draw(render);
        }
        SDL_RenderPresent(render);
        std::cout << "Frame end" << "\n\n";
    }
};

