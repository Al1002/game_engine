#pragma once

#include <iostream> // cout
#include <memory> // smart pointers
#include <list>   // s.e.
#include <unordered_set>
#include <functional>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <thread_pools.hpp>

#include <clock.h>
#include <vects.hpp>        // Mathematical vectors

#include <colors.h>

using std::list;
using std::string;
using std::function;
using std::unordered_set;

using std::move;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;

class Engine;

class Transform
{
public:
    Vect2i offset; // offset relative to parent, or global position if root
    Vect2i global; // actual position, result of parent.global + offset
    Vect2i base_size;
    Vect2i size;
    Vect2f scale;
};

class Object
{
    friend Engine;
protected:
    weak_ptr<Engine> engine_view;
    list<shared_ptr<Object>> children;
    function<void(Object*)> init_behavior;
    function<void(Object*)> loop_behavior;
public:
    virtual void init();
    
    virtual void loop();
    
    void addChild(shared_ptr<Object> child);
    
    shared_ptr<Object> getChild(int index);
    
    /**
     * @brief A callable which is called in the object's loop
     * 
     * @param behaviour 
     */
    void attachInitBehaviour(function<void(Object*)> behavior);
    
    /**
     * @brief A callable which is called in the object's init
     * 
     * @param behaviour 
     */
    void attachLoopBehaviour(std::function<void(Object*)> behavior);
};

class Object2D : virtual public Object, virtual public Transform
{
};

class GraphicSystem;

class GraphicObject : public Object2D
{
    friend GraphicSystem;
public:
    SDL_Renderer* render_view; // due to SDL shenanigans, smart pointers are not an option
    enum Color
    {
        RED,
        GREEN,
        BLUE
    };
    Color color = RED;
    static void setDrawColor(SDL_Renderer *render, Color c);
    virtual void draw();
};

/**
 * @brief Graphic object displaying a texture.
 * 
 */
class TextureObject : public GraphicObject
{
    SDL_Texture* texture;
public:
    void setTexture(SDL_Renderer* render, string filepath);
    
    /**
     * @brief Scale size to have a width of 'x'
     */
    void scaleX(int x);

    /**
     * @brief Scale size to have a height of 'y'
     */
    void scaleY(int y);

    void draw() override;
};

class GraphicSystem
{
public:
    SDL_Renderer *render;
    SDL_Window *window;
    unordered_set<shared_ptr<GraphicObject>> bucket;
    
    GraphicSystem();
    
    shared_ptr<TextureObject> loadTexture(string filepath);
    
    void addObj(shared_ptr<GraphicObject> obj);
    
    void removeObj(shared_ptr<GraphicObject> obj);

    void update();

};

