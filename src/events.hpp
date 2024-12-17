#pragma once

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


/**
 * @brief Base object for all events
 * 
 */
class Event
{
public:
    int qos = 0; // qos 0 - drop if no listen; qos 1 - transmit; qos 2 - handle immediate; unused
    virtual void __enable_RTTI() final // creates a vtable, thus enabling RTTI 
    {}
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
};

/**
 * @brief Base object for handlers. Template parameter is the accepted event type.
 * 
 */
template<typename EventType>
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
