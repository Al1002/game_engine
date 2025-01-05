/**
 * @file objects.cpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 * @copyright Copyright (c) 2024
 */

#include <objects.hpp>

#include <engine.hpp>
#include <graphic_system.hpp>

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
    return engine_view.lock();
}

shared_ptr<Object> Object::getParent()
{
    return parent_view.lock();
}

string Object::desiredName()
{
    return "Object";
}

string Object::getName()
{
    return name;
}

void Object::addChild(shared_ptr<Object> child)
{
    if (std::find(children.begin(), children.end(), child) != children.end())
        return; // child already exists
    // give child name and insert
    string name = child->desiredName();
    string unique_name = name;
    map<string, shared_ptr<Object>>::iterator result;
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
#if 0
shared_ptr<Object> Object::getChild(vector<int> indices)
{
    if (indices.empty())
        throw std::invalid_argument("Indices list is empty");
    int index = indices.front(); // front of the list

    if (children.size() <= index)
        throw std::out_of_range("Index out of range");
    auto iter = children.begin();
    std::advance(iter, index);

    if (indices.size() == 1)
        return *iter;

    std::vector<int> remainder(indices.begin() + 1, indices.end());
    return (*iter)->getChild(remainder);
}
#endif
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
    child->parent_view.reset();
    child->getEngine()->unregisterObj(child);
    child->name = "";
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

const Vect2f Object2D::position()
{
    auto parent = dynamic_pointer_cast<Object2D>(parent_view.lock());
    if (parent.get() == nullptr)
        return offset;
    else
        return parent->position() + offset;
}

const Vect2f Object2D::size()
{
    auto parent = dynamic_pointer_cast<Object2D>(parent_view.lock());
    if (parent.get() == nullptr)
        return base_size * scale;
    else
        return base_size * (scale * parent->scale);
}

Object2D::Object2D()
{
}

string Object2D::desiredName()
{
    return "Object2D";
}

Object2D::Object2D(Vect2f offset, Vect2f base_size)
{
    this->offset = offset;
    this->base_size = base_size;
    this->scale = 1;
}

GraphicObject::GraphicObject()
{
}

string GraphicObject::desiredName()
{
    return "GraphicObject";
}

GraphicObject::GraphicObject(Vect2f offset, Vect2f base_size)
    : Object2D(offset, base_size)
{
}

void GraphicObject::setDrawHeight(int height)
{
    if (gsys_view == nullptr)
        this->height = height;
    else
    {
        // while not nescessary per se, we set the height only after the object is out of the set
        // we technically dont have ownership until the object is outside the queue
        // also dynamic cast can never fail assumin shared_from_this() doesnt
        gsys_view->unregisterObj(dynamic_pointer_cast<GraphicObject>(shared_from_this()));
        this->height = height;
        gsys_view->registerObj(dynamic_pointer_cast<GraphicObject>(shared_from_this()));
    }
}

void GraphicObject::setDrawColor(SDL_Renderer *render, Color c)
{
    switch (c)
    {
    case RED:
        SDL_SetRenderDrawColor(render, RGB_RED, 255);
        break;
    case GREEN:
        SDL_SetRenderDrawColor(render, RGB_GREEN, 255);
        break;
    case BLUE:
        SDL_SetRenderDrawColor(render, RGB_BLUE, 255);
        break;
    default:
        SDL_SetRenderDrawColor(render, RGB_MAGENTA, 255);
    }
}

string Texture::desiredName()
{
    return "Texture";
}

void Texture::setTexture(SDL_Renderer *render, string filepath)
{
    texture = IMG_LoadTexture(render, filepath.c_str());
    if (texture == NULL)
        throw std::runtime_error(string() + "Failed to load texture: " + IMG_GetError());
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
}

SDL_Texture *Texture::getTexture()
{
    return texture;
}

void Texture::defineSprite(Vect4i src_region, string name)
{
    Sprite sprite(static_pointer_cast<Texture>(shared_from_this()), src_region, Vect2f(0, 0), Vect2f(src_region.z(), src_region.w()));
    sprites[name] = sprite;
}

shared_ptr<Sprite> Texture::buildSprite(string name)
{
    // Sprite &sprite = sprites.at(name);
    return make_shared<Sprite>(sprites.at(name)); // throws an exception without explaining what the issue is
}

Texture::~Texture()
{
    SDL_DestroyTexture(texture);
}

Sprite::Sprite()
{
}

Sprite::Sprite(shared_ptr<Texture> texture, Vect4i src_region, Vect2f offset, Vect2f size) : GraphicObject(offset, size)
{
    this->texture = texture;
    this->src_region = {src_region.x(), src_region.y(), src_region.z(), src_region.w()};
}

string Sprite::desiredName()
{
    return "Sprite";
}

void Sprite::scaleX(int x)
{
    scale = x / base_size.x;
}

void Sprite::scaleY(int y)
{
    scale = y / base_size.y;
}

void Sprite::draw()
{
    auto pos = gsys_view->screenTransform({(int)position().x, (int)position().y});
    SDL_Rect dest = {
        pos.x, pos.y,
        (int)(size().x * gsys_view->camera_zoom), (int)(size().y * gsys_view->camera_zoom)};
    if (SDL_RenderCopyEx(render_view, texture->getTexture(), &src_region, &dest, 0, NULL, SDL_FLIP_NONE))
        // TODO: copy ex supports hardware acceld rotation in the last 3 params
        // which wwe currently do not support
        std::cout << SDL_GetError() << '\n';
}

string AudioPlayer::desiredName()
{
    return "AudioPlayer";
}

void EngineController::init()
{
    timeout.start_timer();
}
