#pragma once

#include <std_includes.hpp>

// declared here
class Event;
class KeyboardEvent;
class HandlerI;
template <typename EventType>
class Handler;
template <typename EventType, typename ParentType>
class ParentHandler;

// extern
#include <objects.hpp>

/**
 * @brief Base object for all events
 *
 */
class Event
{
public:
    int qos = 0;                       // qos 0 - drop if no listen; qos 1 - transmit; qos 2 - handle immediate; unused
    virtual void __enable_RTTI() final // creates a vtable, thus enabling RTTI
    {
    }
};

class KeyboardEvent : public Event
{
public:
    bool is_down = true;
    SDL_KeyboardEvent sdl_event;
    KeyboardEvent(SDL_Event e)
    {
        this->sdl_event = e.key;
    }
};

/**
 * @brief Base handler interface to enable templating
 *
 */
class HandlerI : public Object
{
public:
    size_t event_type;
    virtual void operator()(shared_ptr<Event> e) = 0;
};

/**
 * @brief Base object for handlers. EventType is the accepted event type.
 *
 */
template <typename EventType>
class Handler : public HandlerI
{
public:
    Handler()
    {
        event_type = typeid(EventType).hash_code();
    }

    void operator()(shared_ptr<Event> e) final
    {
        handle(static_pointer_cast<EventType>(e));
    }

    virtual void handle(shared_ptr<EventType> e) = 0;
};

/**
 * @brief Base object for parrent handlers - handlers which handle their parent object. EventType is the accepted event type. Parent is the parent type.
 *  If the parent is not of ParentType (std::dynamic_pointer_cast returns nullptr) or has no parent, does nothing.
 *
 */
template <typename EventType, typename ParentType>
class ParentHandler : public Handler<EventType>
{
public:
    void handle(shared_ptr<EventType> e) override final
    {
        auto parent = dynamic_pointer_cast<ParentType>(Object::parent_view.lock());
        if (!parent)
            return;
        handle(static_pointer_cast<EventType>(e), parent);
    }

    virtual void handle(shared_ptr<EventType> e, shared_ptr<ParentType> parent) = 0;
};
