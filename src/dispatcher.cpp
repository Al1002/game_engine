/**
 * @file dispatcher.cpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <dispatcher.hpp>

EventDispatcher::EventDispatcher()
{
    back = &buffers[0];
    front = &buffers[1];
}

void EventDispatcher::addEventHandler(shared_ptr<HandlerI> handle)
{
    handles.emplace(handle);
}

void EventDispatcher::addEvent(shared_ptr<Event> e)
{
    back_m.lock();
    back->push(e);
    back_m.unlock();
}

void EventDispatcher::dispatch()
{
    back_m.lock();
    auto temp = back;
    back = front;
    front = temp;
    back_m.unlock();

    while (!front->empty())
    {
        auto event = front->front();
        back->pop();
        for (auto &handler : handles)
        {
            auto hash = typeid(*event.get()).hash_code();
            if (handler->event_type == hash)
                (*handler)(event);
        }
    }
}
