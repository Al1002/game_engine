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
#include <random>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <thread_pools.hpp>

#include <vects.hpp> // Mathematical vectors
#include <clock.h>   // clock/timer utility

#include <colors.h> // #defined RGB_COLORs
#include <objects.hpp>
#include <events.hpp>
#include <engine.hpp>
#include <audio_system.hpp>

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

class Button
{

};


// 
class Box : public Object2D
{
    
};

class GravityObject : public Object2D
{
public:
    Vect2f accel;
    Vect2f gravity = Vect2f(0, 2000);
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

class BirdHandler : public ParentHandler<KeyboardEvent, GravityObject>
{
    void handle(shared_ptr<KeyboardEvent> e, shared_ptr<GravityObject> parent) override
    {
        if (e->sdl_event.keysym.sym == 'a')
            //that->accel.x = -600;
            0 == 0;
        if (e->sdl_event.keysym.sym == 'd')
            //that->accel.x = 600;
            0 == 0;
        if (e->sdl_event.keysym.sym == 'w')
        {
            parent->accel.y = -600;
            parent->get<AudioPlayer>(1)->play();
        }
        if (e->sdl_event.keysym.sym == 's')
            parent->accel.y = 600;
    }
};

class PipeSpawner : public Object2D
{
public:
    Clock time;

    void loop(double delta) override
    {
        if(time.get_time() < 2.5)
            return;
        time.start_timer();

        add(make_shared<Object2D>());
        get(0)->add(getEngine()->get<Texture>(0)->buildSprite("green_pipe_bellow"));
        get<Sprite>({0,0})->scaleX(100);
        get<Sprite>({0,0})->offset = {-50, 75};
        get(0)->add(getEngine()->get<Texture>(0)->buildSprite("green_pipe_above"));
        get<Sprite>({0,1})->scaleX(100);
        get<Sprite>({0,1})->offset = {-50, -75 - get<Sprite>({0,1})->size().y};
        get<Object2D>(0)->offset = {500, 200 + (float)(rand() % 200)};
        get(0)->attachLoopBehaviour([](Object *self, double delta){
            shared_ptr<Object2D> t_self = static_pointer_cast<GravityObject>(self->shared_from_this());
            t_self->offset.x -= 100 * delta;
            if(t_self->offset.x < -100)
                ; // remove self from scene
        });
        getEngine()->add(removeChild(0));
    }
};


int main()
{
    Engine::enable();
    // TODO: deepcpy for object cloning, analog to packed scenes in godot
    
    auto e = make_shared<Engine>(Vect2i(400, 720));
    //e->gsys->camera_zoom = 0.5;
    //e->gsys->camera_pos = {-200, -360};
    

    // sprites, sizes gotten with brute force guessing
    e->add(e->gsys->loadTexture("./resources/flappy_sprite_sheet.png"));
    e->get<Texture>(0)->defineSprite({148 * 0, 0, 144, 256}, "background");
    e->get<Texture>(0)->defineSprite({148 * 2, 0, 160, 56}, "floor");
    e->get<Texture>(0)->defineSprite({0, 512 - 28, 28, 28}, "bird");
    e->get<Texture>(0)->defineSprite({28 * 2, 512 - 190, 28 * 1, 162}, "green_pipe_above");
    e->get<Texture>(0)->defineSprite({28 * 3, 512 - 190, 28 * 1, 162}, "green_pipe_bellow");
    
    // backround
    e->add(e->get<Texture>(0)->buildSprite("background"));
    e->get<Sprite>(1)->scaleY(720);
    e->get<Sprite>(1)->setDrawHeight(-2);
    e->add(e->get<Texture>(0)->buildSprite("floor"));
    e->get<Sprite>(2)->scaleX(420);
    e->get<Sprite>(2)->offset = {0, 580};
    e->get<Sprite>(2)->setDrawHeight(1);

    // bird
    auto object = make_shared<GravityObject>();
    object->attachInitBehaviour([](Object *self){
        static_cast<GravityObject*>(self)->offset = {100, 300};
        auto sprite = self->getEngine()->get<Texture>(0)->buildSprite("bird");
        sprite->scaleX(84);
        sprite->setDrawHeight(2);
        self->add(sprite);
        self->add(make_shared<AudioPlayer>("resources/sfx_jump.mp3"));
        self->add(make_shared<BirdHandler>());
    });

    object->attachLoopBehaviour([](Object *self, double delta){
        shared_ptr<GravityObject> t_self = static_pointer_cast<GravityObject>(self->shared_from_this());
        if(t_self->offset.y > 520)
            t_self->offset.y = 520;
    });

    e->add(object);

    // pipes
    e->add(make_shared<PipeSpawner>());
    e->start();

    Engine::disable();
    return 0;
}
