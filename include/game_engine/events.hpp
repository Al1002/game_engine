// Class F file

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
 * @brief Base object for all events.
 */
class Event
{
public:
    virtual void __enable_RTTI() final ///< Dummy virtual function causes the compiler to create a vtable, enabling RTTI for dynamic downcasting.
    {
    }
};

/**
 * @brief Wrapper for SDL_KeyboardEvent. Created by keypresses.
 */
class KeyboardEvent : public Event
{
public:
    bool is_down;
    SDL_KeyboardEvent sdl_event;
    KeyboardEvent(SDL_Event e);
};

/**
 * @brief Wrapper for SDL_MouseButtonEvent. Created by mouse clicks.
 */
class MouseButtonEvent : public Event
{
public:
    bool is_down = true;
    SDL_MouseButtonEvent sdl_event;
    MouseButtonEvent(SDL_Event e);
};

/**
 * @brief Base handler interface to enable templating
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
 * @brief Base template for handlers. EventType is the accepted event type.
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