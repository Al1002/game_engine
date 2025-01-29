#include <physics.hpp>

PhysicsObject::PhysicsObject(Vect2f pos, Vect2f size, b2BodyType type, string desiredName) : Object2D(pos, size, desiredName)
{
    motion_check = pos;
    // the pivot in box2d is the center of an object
    def.position.Set(pos.x / pixels_per_meter, pos.y / pixels_per_meter);
    def.fixedRotation = true;
    def.type = type; // 3 types - imovable, movable but immense, and able to move and be moved
    if (type == b2_dynamicBody)
        def.allowSleep = false;
    shape.SetAsBox(size.x / 2 / pixels_per_meter, size.y / 2 / pixels_per_meter);
    fixt.shape = &shape;
    fixt.density = 1;
}

World::World(Vect2f gravity) : world({gravity.x / pixels_per_meter, gravity.y / pixels_per_meter})
{
    
    //world.SetContactListener();
}

void World::registerObj(shared_ptr<PhysicsObject> obj)
{
    obj->body = world.CreateBody(&obj->def);
    obj->body->CreateFixture(&obj->fixt);
    bucket.emplace(obj);
}

void World::unregisterObj(shared_ptr<PhysicsObject> obj)
{
    world.DestroyBody(obj->body);
    obj->body = nullptr;
    bucket.erase(obj);
}

void World::update()
{
    for(auto object : bucket)
    {
        Vect2f pos = object->getPosition();
        if(pos == object->motion_check)
            continue;
        Vect2f game_pos = object->getPosition() / pixels_per_meter;
        object->body->SetTransform({game_pos.x, game_pos.y}, 0);
    }
    world.Step(1.0f/60, 6, 2);
    for(auto object : bucket)
    {
        b2Transform world_pos = object->body->GetTransform();
        object->setPosition({world_pos.p.x * pixels_per_meter, world_pos.p.y * pixels_per_meter});
        object->motion_check = object->getPosition();
    }
}