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

#include <std_includes.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <thread_pools.hpp>

#include <vects.hpp> // Mathematical vectors
#include <clock.h>   // clock/timer utility

#include <colors.h> // #defined RGB_COLORs

// defined here
class Engine;

// extern
class Object;
class Event;
class EngineController;
class GraphicSystem;
class EventDispatcher;
#include <objects.hpp>

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
    unordered_set<shared_ptr<Object>> dead_bucket;
    shared_ptr<Object> root; ///< root object
    double tick_delay;       // minimum time between updates
    std::mutex run;          // signifies the thread running the engine
    Clock clock;
    std::atomic<bool> stop; // set to true to stop engine
    std::mutex operation;   // can either be held when runing an update or changing engine settings

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
        return 0;
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
    
    inline void addChild(shared_ptr<Object> child)
    {
        root->addChild(child);
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

    template <typename T = Object>
    inline shared_ptr<T> getChild(int index)
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
    inline shared_ptr<T> get(int index)
    {
        return getChild<T>(index);
    }
#if 0
    template <typename T = Object>
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
    template <typename T>
    inline shared_ptr<T> get(std::vector<int> indices)
    {
        return getChild<T>(indices);
    }
#endif
    template <typename T = Object2D>
    inline shared_ptr<T> getChild(string path)
    {
        return root->getChild<T>(path);
    }

    template <typename T>
    inline shared_ptr<T> get(string path)
    {
        return getChild<T>(path);
    }

    /**
     * @brief Remove child by index.
     * @param index position of the child in the child list
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if the child index is out of range
     */
    inline shared_ptr<Object> removeChild(int index)
    {
        return root->removeChild(index);
    }

    /**
     * @brief Remove child by name.
     * @param name name of the child to be removed
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if no child has that name
     */
    inline shared_ptr<Object> removeChild(string name)
    {
        return root->removeChild(name);
    }
};
