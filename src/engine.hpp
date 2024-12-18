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
    static shared_ptr<Event> build(SDL_Event e)
    {
        if(e.type == SDL_KEYDOWN)
            return static_pointer_cast<Event>(make_shared<KeyboardEvent>(e));
        else
            return shared_ptr<Event>();
    }

};

class EventDispatcher
{
    queue<shared_ptr<Event>> buffers[2];
    queue<shared_ptr<Event>> *back;  // and consumed
    queue<shared_ptr<Event>> *front; // event go in front  
public:
    mutex front_m;
    unordered_set<shared_ptr<HandlerI>> handles;

    EventDispatcher(){
        back = &buffers[0];
        front = &buffers[1];
    }

    // err - function doesnt support comparison, aka build a wrapper that does; unordered_set<function<void(shared_ptr<Event>)>> handles;
    void addEventHandler(shared_ptr<HandlerI> handle)
    {
        handles.emplace(handle);
    }

    void addEvent(shared_ptr<Event> e)
    {
        front_m.lock();
        front->push(e);
        front_m.unlock();
    }

    void dispatch()
    {
        front_m.lock();
        auto temp = back;
        back = front;
        front = temp;
        front_m.unlock();
    
        while (!back->empty()) {
            auto event = back->front();
            back->pop();
            for (auto& handler : handles)
            {
                auto hash = typeid(*event.get()).hash_code();
                if(handler->event_type == hash)
                    (*handler)(event);
            }
        }
    }
};

class Engine
{
    friend EngineController;
    
    weak_ptr<Engine> view;
    thread_pool::dynamic_pool workers;
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

    static std::shared_ptr<Engine> create()
    {
        auto e = new Engine;
        auto shared = shared_ptr<Engine>(e);
        e->view = weak_ptr<Engine>(shared);
        e->gsys = new GraphicSystem;
        e->disp = new EventDispatcher;
        EngineController *controler = new EngineController;
        controler->init();
        e->addObj(shared_ptr<Object>(controler));
        return shared;
    }

    void start()
    {
        if (!run.try_lock())
        {
            std::cerr << "Engine already running" << '\n';
            throw std::exception();
        }
        stop = false;
        tick_delay = 1.0 / 60;
        while (!stop)
        {
            // update hardware events
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    stop = true;
                    break;
                }
                else
                {
                    auto event = HardwareEventBuilder::build(e);
                    if(event.get() != nullptr)
                        disp->addEvent(event);
                }
            }
            clock.delta_time(tick_delay);
            updateAll();
        }
        run.unlock();
    }

    /**
     * @brief Add root object
     * 
     * @param obj 
     */
    void addObj(shared_ptr<Object> obj)
    {
        obj->engine_view = weak_ptr<Engine>(view);
        root_objects.insert(obj);
        addObjRecursive(obj);
    }
    
    void removeObj(shared_ptr<Object> obj)
    {
        root_objects.erase(obj);
        removeObjRecursive(obj);
    }

    /**
     * @brief Add object to processing wake up list
     * 
     * @param obj 
     */
    void addObjRecursive(shared_ptr<Object> &obj)
    {
        obj->init();
        bucket.insert(obj);
        shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
        if(graphic)
        {
            gsys->addObj(graphic);
        }
        for (auto& child : obj->children)
        {
            addObjRecursive(child);
        }
    }
    
    void removeObjRecursive(shared_ptr<Object> &obj)
    {
        for (auto iter = obj->children.begin(); iter != obj->children.end(); iter++)
        {
            addObjRecursive(*iter);
        }
        shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
        if(graphic)
        {
            gsys->removeObj(graphic);
        }
        bucket.erase(obj);
    }

    /**
     * @brief 
     * 
     */
    void updateAll()
    {
        this->update();
        disp->dispatch();
        gsys->update();
        std::cout << "Tick end" << "\n\n";
    }

    void update()
    {
        // loops
        for (auto iter = bucket.begin(); iter != bucket.end(); iter++)
        {
            workers.enqueue(std::bind(&Object::loop, *iter));
        }
    }
};

void EngineController::loop()
{
    static Clock delta_c;
    double delta = delta_c.delta_time();
    double time = timeout.get_time();
    std::cout << string() + "Lifetime: " + std::to_string(time) + " (" + std::to_string(delta) + ")" << '\n';
    if (time > 30)
    {
        auto shared_engine = engine_view.lock();
        if (!shared_engine)
            return;
        shared_engine->stop = true;
    }
}
