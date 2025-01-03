/**
 * @file objects.hpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-18
 * @ingroup Objects
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <std_includes.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <vects.hpp> // Mathematical vectors
#include <clock.h>   // clock/timer utility

#include <colors.h> // #defined RGB_COLORs

// Defined here
class Object;
class Object2D;
class GraphicObject;
class Texture;
class Sprite;
class AudioPlayer;
class EngineController;

// Extern
class GraphicSystem;
class Engine;

/**
 * @brief Base class for all game objects. 
 */
class Object : public std::enable_shared_from_this<Object>
{
    friend Engine;

protected:
    weak_ptr<Object> parent_view;
    map<string, shared_ptr<Object>> children_map; ///< allows named access to children
    list<shared_ptr<Object>> children; ///< allows indexed access to children
    string name;
    function<void(Object *)> init_behavior;
    function<void(Object *, double)> loop_behavior;
public:
    weak_ptr<Engine> engine_view;

    virtual void init();

    virtual void loop(double delta);

    shared_ptr<Engine> getEngine();

    /**
     * @brief Get the object's parent. If orphan, returns an empty shared_ptr.
     * @return shared_ptr<Object> the object's parent
     */
    shared_ptr<Object> getParent();

    /**
     * @brief The name the node is given by the parrent. May be appended by an index if another child already has that name.
     * @return string 
     */
    virtual string desiredName();

    string getName();

    /**
     * @brief Add child to the object. Child is appended to the back of the child list.
     *
     * @param child child object
     */
    void addChild(shared_ptr<Object> child);

    /**
     * @brief Short alias for addChild.
     *
     * @param child child object
     */
    inline void add(shared_ptr<Object> child)
    {
        addChild(child);
    }

    /**
     * @brief Get child by index.
     *
     * @param index position of the child in the child list
     * @return shared_ptr<Object>
     * @throws out_of_range exception if the child index is out of range
     */
    shared_ptr<Object> getChild(int index);

    /**
     * @brief Get child by index, downcast to the template type.
     *
     * @param index position of the child in the child list
     * @tparam T the type to downcast the child to
     * @return shared_ptr<Object>
     * @throws std::out_of_range exception if the child index is out of range
     * @throws  exception if the type is not an ancestor of the child's actual type
     */
    template <typename T>
    inline shared_ptr<T> getChild(int index)
    {
        auto child = dynamic_pointer_cast<T>(getChild(index));
        if (child.get() == nullptr)
            throw std::runtime_error("Child not of specified class");
        return child;
    }

    /**
     * @brief Short alias for getChild.
     * @param index position of the child in the child list
     * @tparam T the type to downcast the child to
     * @return shared_ptr<Object>
     * @throws std::out_of_range exception if the child index is out of range
     * @throws  exception if the type is not an ancestor of the child's actual type
     */
    template <typename T = Object>
    inline shared_ptr<T> get(int index)
    {
        return getChild<T>(index);
    }
#if 0
    shared_ptr<Object> getChild(vector<int> indices);

    template <typename T>
    inline shared_ptr<T> getChild(vector<int> indices)
    {
        return dynamic_pointer_cast<T>(getChild(indices));
    }

    /**
     * @brief Short alias of getChild
     * @tparam T
     * @param indices
     * @return shared_ptr<T>
     */
    template <typename T = Object>
    inline shared_ptr<T> get(vector<int> indices)
    {
        return getChild<T>(indices);
    }
#endif
    /**
     * @brief Get child by name.
     * @param path 
     * @return shared_ptr<Object>
     * @throws std::out_of_range 
     */
    shared_ptr<Object> getChild(string path);

    /**
     * @brief Get child by name.
     * @param path 
     * @return shared_ptr<Object>
     * @throws std::out_of_range 
     */
    template <typename T>
    inline shared_ptr<T> getChild(string path)
    {
        return dynamic_pointer_cast<T>(getChild(path));
    }

    /**
     * @brief Short alias of getChild
     * @tparam T
     * @param indices
     * @return shared_ptr<T>
     */
    template <typename T = Object>
    inline shared_ptr<T> get(string path)
    {
        return getChild<T>(path);
    }

    /**
     * @brief Remove child by index.
     * @param index position of the child in the child list
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if the child index is out of range
     */
    shared_ptr<Object> removeChild(int index);

    /**
     * @brief Remove child by name.
     * @param name name of the child to be removed
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if no child has that name
     */
    shared_ptr<Object> removeChild(string name);
    
    /**
     * @brief A callable which is called in the object's loop
     *
     * @param behaviour
     */
    void attachInitBehaviour(function<void(Object *)> behavior);

    /**
     * @brief A callable which is called in the object's init
     *
     * @param behaviour
     */
    void attachLoopBehaviour(function<void(Object *, double)> behavior);

    // avoid attachEventHandler(shared_ptr);
};

/**
 * @brief Base class for objects supporting 2D position. Position is relative to its parent.
 */
class Object2D : public Object
{
public:
    Vect2f offset;           ///< offset relative to parent, or global position if root
    const Vect2f position(); ///< actual position, result of parent.position + offset

    Vect2f base_size;    ///< the 'original' size of the object, can be used to remove scaling
    float scale = 1;     ///< factor by which to scale the object
    const Vect2f size(); ///< actual size of the object, scale effected by parrent

    Vect2f rotation; ///< rotation relative to the parent
    const Vect2f orientation() { return {0, 0}; };

    /**
     * @brief Construct a new Object2D
     */
    Object2D();

    virtual string desiredName() override;

    /**
     * @brief Construct a new Object2D
     * @param offset
     * @param base_size
     */
    Object2D(Vect2f offset, Vect2f base_size);
};

/**
 * @brief Base class for all objects drawn on screen.
 */
class GraphicObject : public Object2D
{
    friend GraphicSystem;

public:                                  // TODO: protected later?
    SDL_Renderer *render_view = nullptr; // due to SDL shenanigans, smart pointers are not an option
    GraphicSystem *gsys_view = nullptr;
    enum Color
    {
        RED,
        GREEN,
        BLUE
    };
    Color color = RED;
    int height = 0; ///< also called z, sets draw height/draw order for objects occupying the same space.
public:
    /**
     * @brief Construct a new GraphicObject
     * 
     */
    GraphicObject();

    /**
     * @brief Construct a new GraphicObject
     * 
     * @param offset
     * @param base_size
     */
    GraphicObject(Vect2f offset, Vect2f base_size);

    virtual string desiredName() override;

    /**
     * @brief Set the draw Height of the object. When objects are occupying the same space,
     * the object with the largest height will be drawn above the rest.
     * 
     * @param height
     */
    void setDrawHeight(int height);

    static void setDrawColor(SDL_Renderer *render, Color c);

    virtual void draw() = 0;
};

/**
 * @brief Object representing texture data. Can be used to build sprites.
 */
class Texture : public Object
{
    SDL_Texture *texture;
    Vect2i size;
    map<string, Sprite> sprites;

public:

    virtual string desiredName() override;

    /**
     * @brief Set the internal SDL_Texture
     * @param render SDL_Renderer
     * @param filepath File from which to load the image. Relative path is relative to executable location.
     */
    void setTexture(SDL_Renderer *render, string filepath);

    /**
     * @brief Get the internal SDL_Texture, do not use unless you know what you're doing
     * @return SDL_Texture* pointer to the internal SDL_Texture, guaranteed to be valid for the lifetime of the `Texture` object
     */
    SDL_Texture *getTexture();

    /**
     * @brief Adds a sprite definition to the atlas. This can be used to then produce that sprite.
     *
     */
    void defineSprite(Vect4i src_region, string name);

    /**
     * @brief Creates a sprite previously defined by `defineSprite`
     *
     * @param name the name given to the sprite in `defineSprite`
     * @return shared_ptr<Sprite>
     */
    shared_ptr<Sprite> buildSprite(string name);

    /**
     * @brief Destroy the Texture object
     *
     */
    ~Texture();
};

/**
 * @brief A sprite on the screen. 
 */
class Sprite : public GraphicObject
{
    shared_ptr<Texture> texture; ///< Texture for the sprite to use
    SDL_Rect src_region;         ///< The region of the underlying texture this sprite uses
public:
    /**
     * @brief Construct a new Sprite object
     */
    Sprite();

    /**
     * @brief Construct a new Sprite object
     * @param texture the texture to be used by the sprite
     * @param src_region the region from the texture to be used by the sprite
     * @param offset offset of the sprite
     * @param size size of the sprite
     */
    Sprite(shared_ptr<Texture> texture, Vect4i src_region, Vect2f offset, Vect2f size);

    virtual string desiredName() override;

    /**
     * @brief Scale size to have a width of 'x'
     */
    void scaleX(int x);

    /**
     * @brief Scale size to have a height of 'y'
     */
    void scaleY(int y);

    /**
     * @brief Draw the sprite
     */
    void draw() override;
};

/**
 * @brief Object that plays a sound
 */
class AudioPlayer : public Object
{
    Mix_Chunk *sound;
    int volume;
public:
    bool loop = false;
    AudioPlayer(string file)
    {
        sound = Mix_LoadWAV(file.c_str());
        if (!sound)
            throw std::runtime_error(string() + "Failed to load WAV file: " + Mix_GetError());
    }

    virtual string desiredName() override;

    int play()
    {
        int play_count = 1;
        if (loop)
            play_count = -1;
        if (Mix_PlayChannel(play_count, sound, 0) == -1) // side effect plays sound
        {
            std::cerr << "Failed to play sound: " << Mix_GetError() << '\n'; // acceptable failure
            return 1;
        }
        return 0;
    }

    /**
     * @brief Set the sound volume in range between 0 and 128, 0 being mute and 128 being max volume.
     * 
     * @param volume number between 0 and 128
     */
    void setVolume(int volume)
    {
        if(volume > 128)
            volume = 128;
        if(volume < 0)
            volume = 0;
        
        Mix_VolumeChunk(sound, volume);
    }

    ~AudioPlayer()
    {
        Mix_FreeChunk(sound);
    }
};

class EngineController : public Object
{
    Clock timeout;

public:
    void init() override;

    void loop(double delta) override;
};
