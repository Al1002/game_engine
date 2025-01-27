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
    sprites.emplace(name, sprite);
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

Sprite::Sprite(shared_ptr<Texture> texture, Vect4i src_region, Vect2f offset, Vect2f size, string desiredName) 
    : GraphicObject(offset, size, desiredName)
{
    this->texture = texture;
    this->src_region = {src_region.x(), src_region.y(), src_region.z(), src_region.w()};
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
    auto pos = gsys_view->screenTransform({(int)getPosition().x, (int)getPosition().y});
    Vect2i size = {(int)getSize().x * gsys_view->camera_zoom, (int)getSize().y * gsys_view->camera_zoom};
    SDL_Rect dest = {
        pos.x - size.x / 2, pos.y - size.y / 2,
        size.x, size.y};
    if (SDL_RenderCopyEx(render_view, texture->getTexture(), &src_region, &dest, 0, NULL, SDL_FLIP_NONE))
        // TODO: copy ex supports hardware acceld rotation in the last 3 params
        // which wwe currently do not support
        std::cout << SDL_GetError() << '\n';
}

void EngineController::init()
{
    timeout.start_timer();
}
