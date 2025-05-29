//Class B file
/**
 * @file base_handler.hpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-19
 * @copyright Copyright A. Iliev (c) 2025
 */
#pragma once

// defined here
class HandlerI;
template <typename EventType, typename OwnerType>
class Handler;

// extern
#include "base_object.hpp"
#include "base_event.hpp"

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

    weak_ptr<OwnerType> owner_view;

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