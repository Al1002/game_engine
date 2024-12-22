#include <graphic_system.hpp>

class GraphicObject; 

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
    obj->gsys_view = this;
    bucket.emplace(obj->height, obj);
}

void GraphicSystem::removeObj(shared_ptr<GraphicObject> obj)
{
    bucket.erase({obj->height, obj});
}

void GraphicSystem::update()
{
    SDL_SetRenderDrawColor(render, RGB_WHITE, 255);
    SDL_RenderClear(render);
    for (auto iter = bucket.begin(); iter != bucket.end(); iter++)
    {
        GraphicObject &obj = *iter->second.get();
        //workers->enqueue(std::bind(&GraphicObject::draw, iter->second.get()));
        obj.draw();
    }
    SDL_RenderPresent(render);
    std::cout << "Frame end" << "\n";
}


