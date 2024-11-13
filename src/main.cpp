/*
    Preamble:
    Death to premature optimization. Write code for function, not speed nor scalability.
    Objects with user access may not use raw pointers.
    Objects with need for performance may break any rules as nescessary.

*/

#include <iostream> // cout
// #include <err.h> // err stream
#include <memory> // smart pointers
#include <list>   // s.e.
#include <thread_pools.hpp>
#include <clock.cpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <colors.h>
#include <vects.hpp>
#include <SDL2/SDL_image.h>
#include <objects.hpp>

using std::list;

using std::move;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::weak_ptr;



class MyObj : public Object
{
    int data = 42;
    void init()
    {
        
    }
    void loop()
    {
        std::cout << data << '\n';
    }
};

class EngineController;
class Engine
{
    friend EngineController;
    
    weak_ptr<Engine> view;
    thread_pool::dynamic_pool workers;
    list<shared_ptr<Object>> objs;
    double tick_delay; // minimum time between updates
    Clock clock;
    std::mutex run;         // signifies the thread running the engine
    std::atomic<bool> stop; // set to true to stop engine
    std::mutex operation;   // can either be held when runing an update or changing engine settings
    
    Engine() {};
public:
    GraphicSystem *gsys;

    static std::shared_ptr<Engine> create()
    {
        auto e = new Engine;
        auto shared = std::shared_ptr<Engine>(e);
        e->view = weak_ptr<Engine>(shared);
        e->gsys = new GraphicSystem;
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
            tick();
            clock.delta_time(tick_delay);
        }
        run.unlock();
    }
    std::shared_ptr<Object> getObj()
    {
        return *objs.begin();
    }
    void addObj(std::shared_ptr<Object> obj)
    {
        obj->engine_view = weak_ptr<Engine>(view);
        objs.push_back(obj);
    }
    void tick()
    {
        for (auto iter = objs.begin(); iter != objs.end(); iter++)
        {
            Object &obj = *iter->get();
            workers.enqueue(std::bind(&Object::loop, &obj));
        }
        gsys->update();
        std::cout << "Tick end" << "\n\n";
    }
};

class EngineController : public Object
{
    Clock timeout;

public:
    void init()
    {
        timeout.start_timer();
    }
    void loop()
    {
        double time = timeout.get_time();
        std::cout << "Lifetime: " << time << '\n';
        if (time > 5)
        {
            auto shared_engine = engine_view.lock();
            if (!shared_engine)
                return;
            shared_engine->tick_delay = 1.0/20;
        }
        if (time > 30)
        {
            auto shared_engine = engine_view.lock();
            if (!shared_engine)
                return;
            shared_engine->stop = true;
        }
    }
};

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    auto e = Engine::create();
    EngineController *c = new EngineController;
    c->init();
    e->addObj(shared_ptr<Object>(new MyObj));
    e->addObj(shared_ptr<Object>(c));
    
    auto texture = e->gsys->loadTexture("/home/sasho_b/Coding/game_engine/resources/placeholder.png");
    texture->scaleX(100);
    e->gsys->addObj(shared_ptr<GraphicObject>(move(texture)));
    e->gsys->addObj(shared_ptr<GraphicObject>(new GraphicObject));
    e->start();

    SDL_Quit();
    return 0;
}
