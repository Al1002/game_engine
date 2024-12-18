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
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::shared_ptr;
using std::weak_ptr;
using std::enable_shared_from_this; // allows safe taking of shared_ptr<>(this) instance

class Engine;


class Object : public std::enable_shared_from_this<Object>
{
    friend Engine;
protected:
    weak_ptr<Object> parent_view;
    list<shared_ptr<Object>> children;
    function<void(Object*)> init_behavior;
    function<void(Object*)> loop_behavior;
public:
    weak_ptr<Engine> engine_view;
    
    inline shared_ptr<Engine> getEngine()
    {
        return engine_view.lock();
    }

    virtual void init();
    
    virtual void loop();
    
    void addChild(shared_ptr<Object> child);
    
    template<typename T>
    inline void addChild(shared_ptr<T> child)
    {
        addChild(static_pointer_cast<Object>(child));
    }

    shared_ptr<Object> getChild(int index);
    
    shared_ptr<Object> getChild(const std::vector<int>& indices);
    
    template<typename T>
    inline shared_ptr<T> getChild(int index)
    {
        auto child = dynamic_pointer_cast<T>(getChild(index));
        if(child.get() == nullptr)
            throw "Child not of specified class";
        return child;
    }
    
    template<typename T>
    inline shared_ptr<T> getChild(const std::vector<int>& indices)
    {
        return dynamic_pointer_cast<T>(getChild(indices));
    }
    
    inline void operator[](int index)
    {
        getChild(index);
    }

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
    void attachLoopBehaviour(function<void(Object*)> behavior);

    //avoid attachEventHandler(shared_ptr);
};

class Object2D : virtual public Object
{
public:
    Vect2i offset; // offset relative to parent, or global position if root
    const Vect2i global() // actual position, result of parent.global + offset
    {
        auto parent = dynamic_pointer_cast<Object2D>(parent_view.lock());
        if(parent.get() == nullptr)
            return offset;
        else
            return parent->global() + offset;
    } 
    Vect2i base_size;
    Vect2i size;
    Vect2f scale;

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

class EngineController : public Object
{
    Clock timeout;
public:
    void init();
    
    void loop();
};
