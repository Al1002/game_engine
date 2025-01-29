#pragma once

#include "objects.hpp"

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
    void init() final override;
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

void Button::init()
{
    attachHandler(make_shared<ButtonHandler>());
}
