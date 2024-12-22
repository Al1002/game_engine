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
#include <events.hpp>
#include <engine.hpp>

using std::list;
using std::unordered_map;
using std::unordered_multimap;
using std::unordered_set;
using std::queue;
using std::mutex;
using std::vector;

using std::move;
using std::make_shared;
using std::make_unique;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::shared_ptr;
using std::weak_ptr;

class GravityObj : public Object2D
{
public:
    Vect2f accel;
    Vect2f gravity = Vect2f(0, 1200);
    float terminal_velocity = 600;
    void loop(double delta) override
    {
        accel += gravity * delta;
        if(accel.length() > terminal_velocity)
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
        if(e->sdl_event.keysym.sym == 'a')
            that->offset.x-=100;
        if(e->sdl_event.keysym.sym == 'd')
            that->offset.x+=100;
        if(e->sdl_event.keysym.sym == 'w')
            that->offset.y-=100;
        if(e->sdl_event.keysym.sym == 's')
            that->offset.y+=100;
    }
};

class FixedAccelHandler : public Handler<KeyboardEvent>
{
public:
    shared_ptr<GravityObj> that;
    FixedAccelHandler(shared_ptr<GravityObj> t)
    {
        that = t;
    }
    void handle(shared_ptr<KeyboardEvent> e) override
    {
        if(e->sdl_event.keysym.sym == 'a')
            //that->accel += Vect2f(-100, 0);
            0 == 0;
        if(e->sdl_event.keysym.sym == 'd')
            //that->accel += Vect2f(100, 0);
            0 == 0;
        if(e->sdl_event.keysym.sym == 'w')
            //that->accel += Vect2f(0, -100);
            that->accel.y = -600;
        if(e->sdl_event.keysym.sym == 's')
            //that->accel += Vect2f(0, 100);
            that->accel.y = 600;
    }
};

int main()
{
    // NOTE: define diff between texture and sprite
    // TODO: THE MAIN DRAW LOOP IS DONE SINGLE FILE, COSTING 6 OF 16 ms max FOR UPDATES FOR 100 SPRITES
    // OUTSORCE WORK TO THREAD POOL; LOOK AT IF SDL DRAWING IS BLOCKING
    SDL_Init(SDL_INIT_EVERYTHING);
    auto e = Engine::create();

    // backround
    auto texture = e->gsys->loadTexture("./resources/background_1.png");
    texture->scaleX(400);
    e->addObj(texture);
    //for(int i = 0; i < 100; i ++) // may be slow cuz of the 440 kbs of repeat memory. who knows
    //    e->addObj(e->gsys->loadTexture("./resources/placeholder.png"));

    // bird
    auto object = make_shared<Object>();
    object->attachInitBehaviour([&](Object* self) {
        self->addChild(make_shared<GravityObj>());
        self->getChild(0)->
            addChild(e->gsys->loadTexture("resources/bird_1.png"));
        self->getChild<GraphicObject>({0, 0})->
            offset = {200, 200};
        self->getChild<TextureObject>({0, 0})->
            scaleX(200);
        self->getChild<TextureObject>({0, 0})->
            setDrawHeight(1);
        self->getEngine()->disp->addEventHandler(make_shared<FixedAccelHandler>(self->getChild<GravityObj>(0)));
    });

    object->attachLoopBehaviour( [](Object* self, double delta) {
        auto sprite = self->getChild<Object2D>(0);
    });

    e->addObj(object);

    // pipe 
    //object = make_shared<Object>();

    e->start();
    SDL_Quit();
    return 0;
}
