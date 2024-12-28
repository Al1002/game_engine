/**
 * @file engine.cpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <engine.hpp>

class KeyboardEvent;
#include <graphic_system.hpp>
#include <dispatcher.hpp>
#include <objects.hpp>
#include <events.hpp>

shared_ptr<Event> HardwareEventBuilder::build(SDL_Event e)
{
    if (e.type == SDL_KEYDOWN)
        return static_pointer_cast<Event>(make_shared<KeyboardEvent>(e));
    else
        return shared_ptr<Event>();
}

Engine::Engine(Vect2i window_size)
{
    gsys = new GraphicSystem(window_size);
    disp = new EventDispatcher;
    root = make_shared<Object>();
    registerObj(root);
    registerObj(make_shared<EngineController>()); // does not exist in root, only bucket - bad
}

void Engine::start()
{
    if (!run.try_lock())
        throw std::runtime_error("Engine already running!");
    stop = false;
    tick_delay = 1.0 / 60;
    // tick_delay = 0; // test at max run speed
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
                if (event.get() != nullptr)
                    disp->addEvent(event);
            }
        }
        auto delta = clock.delta_time(tick_delay);
        std::cout << string() + "Delta: (" + std::to_string(delta) + ")" << '\n';
        std::cout << "Tick start\n";
        this->update(delta);
        disp->dispatch();
        gsys->update();
        std::cout << "Tick end\n\n";
    }
    run.unlock();
}

/**
 * @brief Add object to processing wake up list
 *
 * @param obj
 */
void Engine::registerObj(shared_ptr<Object> obj)
{
    obj->engine_view = weak_from_this();
    obj->init();
    bucket.insert(obj);
    shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
    if (graphic)
    {
        gsys->registerObj(graphic);
    }
    shared_ptr<HandlerI> handle = dynamic_pointer_cast<HandlerI>(obj);
    if (handle)
    {
        disp->addEventHandler(handle);
    }
    for (auto &child : obj->children)
    {
        registerObj(child);
    }
}

void Engine::unregisterObj(shared_ptr<Object> obj)
{
    for (auto iter = obj->children.begin(); iter != obj->children.end(); iter++)
    {
        registerObj(*iter);
    }
    shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
    if (graphic)
    {
        gsys->unregisterObj(graphic);
    }
    bucket.erase(obj);
}

void Engine::update(double delta)
{
    // loops
    for (auto iter = bucket.begin(); iter != bucket.end(); iter++)
    {
        (*iter)->loop(delta);
    }
}

void EngineController::loop(double delta)
{
    double time = timeout.get_time();
    std::cout << string() + "Time: " + std::to_string(time) + "\n";
    if (time > 120)
    {
        auto shared_engine = engine_view.lock();
        if (!shared_engine)
            return;
        shared_engine->stop = true;
    }
}
