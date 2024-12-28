#pragma once
#include <SDL2/SDL_mixer.h>

#include <objects.hpp>

/**
 * @brief Object that plays a sound
 * 
 */
class AudioPlayer : public Object
{
    Mix_Chunk* sound;
public:
    bool loop = false;
    AudioPlayer(string file)
    {
        sound = Mix_LoadWAV(file.c_str());
        if (!sound)
            throw  std::runtime_error(string() + "Failed to load WAV file: " + Mix_GetError());
    }

    int play()
    {
        int play_count = 1;
        if(loop)
            play_count = -1;
        if (Mix_PlayChannel(play_count, sound, 0) == -1) // side effect plays sound
        {
            std::cerr << "Failed to play sound: " << Mix_GetError() << '\n'; // acceptable failure
            return 1;
        }
        return 0;
    }

    ~AudioPlayer()
    {
        Mix_FreeChunk(sound);
    }
};
