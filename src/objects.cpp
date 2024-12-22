/**
 * @file objects.cpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <objects.hpp>

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

void Object::addChild(shared_ptr<Object> child)
{
    if (std::find(children.begin(), children.end(), child) != children.end())
        return;
    children.push_back(child);
    child->parent_view = weak_ptr<Object>(shared_from_this());
}

shared_ptr<Object> Object::getChild(int index)
{
    if (children.size() <= index)
        throw std::out_of_range("Index out of range");
    auto iter = children.begin();
    std::advance(iter, index);
    return *iter;
}

shared_ptr<Object> Object::getChild(const std::vector<int> &indices)
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

/**
 * @brief A callable which is called in the object's loop
 *
 * @param behaviour
 */
void Object::attachInitBehaviour(function<void(Object *)> behavior)
{
    this->init_behavior = behavior;
}

/**
 * @brief A callable which is called in the object's init
 *
 * @param behaviour
 */
void Object::attachLoopBehaviour(function<void(Object *, double)> behavior)
{
    this->loop_behavior = behavior;
}

const Vect2f Object2D::global() // actual position, result of parent.global + offset
{
    auto parent = dynamic_pointer_cast<Object2D>(parent_view.lock());
    if (parent.get() == nullptr)
        return offset;
    else
        return parent->global() + offset;
}

class GraphicSystem;

/**
 * @brief Set the draw Height of the object. When objects are occupying the same space,
 * the object with the largest height will be drawn above the rest.
 *
 * @param height
 */
void GraphicObject::setDrawHeight(const int &height)
{
    if (gsys_view == nullptr)
        this->height = height;
    else
    {
        // while not nescessary per se, we set the height only after the object is out of the set
        // we technically dont have ownership until the object is outside the queue
        // also dynamic cast can never fail assumin shared_from_this() doesnt
        gsys_view->removeObj(dynamic_pointer_cast<GraphicObject>(shared_from_this()));
        this->height = height;
        gsys_view->addObj(dynamic_pointer_cast<GraphicObject>(shared_from_this()));
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

void GraphicObject::draw()
{
    setDrawColor(render_view, color);
    SDL_Rect r = {(int)global().x, (int)global().y, (int)size.x, (int)size.y};
    SDL_RenderFillRect(render_view, &r);
}

void TextureObject::setTexture(SDL_Renderer *render, string filepath)
{
    texture = IMG_LoadTexture(render, filepath.c_str());
    if (texture == NULL)
    {
        std::cout << "Failed to load texture: " << IMG_GetError() << '\n';
        throw 42;
    }
    int x, y;
    SDL_QueryTexture(texture, NULL, NULL, &x, &y);
    base_size = Vect2f(x, y); // implicit type conversion int -> float
    size = base_size;
}

/**
 * @brief Scale size to have a width of 'x'
 */
void TextureObject::scaleX(int x)
{
    size = (base_size * x) / base_size.x;
}

/**
 * @brief Scale size to have a height of 'y'
 */
void TextureObject::scaleY(int y)
{
    size = (base_size * y) / base_size.y;
}

void TextureObject::draw()
{
    SDL_Rect dest = {(int)global().x, (int)global().y, (int)size.x, (int)size.y};
    //if (SDL_RenderCopyEx(render_view, texture, NULL, &dest, 0, NULL, SDL_FLIP_NONE))
    if (SDL_RenderCopy(render_view, texture, NULL, &dest))
    // TODO: copy ex supports hardware acceld rotation in the last 3 params
    // which we currently do not support
        std::cout << SDL_GetError() << '\n';
}

void EngineController::init()
{
    timeout.start_timer();
}