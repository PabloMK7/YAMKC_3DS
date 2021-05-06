#pragma once
#include <string>
#include <3ds.h>

#include <cwav_file.h>

class Sound {
    public:
    Sound(const std::string& fileName, int simulPlays);
	~Sound();
    bool IsLoaded();
    bool IsPlaying();
    void Play();
    void StereoPlay();
    void Stop();
    void SetVolume(float volume);

    private:
    CWAV* sound;
    bool isLoaded;
};