#include <box2d/box2d.h>

#include "objects.hpp"

const float pixels_per_meter = 1024;
/**
 * @brief Base class for physics-supporting objects
 */

// TL;DR-
// the phy updates
// 
// 
//
class PhysicsObject : public Object2D
{
public:
    b2BodyDef def;
    b2Body *body;
    PhysicsObject(b2World& world, Vect2f pos, Vect2f size, b2BodyType type) : Object2D(pos, size)
    {
        def.position.Set(pos.x / pixels_per_meter, pos.y / pixels_per_meter);
        def.type = type; // 3 types - imovable, movable but immense, and able to move and be moved
        body = world.CreateBody(&def);
        b2PolygonShape box;
        box.SetAsBox(size.x / 2 / pixels_per_meter, size.y / 2 / pixels_per_meter);
        body->CreateFixture(&box, 1.0f);
    }
};

/**
 * @brief Wrapper for box2d world. Physics simmulation 
 */
class World
{
public:
    b2World world;
     // box2d works with meters, as such the display needs to be ~1m in size for the physics to work well.
    set<shared_ptr<PhysicsObject>> objects;
    World(Vect2f gravity) : world({gravity.x, gravity.y})
    {
    }

    void update()
    {
        for(auto object : objects)
        {
            std::cout<<"Start: " << object->getPosition().x << ", "<<object->getPosition().y << "\n";
            Vect2f game_pos = object->getPosition() / pixels_per_meter;
            object->body->SetTransform({game_pos.x, game_pos.y}, 0);
            std::cout<<"phy Start: " << object->body->GetTransform().p.x << ", "<<object->body->GetTransform().p.y << "\n";
            world.Step(1.0f/60, 6, 2);
            std::cout<<"phy End: " << object->body->GetTransform().p.x << ", "<<object->body->GetTransform().p.y << "\n";
            b2Transform world_pos = object->body->GetTransform();
            object->setPosition({world_pos.p.x * pixels_per_meter, world_pos.p.y * pixels_per_meter});
            std::cout<<"End: " << object->getPosition().x << ", "<<object->getPosition().y << "\n";
        }
    }
};


/***
        b2Vec2 gravity(0.0f, -10.0f);
    
    // Create the Box2D world with the specified gravity
    b2World world(gravity);
        //static 
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(0.0f, 0.0f); // Position at origin
        groundBodyDef.type = b2_staticBody;

        // Create the body in the world
        b2Body* groundBody = world.CreateBody(&groundBodyDef);

        // Define a box shape for the static body
        b2PolygonShape groundBox;
        groundBox.SetAsBox(0.5f, 0.5f); // Half-dimensions (1x1 meter total)

        // Attach the shape to the static body
        groundBody->CreateFixture(&groundBox, 0.0f);
                // Define the dynamic body
        b2BodyDef dynamicBodyDef;
        dynamicBodyDef.position.Set(0.0f, 10.0f); // Start at height of 10 meters
        dynamicBodyDef.type = b2_dynamicBody;

        // Create the body in the world
        b2Body* dynamicBody = world.CreateBody(&dynamicBodyDef);

        // Define a box shape for the dynamic body
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(0.5f, 0.5f); // Half-dimensions (1x1 meter total)

        // Define the fixture properties for the dynamic body
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f; // Mass density (kg/m²)
        fixtureDef.friction = 0.3f; // Coefficient of friction

        // Attach the shape to the dynamic body
        dynamicBody->CreateFixture(&fixtureDef);
        // Simulate the world for a few steps
    float timeStep = 1.0f / 60.0f; // 60 Hz simulation step
    int32 velocityIterations = 6;  // Solve velocity constraints
    int32 positionIterations = 2; // Solve position constraints

    for (int32 step = 0; step < 60; ++step) {
        world.Step(timeStep, velocityIterations, positionIterations);

        // Get the position of the dynamic body
        b2Body* body = world.GetBodyList()->GetNext(); // Assuming the first is ground
        const b2Vec2 position = body->GetPosition();
        const float angle = body->GetAngle();

        std::cout << "Step " << step << ": ";
        std::cout << "Position = (" << position.x << ", " << position.y << "), ";
        std::cout << "Angle = " << angle << "\n";
    }*/