// Class B file
/**
 * @file base_event.hpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-19
 * @copyright Copyright A. Iliev (c) 2025
 */
#pragma once
#include "std_includes.hpp"

// defined here
class Event;

/**
 * @brief Base object for all events.
 */
class Event
{
public:
    /// Dummy virtual function causes the compiler to create a vtable, enabling RTTI for dynamic downcasting.
    virtual void __enable_RTTI() final 
    {
    }
};
