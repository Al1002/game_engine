#include <box2d/box2d.h>
class ContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override {
        std::cout << "foobar" << std::endl;
    }
};

// Initialize Box2D world with gravity
    b2Vec2 gravity(0.0f, -10.0f); // Gravity pulling downwards
    b2World world(gravity);

    // Create a contact listener to detect collisions
    ContactListener contactListener;
    world.SetContactListener(&contactListener);

    // Create a static ground box (100x100)
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f); // Position at the origin
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    
    b2PolygonShape groundShape;
    groundShape.SetAsBox(50.0f, 1.0f); // A long ground shape (half width of 50, half height of 1)
    groundBody->CreateFixture(&groundShape, 0.0f); // Static body, no density

    // Create a dynamic box (100x100) above the ground
    b2BodyDef dynamicBodyDef;
    dynamicBodyDef.position.Set(0.0f, 10.0f); // Starting position above the ground
    dynamicBodyDef.type = b2_dynamicBody; // Make sure it's a dynamic body
    b2Body* dynamicBody = world.CreateBody(&dynamicBodyDef);

    b2PolygonShape dynamicShape;
    dynamicShape.SetAsBox(1.0f, 1.0f); // Half width and half height of 1 unit (100x100 in Box2D scale)

    // Define fixture for dynamic body
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicShape;
    fixtureDef.density = 1.0f; // Density affects gravity
    fixtureDef.friction = 0.3f;
    dynamicBody->CreateFixture(&fixtureDef);

    // Run the simulation and print positions (50 steps)
    for (int i = 0; i < 500; ++i) {
        world.Step(1.0f / 60.0f, 6, 2); // Step the simulation (1/60th of a second)

        // Get the position of the dynamic box
        b2Vec2 dynamicPosition = dynamicBody->GetPosition();

        std::cout.precision(2);
        // Print the position
        std::cout << "Step " << i << " - Dynamic Box Position: (" 
                  << dynamicPosition.x << ", " << dynamicPosition.y << ")\n";
    }

    return 0;