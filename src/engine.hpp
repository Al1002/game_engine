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

class Engine
{
    friend EngineController;
    
    weak_ptr<Engine> view;
    thread_pool::static_pool workers;
    unordered_set<shared_ptr<Object>> root_objects;
    unordered_set<shared_ptr<Object>> bucket;
    double tick_delay; // minimum time between updates
    std::mutex run;         // signifies the thread running the engine
    Clock clock;
    std::atomic<bool> stop; // set to true to stop engine
    std::mutex operation;   // can either be held when runing an update or changing engine settings
    
    Engine() {};
public:
    GraphicSystem *gsys;
    EventDispatcher *disp;


    static std::shared_ptr<Engine> create();


    void start();

    /**
     * @brief Add root object
     * 
     * @param obj 
     */
    void addObj(shared_ptr<Object> obj);


    void removeObj(shared_ptr<Object> obj);

    /**
     * @brief Add object to processing wake up list
     * 
     * @param obj 
     */
    void addObjRecursive(shared_ptr<Object> &obj);


    void removeObjRecursive(shared_ptr<Object> &obj);


    void update(double delta);
};

/**
 * TODO: objects can either be tracked or added to the scene root.
 * One does not imply the other
 * 
 */