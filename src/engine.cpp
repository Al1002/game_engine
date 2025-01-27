/**
 * @file engine.cpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 * @copyright Copyright (c) 2024
 */

#include <engine.hpp>

class KeyboardEvent;
#include <graphic_system.hpp>
#include <dispatcher.hpp>
#include <objects.hpp>
#include <events.hpp>
#include <physics.hpp>

int Engine::enable()
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

void Engine::disable()
{
    Mix_CloseAudio();
    SDL_Quit();
}

shared_ptr<Event> HardwareEventBuilder::build(SDL_Event e)
{
    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        return static_pointer_cast<Event>(make_shared<KeyboardEvent>(e));
    else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
        return static_pointer_cast<Event>(make_shared<MouseButtonEvent>(e));
    else
        return shared_ptr<Event>();
}

Engine::Engine(Vect2i window_size, Vect2f gravity, double tick_delay)
{
    gsys = make_shared<GraphicSystem>(window_size);
    disp = make_shared<EventDispatcher>();
    world = make_shared<World>(gravity);
    root = make_shared<Object>();
    tick_delay = 1.0f / tick_delay;
    registerObj(root);
    registerObj(make_shared<EngineController>()); // does not exist in root, only bucket - bad
}

void Engine::start()
{
    if (!run.try_lock())
        throw std::runtime_error("Engine already running!");
    is_stopped = false;
    while (!is_stopped)
    {
        // update hardware events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                is_stopped = true;
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
        world->update();
        std::cout << "Tick end\n\n";
    }
    run.unlock();
}

void Engine::stop()
{
    is_stopped = true;
}

void Engine::registerObj(shared_ptr<Object> obj)
{
    obj->engine_view = enable_shared_from_this<Engine>::weak_from_this();
    obj->init();
    bucket.insert(obj);
    shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
    if (graphic)
    {
        gsys->registerObj(graphic);
    }
    shared_ptr<PhysicsObject> physics = dynamic_pointer_cast<PhysicsObject>(obj);
    if (physics)
    {
        world->registerObj(physics);
    }
    for (auto handle : obj->handlers)
    {
        disp->registerEventHandler(handle);
    }
    for (auto &iter : obj->children)
    {
        registerObj(iter);
    }
}

void Engine::unregisterObj(shared_ptr<Object> obj)
{
    for (auto iter = obj->children.begin(); iter != obj->children.end(); iter++)
    {
        unregisterObj(*iter);
    }
    for (auto handle : obj->handlers)
    {
        disp->registerEventHandler(handle);
    }
    shared_ptr<GraphicObject> graphic = dynamic_pointer_cast<GraphicObject>(obj);
    if (graphic)
    {
        gsys->unregisterObj(graphic);
    }
    shared_ptr<PhysicsObject> physics = dynamic_pointer_cast<PhysicsObject>(obj);
    if (physics)
    {
        world->unregisterObj(physics);
    }
    dead_bucket.emplace(obj);
    obj->engine_view.reset();
}

void Engine::update(double delta)
{
    for(auto obj : dead_bucket)
    {
        bucket.erase(obj);
    }
    dead_bucket.clear();
    for(auto obj : bucket)
    {
        obj->loop(delta);
    }
}

void Engine::addChild(shared_ptr<Object> child)
{
    root->addChild(child);
    registerObj(child);
}

shared_ptr<Object> Engine::removeChild(int index)
{
    auto obj = root->removeChild(index);
    unregisterObj(obj);
    return obj;
}

shared_ptr<Object> Engine::removeChild(string name)
{
    auto obj = root->removeChild(name);
    unregisterObj(obj);
    return obj;
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
        shared_engine->is_stopped = true;
    }
}
