#include <graphic_system.hpp>
#include <colors.h>
#include <objects.hpp>

class GraphicObject;

GraphicSystem::GraphicSystem(Vect2i window_size)
{
    camera_pos = window_size / 2;
    this->window_size = window_size;
    window = SDL_CreateWindow("Window Name", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_size.x, window_size.y, SDL_WINDOW_SHOWN);
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

shared_ptr<Texture> GraphicSystem::loadTexture(string filepath)
{
    shared_ptr<Texture> texture = make_shared<Texture>();
    texture->setTexture(render, filepath);
    return texture;
}

void GraphicSystem::registerObj(shared_ptr<GraphicObject> obj)
{
    obj->render_view = render;
    obj->gsys_view = this;
    bucket.emplace(obj->z, obj);
}

void GraphicSystem::unregisterObj(shared_ptr<GraphicObject> obj)
{
    obj->render_view = nullptr;
    obj->gsys_view = nullptr;
    auto &buckobj = *bucket.begin();
    bucket.erase({obj->z, obj});
}

void GraphicSystem::update()
{
    SDL_SetRenderDrawColor(render, RGB_WHITE, 255);
    SDL_RenderClear(render);
    for (auto iter = bucket.begin(); iter != bucket.end(); iter++)
    {
        GraphicObject &obj = *iter->second.get();
        // workers->enqueue(std::bind(&GraphicObject::draw, iter->second.get()));
        obj.draw();
    }
    SDL_RenderPresent(render);
}
