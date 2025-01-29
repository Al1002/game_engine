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
#include <events.hpp>
#include <dispatcher.hpp>

BlueprintFactory::BlueprintFactory(string desiredName) : Object(desiredName)
{
};

void BlueprintFactory::addBlueprint(shared_ptr<Object> blueprint, string name)
{
    if(!blueprints.emplace(name, blueprint).second)
    {
        throw std::runtime_error("Blueprint with name \'" + name + "\' already exists.");
    }
}

void BlueprintFactory::removeBlueprint(string name)
{
    blueprints.erase(name);
}

shared_ptr<Object> BlueprintFactory::build(string name)
{
    return blueprints.at(name)->clone();
}

const Vect2f Object2D::getPosition()
{
    auto parent = dynamic_pointer_cast<Object2D>(parent_view.lock());
    if (parent.get() == nullptr)
        return offset;
    else
    {
        Vect2f parent_orientation = parent->getOrientation();
        return parent->getPosition() +
            Vect2f(
                offset.x * parent_orientation.x - offset.y * parent_orientation.y,
                offset.x * parent_orientation.y + offset.y * parent_orientation.x); // standard rotation matrix
    }
}

const Vect2f Object2D::getSize()
{
    auto parent = dynamic_pointer_cast<Object2D>(parent_view.lock());
    if (parent.get() == nullptr)
        return base_size * scale;
    else
        return base_size * (scale * parent->scale);
}

const Vect2f Object2D::getOrientation()
{
    auto parent = dynamic_pointer_cast<Object2D>(parent_view.lock());
    if (parent.get() == nullptr)
        return rotation;
    else
    {
        Vect2f parent_orientation = parent->getOrientation();
        return Vect2f(
                rotation.x * parent_orientation.x - rotation.y * parent_orientation.y,
                rotation.x * parent_orientation.y + rotation.y * parent_orientation.x); // standard rotation matrix
    }
}

Object2D::Object2D(string desiredName) : Object(desiredName)
{}

Object2D::Object2D(Vect2f offset, Vect2f base_size, string desiredName) : Object(desiredName)
{
    this->offset = offset;
    this->base_size = base_size;
    this->scale = 1;
}

Object2D::Object2D(Vect4f rect, string desiredName) : Object(desiredName)
{
    this->offset = {rect.x(), rect.y()};
    this->base_size = {rect.z(), rect.w()};
    this->scale = 1;
}

void Object2D::scaleX(int x)
{
    scale = x / base_size.x;
}

void Object2D::scaleY(int y)
{
    scale = y / base_size.y;
}

GraphicObject::GraphicObject()
{}

GraphicObject::GraphicObject(Vect2f offset, Vect2f base_size, string desiredName)
    : Object2D(offset, base_size, desiredName)
{}

void GraphicObject::setDrawHeight(int z)
{
    if (gsys_view == nullptr)
        this->z = z;
    else
    {
        // while not nescessary per se, we set the z only after the object is out of the set
        // we technically dont have ownership until the object is outside the queue
        // also dynamic cast can never fail assumin shared_from_this() doesnt
        auto hold = gsys_view; // unregister deletes view
        gsys_view->unregisterObj(dynamic_pointer_cast<GraphicObject>(shared_from_this()));
        this->z = z;
        hold->registerObj(dynamic_pointer_cast<GraphicObject>(shared_from_this()));
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

Texture::Texture(string desiredName) : Object(desiredName)
{}

Vect2i Texture::getSize() const
{
    return size;
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

/**
void Texture::defineSprite(Vect2i origin, Vect2i size, string name, float scale)
{
    Sprite sprite(static_pointer_cast<Texture>(shared_from_this()), origin, size);
    sprite.scale = scale;
    sprites.emplace(name, Sprite(sprite));
}

void Texture::defineAnimatedSprite(int frames, int frame_duration, Vect2i origin, Vect2i size, string name, float scale)
{
    AnimatedSprite sprite(frames, frame_duration, static_pointer_cast<Texture>(shared_from_this()), origin, size);
    sprite.scale = scale;
    sprites.emplace(name, AnimatedSprite(sprite));
}

shared_ptr<Sprite> Texture::buildSprite(string name)
{
    // Sprite &sprite = sprites.at(name);
    return make_shared<Sprite>(*sprites.at(name)); // throws an exception without explaining what the issue is
}
*/

Texture::~Texture()
{
    SDL_DestroyTexture(texture);
}

Sprite::Sprite(shared_ptr<Texture> texture, Vect2i origin, Vect2i base_size, string desiredName) 
    : GraphicObject(Vect2f(0, 0), Vect2f(base_size.x, base_size.y), desiredName)
{
    this->texture = texture;
    this->src_region = {origin.x, origin.y, base_size.x, base_size.y};
}

shared_ptr<Object> Sprite::clone() const
{
    auto c = make_shared<Sprite>(texture, Vect2i(src_region.x, src_region.y), Vect2i(src_region.w, src_region.h), desiredName);
    c->offset = offset;
    c->scale = scale;
    c->rotation = rotation;
    for(auto child : children)
    {
        c->add(child->clone());
    }
    return c;
}

void Sprite::draw()
{
    auto pos = gsys_view->screenTransform({(int)getPosition().x, (int)getPosition().y});
    Vect2i size = Vect2i(getSize().x * gsys_view->camera_zoom, getSize().y * gsys_view->camera_zoom);
    SDL_Rect dest = {
        pos.x - size.x / 2, pos.y - size.y / 2,
        size.x, size.y};
    if (SDL_RenderCopyEx(render_view, texture->getTexture(), &src_region, &dest, 0, NULL, SDL_FLIP_NONE))
        // TODO: copy ex supports hardware acceld rotation in the last 3 params
        // which wwe currently do not support
        std::cout << SDL_GetError() << '\n';
}

AnimatedSprite::AnimatedSprite(int frames, int ticks_per_frame, shared_ptr<Texture> texture, Vect2i origin, Vect2i size, string desiredName)
    : Sprite(texture, origin, size, desiredName)
{
    this->start_origin = origin;
    this->frames = frames;
    this->ticks_per_frame = ticks_per_frame;
}

shared_ptr<Object> AnimatedSprite::clone() const
{
    auto c = make_shared<AnimatedSprite>(frames, ticks_per_frame, texture, Vect2i(src_region.x, src_region.y), Vect2i(src_region.w, src_region.h), desiredName);
    c->offset = offset;
    c->scale = scale;
    c->rotation = rotation;
    for(auto child : children)
    {
        c->add(child->clone());
    }
    return c;
}

void AnimatedSprite::draw()
{
    current_tick++;
    if(current_tick >= ticks_per_frame)
    {
        current_tick = 0;
        current_frame++;
        src_region.x += src_region.w;
        if(src_region.x > texture->getSize().x)
        {
            src_region.x -= texture->getSize().x;
            src_region.y += src_region.h;
        }
    }
    if(current_frame >= frames)
    {
        current_frame = 0;
        src_region.x = start_origin.x;
        src_region.y = start_origin.y;
    }
    Sprite::draw();
}

void EngineController::init()
{
    timeout.start_timer();
}
