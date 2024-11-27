/**
 * @file main.cpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*
    Preamble
    Death to premature optimization. Write code for function, not speed nor scalability.
    Objects with user access may not use raw pointers.
    Objects with need for performance may break any rules as nescessary.
*/

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

class EngineController : public Object
{
    Clock timeout;
public:
    void init()
    {
        timeout.start_timer();
    }
    void loop();
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

    static std::shared_ptr<Engine> create()
    {
        auto e = new Engine;
        auto shared = shared_ptr<Engine>(e);
        e->view = weak_ptr<Engine>(shared);
        e->gsys = new GraphicSystem;
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
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    stop = true;
                    break;
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
        for (auto iter = obj->children.begin(); iter != obj->children.end(); iter++)
        {
            addObjRecursive(*iter);
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

    void updateAll()
    {
        this->update();
        gsys->update();
        std::cout << "Tick end" << "\n\n";
    }

    void update()
    {
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

class Event{};

class EventDispatcher
{
public:
    queue<Event> waiting;
    queue<Event> active;
    mutex
    unordered_multimap<SDL_Scancode, function<void(void)>> handles;
    void addEventHandler(SDL_Scancode event_type, function<void(void)> handle)
    {
        handles.emplace(event_type, handle);
    }
    void addEvent()
    {

    };
};

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    auto e = Engine::create();
    
    auto texture = e->gsys->loadTexture("/home/sasho_b/Coding/game_engine/resources/placeholder.png");
    texture->scaleX(100);
    e->addObj(texture);

    auto object = make_shared<Object>();
    object->attachInitBehaviour( [](Object* self) {
        auto sprite = make_shared<GraphicObject>();
        self->addChild(static_pointer_cast<Object>(sprite));
        sprite->global = {200, 200};
        sprite->size = {50, 400};
    });
    
    object->attachLoopBehaviour( [](Object* self) {
        auto sprite = dynamic_pointer_cast<GraphicObject>(self->getChild(0));
        sprite->global.x++;
    });
    
    e->addObj(object);

    e->start();
    SDL_Quit();
    return 0;
}
