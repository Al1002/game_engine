// Class F file - include leakage, header definitions, no comments 
/**
 * @file engine.hpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 * @copyright Copyright (c) 2024
 */
#pragma once


#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <box2d/box2d.h>

#include <clock.h>   // clock/timer utility

#include "std_includes.hpp"
#include "vects.hpp" // Mathematical vectors
#include "colors.h" // #defined RGB_COLORs

// defined here
class Engine;

// extern
class Object;
class Event;
class EngineController;
class GraphicSystem;
#include "events.hpp"
#include "dispatcher.hpp"
#include "obj_manager.hpp"
#include "objects.hpp"
#include "physics.hpp"

class HardwareEventBuilder
{
public:
    static shared_ptr<Event> build(SDL_Event e);
};

class Engine : public std::enable_shared_from_this<Engine>
{
    friend EngineController;

    unordered_set<shared_ptr<Object>> bucket;
    unordered_set<shared_ptr<Object>> dead_bucket;
    shared_ptr<Object> root; ///< root object

    double tick_delay;       ///< minimum time between updates
    Clock clock;
    std::mutex run;
    std::atomic<bool> is_stopped; ///< set to true to stop engine
    std::mutex operation;   // can either be held when runing an update or changing engine settings

public:
    GraphicSystem *gsys;
    EventDispatcher *disp;
    World *world;

    /**
     * @brief Call before creating any engine objects. Enables SDL utilities and other global state required for the `Engine` class to work.
     */
    static int enable();

    /**
     * @brief Call after all engine objects are destroyed. Dissables SDL utilities and other global state required for the `Engine` class to work.
     */
    static void disable();

    /**
     * @brief Construct a new Engine.
     * @param window_size 
     * @param gravity 
     */
    Engine(Vect2i window_size = {1024, 720}, Vect2f gravity = {0, 1024});

    /**
     * @brief Runs the main engine loop.
     */
    void start();

    /**
     * @brief Tells the engine loop to stop. Will do so at the start of the next frame.
     */
    void stop();

    /**
     * @brief Add object for updates and initialization by all sub-systems.
     * @param obj
     */
    void registerObj(shared_ptr<Object> obj);

    /**
     * @brief Remove object from updates and initialization by all sub-systems.
     * @param obj
     */
    void unregisterObj(shared_ptr<Object> obj);

    void update(double delta);

    // Composition with root object
    
    void addChild(shared_ptr<Object> child);

    /**
     * @brief Short alias for addChild.
     * @param child child object
     */
    void add(shared_ptr<Object> child)
    {
        addChild(child);
    }

    template <typename T = Object>
    shared_ptr<T> getChild(int index)
    {
        return root->getChild<T>(index);
    }

    /**
     * @brief Short alias for getChild().
     * @param index position of the child in the child list
     * @return shared_ptr<Object>
     * @throws out_of_range exception if the child index is out of range
     */
    template <typename T = Object>
    shared_ptr<T> get(int index)
    {
        return getChild<T>(index);
    }

    template <typename T = Object>
    shared_ptr<T> getChild(string path)
    {
        return root->getChild<T>(path);
    }

    template <typename T = Object>
    shared_ptr<T> get(string path)
    {
        return getChild<T>(path);
    }

    /**
     * @brief Remove child by index.
     * @param index position of the child in the child list
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if the child index is out of range
     */
    shared_ptr<Object> removeChild(int index);

    /**
     * @brief Remove child by name.
     * @param name name of the child to be removed
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if no child has that name
     */
    shared_ptr<Object> removeChild(string name);

};
