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


shared_ptr<Event> HardwareEventBuilder::build(SDL_Event e)
{
    if (e.type == SDL_KEYDOWN)
        return static_pointer_cast<Event>(make_shared<KeyboardEvent>(e));
    else
        return shared_ptr<Event>();
}


std::shared_ptr<Engine> Engine::create()
{
    auto e = new Engine;
    auto shared = shared_ptr<Engine>(e);
    e->view = weak_ptr<Engine>(shared);
    e->gsys = new GraphicSystem;
    //e->gsys->workers = &e->workers; // FIXME
    e->disp = new EventDispatcher;
    EngineController *controler = new EngineController;
    controler->init();
    e->addObj(shared_ptr<Object>(controler));
    return shared;
}


void Engine::start()
{
    if (!run.try_lock())
    {
        std::cerr << "Engine already running" << '\n';
        throw std::exception();
    }
    stop = false;
    tick_delay = 1.0 / 60;
    //tick_delay = 0; // test at max run speed
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
 * @brief Add root object
 *
 * @param obj
 */
void Engine::addObj(shared_ptr<Object> obj)
{
    obj->engine_view = weak_ptr<Engine>(view);
    if(obj->parent_view.lock() == nullptr)
        root_objects.insert(obj);
    addObjRecursive(obj);
}


void Engine::removeObj(shared_ptr<Object> obj)
{
    root_objects.erase(obj);
    removeObjRecursive(obj);
}

/**
 * @brief Add object to processing wake up list
 *
 * @param obj
 */
void Engine::addObjRecursive(shared_ptr<Object> &obj)
{
    obj->init();
    bucket.insert(obj);
    shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
    if (graphic)
    {
        gsys->addObj(graphic);
    }
    for (auto &child : obj->children)
    {
        addObjRecursive(child);
    }
}


void Engine::removeObjRecursive(shared_ptr<Object> &obj)
{
    for (auto iter = obj->children.begin(); iter != obj->children.end(); iter++)
    {
        addObjRecursive(*iter);
    }
    shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
    if (graphic)
    {
        gsys->removeObj(graphic);
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
