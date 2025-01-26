#pragma once
#include <box2d/box2d.h>

#include "objects.hpp"
#include "events.hpp"

const float pixels_per_meter = 1024;

/**
 * @brief Base class for physics-supporting objects
 */
class PhysicsObject : public Object2D
{
public:
    b2BodyDef def;
    b2PolygonShape shape;
    b2FixtureDef fixt;
    b2Body *body = nullptr;
    PhysicsObject(Vect2f pos, Vect2f size, b2BodyType type, string desiredName = "PhysicsObject");
};

/**
 * @brief Wrapper for box2d world. Physics simmulation 
 */
class World
{
public:
    b2World world;
    // box2d works with meters, as such the display needs to be ~1m in size for the physics to work well.
    set<shared_ptr<PhysicsObject>> bucket;
    World(Vect2f gravity);

    void registerObj(shared_ptr<PhysicsObject> obj);

    void unregisterObj(shared_ptr<PhysicsObject> obj);
    
    void update();
};

class ContactEvent : public Event
{
    
};

/**
 * @brief Transforms box2d callbacks into Events
 */
class GameContactEventBuilder : public b2ContactListener
{
public:
    /// Called when two fixtures begin to touch.
	virtual void BeginContact(b2Contact* contact) override
    {
        contact->GetFixtureA();
        contact->GetFixtureB();
    }

	/// Called when two fixtures cease to touch.
	virtual void EndContact(b2Contact* contact) override
    {
        contact->GetFixtureA();
        contact->GetFixtureB();
    }

};