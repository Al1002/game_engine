#pragma once

#include <std_includes.hpp>

// defined here
class Event;
class KeyboardEvent;
class HandlerI;
template <typename EventType, typename OwnerType>
class Handler;

// extern
#include "objects.hpp"

/**
 * @brief Base object for all events
 *
 */
class Event
{
public:
    virtual void __enable_RTTI() final // creates a vtable, thus enabling RTTI
    {
    }
};

class KeyboardEvent : public Event
{
public:
    bool is_down;
    SDL_KeyboardEvent sdl_event;
    KeyboardEvent(SDL_Event e)
    {
        if(e.type == SDL_KEYDOWN)
            is_down = true;
        else if(e.type == SDL_KEYUP)
            is_down = false;
        else
            throw std::runtime_error("KeyboardEvent instance initialized with non-keyboard SDL_Event");
        this->sdl_event = e.key;
    }
};

class MouseEvent : public Event
{
public:
    bool is_down = true;
    SDL_MouseButtonEvent sdl_event;
    MouseEvent(SDL_Event e)
    {
        if(e.type == SDL_MOUSEBUTTONDOWN)
            is_down = true;
        else if(e.type == SDL_MOUSEBUTTONUP)
            is_down = false;
        else
            throw std::runtime_error("MouseEvent instance initialized with non-mouse SDL_Event");
        this->sdl_event = e.button;
    }
};

/**
 * @brief Base handler interface to enable templating
 *
 */
class HandlerI
{
public:
    size_t event_type;
    virtual void operator()(shared_ptr<Event> e) = 0;
    virtual void setOwner(weak_ptr<Object> obj) = 0;
    virtual void clearOwner() = 0;
};

/**
 * @brief Base object for handlers. EventType is the accepted event type.
 *
 */
template <typename EventType, typename OwnerType>
class Handler : public HandlerI
{
    friend void Object::attachHandler(shared_ptr<HandlerI> handle);
    virtual void setOwner(weak_ptr<Object> obj) final
    {
        owner_view = dynamic_pointer_cast<OwnerType>(obj.lock());
        if(!owner_view.lock())
            throw std::runtime_error("Attempt to assign handler to incorrect owner type");
    }
    virtual void clearOwner() final
    {
        owner_view.reset();
    }
    weak_ptr<OwnerType> owner_view;
protected:
    shared_ptr<OwnerType> getOwner()
    {
        //if(!owner_view.lock())
        //    throw std::runtime_error("Owner view is invalid");
        return owner_view.lock();
    }
public:
    Handler()
    {
        event_type = typeid(EventType).hash_code();
    }
    virtual void operator()(shared_ptr<Event> e) final
    {
        if(owner_view.lock())
            handle(static_pointer_cast<EventType>(e));
    }

    virtual void handle(shared_ptr<EventType> e) = 0;
};