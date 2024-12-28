/**
 * @file engine.hpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <iostream>         // cout
#include <memory>           // smart pointers
#include <list>             // s.e.
#include <unordered_set>    // hash table

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <thread_pools.hpp>

#include <vects.hpp>        // Mathematical vectors
#include <clock.h>          // clock/timer utility

#include <colors.h>         // #defined RGB_COLORs
#include <objects.hpp>
#include <events.hpp>
#include <dispatcher.hpp>
#include <graphic_system.hpp>

using std::list;
using std::unordered_map;
using std::unordered_multimap;
using std::unordered_set;
using std::queue;
using std::mutex;

using std::move;
using std::make_shared;
using std::make_unique;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::shared_ptr;
using std::weak_ptr;

class HardwareEventBuilder
{
public:
    static shared_ptr<Event> build(SDL_Event e);
};

class Engine : public std::enable_shared_from_this<Engine>
{
    friend EngineController;
    
    thread_pool::static_pool workers;
    unordered_set<shared_ptr<Object>> bucket;
    shared_ptr<Object> root; ///< root object
    double tick_delay;  // minimum time between updates
    std::mutex run;         // signifies the thread running the engine
    Clock clock;
    std::atomic<bool> stop; // set to true to stop engine
    std::mutex operation;   // can either be held when runing an update or changing engine settings
    Vect2i window_size;
public:
    GraphicSystem *gsys;
    EventDispatcher *disp;

    /**
     * @brief Call before creating any engine objects. Enables SDL utilities and other global state required for the Engine class to work. 
     * 
     */
    static int enable()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
        {
            std::cerr << "Failed to initialize SDL_subsystems: " << SDL_GetError() << '\n';
            return 1;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 2048) < 0)
        {
            std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << '\n';
            return 1;
        }
        srand(time(NULL));
    }

    /**
     * @brief Call after all engine objects are destroyed. Dissables SDL utilities and other global state required for the Engine class to work. 
     * 
     */
    static void disable()
    {
        Mix_CloseAudio();
        SDL_Quit();
    }

    Engine(Vect2i window_size = {1024, 720});


    void start();

    /**
     * @brief Add object for updates and initialization
     * 
     * @param obj 
     */
    void registerObj(shared_ptr<Object> obj);


    void unregisterObj(shared_ptr<Object> obj);


    void update(double delta);


    // Composition with root object

    template<typename T = Object>
    inline void addChild(shared_ptr<T> child)
    {
        root->addChild<T>(child);
        registerObj(child);
    }

    /**
     * @brief Short alias for addChild.
     * 
     * @param child child object
     */
    inline void add(shared_ptr<Object> child)
    {
        addChild(child);
    }


    template<typename T = Object>
    inline shared_ptr<T> getChild(int index)
    {
        return root->getChild<T>(index);
    }

    /**
     * @brief Short alias for getChild().
     * 
     * @param index position of the child in the child list
     * @return shared_ptr<Object>
     * @throws out_of_range exception if the child index is out of range 
     */
    template<typename T = Object>
    inline shared_ptr<T> get(int index)
    {
        return getChild<T>(index);
    }


    template<typename T = Object>
    inline shared_ptr<T> getChild(std::vector<int> indices)
    {
        return root->getChild<T>(indices);
    }

    /**
     * @brief Short alias for getChild.
     * 
     * @param index position of the child in the child list
     * @tparam T the type to downcast the child to
     * @return shared_ptr<Object>
     * @throws std::out_of_range exception if the child index is out of range
     * @throws  exception if the type is not an ancestor of the child's actual type
     */
    template<typename T>
    inline shared_ptr<T> get(std::vector<int> indices)
    {
        return getChild<T>(indices);
    }
};
