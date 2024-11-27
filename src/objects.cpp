#include <objects.hpp>

void Object::init()
{
    if (init_behavior)
        init_behavior(this);
}

void Object::loop()
{
    if (loop_behavior)
        loop_behavior(this);
}

void Object::addChild(shared_ptr<Object> child)
{
    if (std::find(children.begin(), children.end(), child) != children.end())
        ;
    children.push_back(child);
}

shared_ptr<Object> Object::getChild(int index)
{
    if (children.size() <= index)
        throw 42;
    auto it = children.begin();
    std::advance(it, index);
    return *it;
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
void Object::attachLoopBehaviour(std::function<void(Object *)> behavior)
{
    this->loop_behavior = behavior;
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
    SDL_Rect r = {global.x, global.y, size.x, size.y};
    SDL_RenderFillRect(render_view, &r);
}

void TextureObject::setTexture(SDL_Renderer *render, string filepath)
{
    texture = IMG_LoadTexture(render, filepath.c_str());
    if (!texture)
        std::cout << IMG_GetError() << '\n';
    SDL_QueryTexture(texture, NULL, NULL, &base_size.x, &base_size.y);
    size = base_size;
}

/**
 * @brief Scale size to have a width of 'x'
 */
void TextureObject::scaleX(int x)
{
    size = base_size / base_size.x * x;
}

/**
 * @brief Scale size to have a height of 'y'
 */
void TextureObject::scaleY(int y)
{
    size = base_size / base_size.y * y;
}

void TextureObject::draw()
{
    int w, h;
    SDL_Rect dest = {global.x, global.y, size.x, size.y};
    if (SDL_RenderCopy(render_view, texture, NULL, &dest))
        std::cout << SDL_GetError() << '\n';
}

GraphicSystem::GraphicSystem()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }
    window = SDL_CreateWindow("Window Name", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 840, SDL_WINDOW_SHOWN);
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // SDL_SetRenderDrawColor(render, RGB_WHITE, 255);
    // SDL_RenderClear(render);
    // SDL_RenderPresent(render);
}

shared_ptr<TextureObject> GraphicSystem::loadTexture(string filepath)
{
    shared_ptr<TextureObject> texture = make_shared<TextureObject>();
    texture->setTexture(render, filepath);
    return texture;
}

void GraphicSystem::addObj(shared_ptr<GraphicObject> obj)
{
    obj->render_view = render;
    bucket.insert(obj);
}

void GraphicSystem::removeObj(shared_ptr<GraphicObject> obj)
{
    bucket.erase(obj);
}

void GraphicSystem::update()
{
    SDL_SetRenderDrawColor(render, RGB_WHITE, 255);
    SDL_RenderClear(render);
    for (auto iter = bucket.begin(); iter != bucket.end(); iter++)
    {
        GraphicObject &obj = *iter->get();
        obj.draw();
    }
    SDL_RenderPresent(render);
    std::cout << "Frame end" << "\n";
}
