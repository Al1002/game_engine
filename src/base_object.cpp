#include <base_object.hpp>
#include <events.hpp>
#include <engine.hpp>

Object::Object(string desiredName)
{
    this->desiredName = desiredName;
}

void Object::init()
{
    if (init_behavior)
        init_behavior(this);
}

void Object::loop(double delta)
{
    if (loop_behavior)
        loop_behavior(this, delta);
}

shared_ptr<Engine> Object::getEngine()
{
    //if(!engine_view.lock())
    //    throw std::runtime_error("Object not owned by engine");
    return engine_view.lock();
}

shared_ptr<Object> Object::getParent()
{
    return parent_view.lock();
}

string Object::getDesiredName()
{
    return desiredName;
}

string Object::getName()
{
    return name;
}

void Object::attachHandler(shared_ptr<HandlerI> handle)
{
    handle->setOwner(shared_from_this());
    handlers.push_back(handle);
    if(engine_view.lock())
        getEngine()->disp->registerEventHandler(handle);
}

void Object::dettachHandler(shared_ptr<HandlerI> handle)
{
    handlers.remove(handle);
    handle->clearOwner();
}

void Object::addChild(shared_ptr<Object> child)
{
    if (std::find(children.begin(), children.end(), child) != children.end())
        return; // child already exists
    // give child name and insert
    string name = child->getDesiredName();
    string unique_name = name;
    for(int i = 1; children_map.find(unique_name) != children_map.end(); i++)
    {
        // if the name is already unique, we dont enter this loop and it remains as it was originally
        unique_name = name + "_" + std::to_string(i);
    }
    children_map.emplace(unique_name, child);
    children.push_back(child);
    // insert end
    child->name = unique_name;
    child->parent_view = weak_ptr<Object>(shared_from_this());
    auto engine = getEngine();
    if (engine != nullptr)
        engine->registerObj(child);
}

shared_ptr<Object> Object::getChild(int index)
{
    if (children.size() <= index)
        throw std::out_of_range("Index out of range");
    auto iter = children.begin();
    std::advance(iter, index);
    return *iter;
}

shared_ptr<Object> Object::getChild(string path)
{
    int delim = path.find('/');
    string current = path.substr(0, delim);
    auto it = children_map.find(current);

    if (it == children_map.end())
        throw std::out_of_range("Child " + current + " not found");
    
    if (delim == string::npos)
        return it->second;
    
    return it->second->getChild(path.substr(delim + 1));
}

shared_ptr<Object> Object::removeChild(int index)
    {
        if (children.size() <= index)
            throw std::out_of_range("Index out of range");
        auto iter = children.begin();
        std::advance(iter, index);
        shared_ptr<Object> child = *iter;
        children.erase(iter);
        children_map.erase(child->name);
        child->parent_view.reset();
        child->engine_view.reset();
        child->name = "";
        return child;
    }

shared_ptr<Object> Object::removeChild(string name)
{
    shared_ptr<Object> child = children_map.at(name);
    children.erase(std::find(children.begin(), children.end(), child));
    children_map.erase(name);
    child->name = "";
    child->parent_view.reset();
    child->getEngine()->unregisterObj(child);
    return child;
}

void Object::attachInitBehaviour(function<void(Object *)> behavior)
{
    this->init_behavior = behavior;
}

void Object::attachLoopBehaviour(function<void(Object *, double)> behavior)
{
    this->loop_behavior = behavior;
}
