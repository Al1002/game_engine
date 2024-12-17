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

using std::move;
using std::make_shared;
using std::make_unique;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::shared_ptr;
using std::weak_ptr;


class MyHandler : public Handler<KeyboardEvent>
{
public:
    shared_ptr<GraphicObject> that;
    MyHandler(shared_ptr<GraphicObject> t)
    {
        that = t;
    }
    void handle(shared_ptr<KeyboardEvent> e) override
    {
        if(e->sdl_event.keysym.sym == 'a')
            that->global.x-=100;
        if(e->sdl_event.keysym.sym == 'd')
            that->global.x+=100;
        if(e->sdl_event.keysym.sym == 'w')
            that->global.y-=100;
        if(e->sdl_event.keysym.sym == 's')
            that->global.y+=100;
    
    }
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
        self->addChild(sprite);
        sprite->global = {200, 200};
        sprite->size = {50, 400};
        self->getEngine()->disp->addEventHandler(make_shared<MyHandler>(self->getChild<GraphicObject>(0)));
    });

    object->attachLoopBehaviour( [](Object* self) {
        auto sprite = self->getChild<GraphicObject>(0);
        sprite->global.x++;
    });
    
    
    e->addObj(object);

    e->start();
    SDL_Quit();
    return 0;
}
