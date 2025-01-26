// Class A file
/**
 * @file dispatcher.hpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "std_includes.hpp"

// defined here
class EventDispatcher;

// extern
class Event;
class HandlerI;

/**
 * @brief Class which notifies registered handlers of events.
 */
class EventDispatcher
{
    queue<shared_ptr<Event>> buffers[2]; ///< queues to implement double buffering
    queue<shared_ptr<Event>> *back;      ///< back buffer, meant to take in new events
    queue<shared_ptr<Event>> *front;     ///< front bugger, meant to have events read and handled from it

    mutex back_m; ///< write lock to atomize queue access
public:
    unordered_set<shared_ptr<HandlerI>> handles; /// < handler objects to be notified by events

    /**
     * @brief Construct a new Event Dispatcher object
     */
    EventDispatcher();

    /**
     * @brief Add new `Handler` to be notified of `Event`s. Will only be notified of events of its `Event` type.
     * @param handle
     */
    void registerEventHandler(shared_ptr<HandlerI> handle);

    /**
     * @brief Remove `Handler` from getting notified of `Event`s.
     * @param handle
     */
    void unregisterEventHandler(shared_ptr<HandlerI> handle);

    /**
     * @brief Add `Event` to be sent to `Handler`s. Handlers will only recieve events when `dispatch()` is called.
     * @param e
     */
    void addEvent(shared_ptr<Event> e);

    /**
     * @brief Function to dispatch
     */
    void dispatch();
};
