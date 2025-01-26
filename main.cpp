/**
 * @file main.cpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-27
 * @copyright Copyright (c) 2024
 */

#include <std_includes.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <vects.hpp> // Mathematical vectors
#include <clock.h>   // clock/timer utility

#include <box2d/box2d.h> 

#include <colors.h>  // #defined RGB_COLORs
#include <objects.hpp>
#include <events.hpp>
#include <engine.hpp>
#include <graphic_system.hpp>
#include <physics.hpp>

class Button;
class ButtonHandler;

class Button : public Object2D
{
public:
    Button(string desiredName = "Button") : Object2D(desiredName)
    {
    }
    static shared_ptr<Button> create(string desiredName = "Button");
    virtual void onClick()
    {
        std::cout<<"click\n";
    };
};
class ButtonHandler : public Handler<MouseButtonEvent, Button>
{
public:
    virtual void handle(shared_ptr<MouseButtonEvent> e) override
    {
        if(e->is_down)
            if(e->sdl_event.x > getOwner()->getPosition().x &&
                e->sdl_event.x < getOwner()->getPosition().x + getOwner()->getSize().x &&
                e->sdl_event.x > getOwner()->getPosition().y &&
                e->sdl_event.x < getOwner()->getPosition().y + getOwner()->getSize().y)
                getOwner()->onClick();
    }
};

shared_ptr<Button> Button::create(string desiredName)
{
    shared_ptr<Button> button = make_shared<Button>(desiredName);
    button->attachHandler(make_shared<ButtonHandler>());
    return button;
}


/**** 
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
*/

class BirdHandler : public Handler<KeyboardEvent, PhysicsObject>
{
public:
    void handle(shared_ptr<KeyboardEvent> e) override
    {
        if(!e->is_down)
            return;
        if (e->sdl_event.keysym.sym == 'w' || e->sdl_event.keysym.sym == ' ' || e->sdl_event.keysym.sym == SDLK_UP)
        {
            getOwner()->body->SetLinearVelocity({0, -600.0f / 1024});
            getOwner()->get<AudioPlayer>(1)->play();
        }
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
        get<Object2D>(0)->offset = {500, 500 + (float)(rand() % 200)};
        get(0)->add(getEngine()->get<Texture>("Texture")->buildSprite("green_pipe_bellow"));
        get<Sprite>("Object2D/Sprite")->scaleX(100);
        get<Sprite>("Object2D/Sprite")->offset = {0, 75};
        get(0)->add(make_shared<PhysicsObject>(get<Sprite>("Object2D/Sprite")->offset, get<Sprite>("Object2D/Sprite")->getSize(), b2_kinematicBody));
        get(0)->add(getEngine()->get<Texture>("Texture")->buildSprite("green_pipe_above"));
        get<Sprite>("Object2D/Sprite_1")->scaleX(100);
        get<Sprite>("Object2D/Sprite_1")->offset = {0, -75 - get<Sprite>("Object2D/Sprite_1")->getSize().y};
        get(0)->add(make_shared<PhysicsObject>(get<Sprite>("Object2D/Sprite_1")->offset, get<Sprite>("Object2D/Sprite_1")->getSize(), b2_kinematicBody));
        get(0)->attachLoopBehaviour([](Object *self, double delta){
            shared_ptr<Object2D> t_self = static_pointer_cast<Object2D>(self->shared_from_this());
            t_self->offset.x -= 100 * delta;
            if(t_self->offset.x < -100)
                t_self->getParent()->removeChild(t_self->getName());
        });
        getEngine()->add(removeChild(0));
    }
};


class StartGameButton : public Button
{
public:
    bool isStart = false;
    void onClick()override
    {
        if(isStart)
            return;
        else
            isStart = true; 
        getEngine()->add(make_shared<Object>("Game"));
        // bird
        auto bird = make_shared<PhysicsObject>(Vect2f(0,0), Vect2f(6,6), b2_dynamicBody);
        bird->attachInitBehaviour([](Object *self){
        static_cast<Object2D*>(self)->offset = {100, 100};
        auto sprite = self->getEngine()->get<Texture>("Texture")->buildSprite("bird");
        sprite->scaleX(84);
        sprite->setDrawHeight(2);
        self->add(sprite);
        try{
            self->add(make_shared<AudioPlayer>("resources/sfx_jump.mp3"));
        }catch(std::exception any){
            self->add(make_shared<AudioPlayer>("../exec_env/resources/sfx_jump.mp3"));
        }
    
        self->get<AudioPlayer>(1)->setVolume(25);
        self->attachHandler(make_shared<BirdHandler>());
        });
        getEngine()->get("Game")->add(bird);

        // pipes
        getEngine()->get("Game")->add(make_shared<PipeSpawner>());
    
    }
};

#include <exception>

#ifdef __WIN32__ // the mingw SDL expects WinMain
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main(int argc, char **argv)
#endif
{
    Engine::enable();
    // TODO: deepcpy for object cloning, analog to packed scenes in godot
    
    auto e = make_shared<Engine>(Vect2i(400, 720), Vect2f(0, 2000));
    // sprites, sizes gotten with brute force guessing
    try{
        e->add(e->gsys->loadTexture("./resources/flappy_sprite_sheet.png"));
    }catch(std::exception any){
        e->add(e->gsys->loadTexture("../exec_env/resources/flappy_sprite_sheet.png"));
    }
    e->get<Texture>("Texture")->defineSprite({148 * 0, 0, 144, 256}, "background");
    e->get<Texture>("Texture")->defineSprite({148 * 2, 0, 160, 56}, "floor");
    e->get<Texture>("Texture")->defineSprite({0, 512 - 28, 28, 28}, "bird");
    e->get<Texture>("Texture")->defineSprite({28 * 2, 512 - 190, 28 * 1, 162}, "green_pipe_above");
    e->get<Texture>("Texture")->defineSprite({28 * 3, 512 - 190, 28 * 1, 162}, "green_pipe_bellow");
    
    // backround
    e->add(e->get<Texture>("Texture")->buildSprite("background"));
    e->get<Sprite>("Sprite")->offset = {400/2, 720/2};
    e->get<Sprite>("Sprite")->scaleY(720);
    e->get<Sprite>("Sprite")->setDrawHeight(-2);
    
    // floor
    e->add(make_shared<PhysicsObject>(Vect2f(0, 0), Vect2f(420,168), b2_staticBody));
    e->get<PhysicsObject>("PhysicsObject")->offset = {400/2, 650};
    e->get("PhysicsObject")->add(e->get<Texture>("Texture")->buildSprite("floor"));
    e->get<Sprite>("PhysicsObject/Sprite")->scaleX(480);
    e->get<Sprite>("PhysicsObject/Sprite")->setDrawHeight(1);
    e->get<Sprite>("PhysicsObject/Sprite")->attachLoopBehaviour([](Object *self, double delta){
        shared_ptr<Object2D> t_self = static_pointer_cast<Object2D>(self->shared_from_this());
        t_self->offset.x -= 100 * delta;
        if(t_self->offset.x < -36)
            t_self->offset.x = 0;
    });
    

    e->add(make_shared<StartGameButton>());
    e->get<Button>("Button")->base_size = {400, 720};
    e->get<Button>("Button")->attachHandler(make_shared<ButtonHandler>());
    e->start();

    Engine::disable();
    return 0;
}
