// class B file - too long, some vestigial code
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h> //
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <clock.h>   // clock/timer utility

#include "std_includes.hpp"
#include "colors.h" // #defined RGB_COLORs
#include "vects.hpp" // Mathematical vectors

// defined here
class Object;
class Object2D;
class GraphicObject;
class Texture;
class Sprite;
class AudioPlayer;
class EngineController;
class HandlerI;

// extern
class GraphicSystem;
class Engine;
#include "base_object.hpp"

/**
 * @brief Class for storing blueprintf of other objects and building them
 */
class BlueprintFactory : public Object
{
    map<string, shared_ptr<Object>> blueprints;
public:
    BlueprintFactory(string desiredName = "BlueprintFactory");

    void addBlueprint(shared_ptr<Object> object, string name);

    void removeBlueprint(string name);

    shared_ptr<Object> build(string name);
    
    template<typename T>
    shared_ptr<T> build(string name)
    {
        auto print = dynamic_pointer_cast<T>(build(name));
        if (print.get() == nullptr)
            throw std::runtime_error("Blueprint not of specified type");
        return print;
    }
};

/**
 * @brief Base class for objects supporting 2D position. Position is relative to its parent.
 */
class Object2D : public Object
{
public:
    Vect2f offset;           ///< offset relative to parent, or global position if root
    const Vect2f getPosition(); ///< actual position, result of parent.position + offset 
    void setPosition(Vect2f pos) ///< set offset such that getPosition() returns pos
    {
        offset -= getPosition() - pos;
    }

    Vect2f base_size;    ///< the 'original' size of the object, can be used to remove scaling
    float scale = 1;     ///< factor by which to scale the object
    const Vect2f getSize(); ///< actual size of the object, scale effected by parrent

    Vect2f rotation = {1, 0}; ///< rotation relative to the parent, as a unit vector
    const Vect2f getOrientation();

    /**
     * @brief Construct a new Object2D
     */
    Object2D(string desiredName = "Object2D");

    /**
     * @brief Construct a new Object2D
     * @param offset
     * @param base_size
     */
    Object2D(Vect2f offset, Vect2f base_size, string desiredName = "Object2D");

    /**
     * @brief Construct a new GraphicObject
     * @param rect
     */
    Object2D(Vect4f rect, string desiredName = "Object2D");

    /**
     * @brief Scale size to have a width of 'x'
     */
    void scaleX(int x);

    /**
     * @brief Scale size to have a height of 'y'
     */
    void scaleY(int y);
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
    int z = 0; ///< also called z, sets draw z/draw order for objects occupying the same space.
public:
    /**
     * @brief Construct a new GraphicObject
     * 
     */
    GraphicObject();

    /**
     * @brief Construct a new GraphicObject
     * @param offset
     * @param base_size
     */
    GraphicObject(Vect2f offset, Vect2f base_size, string desiredName = "GraphicObject");

    /**
     * @brief Set the draw Height of the object. When objects are occupying the same space,
     * the object with the largest z will be drawn above the rest.
     * 
     * @param z
     */
    void setDrawHeight(int z);

    static void setDrawColor(SDL_Renderer *render, Color c);

    virtual void draw() = 0;
};

/**
 * @brief Object representing texture data.
 */
class Texture : public Object
{
    SDL_Texture *texture;
    Vect2i size;
public:

    Vect2i getSize() const;

    /**
     * @brief Construct a new Texture object
     * @param desiredName
     */
    Texture(string desiredName = "Texture");
    
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
     * @brief Destroy the Texture object
     */
    ~Texture();
};

/**
 * @brief A sprite on the screen. 
 */
class Sprite : public GraphicObject
{
protected:
    shared_ptr<Texture> texture; ///< Texture for the sprite to use
    SDL_Rect src_region;         ///< The region of the underlying texture this sprite uses
public:

    /**
     * @brief Construct a new Sprite object
     * @param texture the texture to be used by the sprite
     * @param origin the region from the texture to be used by the sprite
     * @param size the region from the texture to be used by the sprite
     */
    Sprite(shared_ptr<Texture> texture, Vect2i origin, Vect2i size, string desiredName = "Sprite");

    /**
     * @brief Returns a deep copy of the object. The clone is entirely separate from the original.
     * @return shared_ptr<Object> the cloned object
     */
    shared_ptr<Object> clone() const override;

    /**
     * @brief Draw the sprite
     */
    void draw() override;
};

/**
 * @brief An animated sprite on the screen. 
 */
class AnimatedSprite : public Sprite
{
    int frames; ///< Number of animation frames
    int current_frame = 0; ///< Current frame of the animation
    int ticks_per_frame; ///< How long to hold each frame, in graphic updates
    int current_tick = 0; 
    Vect2i start_origin;
public:
    /**
     * @brief Construct a new AnimatedSprite object
     * @param step the offset by which to change the position of the source for each frame of the sprite
     * @param texture the texture to be used by the sprite
     * @param src_region the region from the texture to be used by the sprite
     * @param offset offset of the sprite
     * @param size size of the sprite
     */
    AnimatedSprite(int frames, int frame_duration, shared_ptr<Texture> texture, Vect2i origin, Vect2i size, string desiredName = "AnimatedSprite");

    /**
     * @brief Returns a deep copy of the object. The clone is entirely separate from the original.
     * @return shared_ptr<Object> the cloned object
     */
    shared_ptr<Object> clone() const override;

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
    AudioPlayer(string file, string desiredName = "Sprite") : Object(desiredName)
    {
        sound = Mix_LoadWAV(file.c_str());
        if (!sound)
            throw std::runtime_error(string() + "Failed to load WAV file: " + Mix_GetError());
    }

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
