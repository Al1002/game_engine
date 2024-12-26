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

#include <iostream>      // cout
#include <memory>        // smart pointers
#include <list>          // s.e.
#include <unordered_set> // hash table
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <thread_pools.hpp>

#include <vects.hpp> // Mathematical vectors
#include <clock.h>   // clock/timer utility

#include <colors.h> // #defined RGB_COLORs
#include <objects.hpp>
#include <events.hpp>
#include <engine.hpp>

// general ADTs
using std::list;
using std::map;
using std::mutex;
using std::queue;
using std::unordered_map;
using std::unordered_multimap;
using std::unordered_set;
using std::vector;

// smart pointer relevant
using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast; 
using std::make_shared;
using std::make_unique;
using std::move;


class GravityObject : public Object2D
{
public:
    Vect2f accel;
    Vect2f gravity = Vect2f(0, 1200);
    float terminal_velocity = 600;
    void loop(double delta) override
    {
        accel += gravity * delta;
        if (accel.length() > terminal_velocity)
        {
            accel = accel * terminal_velocity / accel.length();
        }
        offset = offset + accel * delta; // bad for small delta; typing is hard as pos migth require some FLOPS
        Object::loop(delta);
    }
};

class FixedJumpHandler : public Handler<KeyboardEvent>
{
public:
    shared_ptr<Object2D> that;
    FixedJumpHandler(shared_ptr<Object2D> t)
    {
        that = t;
    }
    void handle(shared_ptr<KeyboardEvent> e) override
    {
        if (e->sdl_event.keysym.sym == 'a')
            that->offset.x -= 100;
        if (e->sdl_event.keysym.sym == 'd')
            that->offset.x += 100;
        if (e->sdl_event.keysym.sym == 'w')
            that->offset.y -= 100;
        if (e->sdl_event.keysym.sym == 's')
            that->offset.y += 100;
    }
};

class FixedAccelHandler : public Handler<KeyboardEvent>
{
public:
    shared_ptr<GravityObject> that;
    FixedAccelHandler(shared_ptr<GravityObject> t)
    {
        that = t;
    }
    void handle(shared_ptr<KeyboardEvent> e) override
    {
        if (e->sdl_event.keysym.sym == 'a')
            // that->accel += Vect2f(-100, 0);
            that->accel.x = -600;
        if (e->sdl_event.keysym.sym == 'd')
            // that->accel += Vect2f(100, 0);
            that->accel.x = 600;
        if (e->sdl_event.keysym.sym == 'w')
            // that->accel += Vect2f(0, -100);
            that->accel.y = -600;
        if (e->sdl_event.keysym.sym == 's')
            // that->accel += Vect2f(0, 100);
            that->accel.y = 600;
    }
};

int main()
{
    // TODO: deepcpy for object cloning, analog to packed scenes in godot
    // TODO: due to smart pointer lambda shenanigans, a decent chunk of code is ugly template casts
    // this is simply a byproduct of c++ smart pointers not being part of the grammar (why?!)
    // as such im considering #defs as a fix 
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    auto e = Engine::create();

    // backround
    auto texture = e->gsys->loadTexture("./resources/flappy_sprite_sheet.png");
    texture->defineSprite({0, 0, 128, 256}, "background");
    texture->defineSprite({0, 511 - 24, 24, 24}, "bird");
    
    auto sprite = texture->buildSprite("background");
    sprite->scaleX(400);
    e->addObj(sprite);

    // bird
    auto object = make_shared<GravityObject>();
    object->attachInitBehaviour([&](Object *self){
        static_cast<GravityObject*>(self)->offset = {200, 200};
        auto sprite = texture->buildSprite("bird");
        sprite->scaleX(100);
        sprite->setDrawHeight(1);
        self->addChild(sprite);
        self->getEngine()->disp->addEventHandler(make_shared<FixedAccelHandler>(static_pointer_cast<GravityObject>(self->shared_from_this())));
    });

    object->attachLoopBehaviour([](Object *self, double delta){
        shared_ptr<GravityObject> t_self = static_pointer_cast<GravityObject>(self->shared_from_this());
        if(t_self->offset.y > 500)
            t_self->offset.y = 500;
    });

    e->addObj(object);

    // pipe
    
    e->start();
    SDL_Quit();
    return 0;
}
