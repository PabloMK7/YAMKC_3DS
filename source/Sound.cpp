#include "Sound.hpp"
#include <3ds.h>
#include <cwav.h>
#include <cwav_file.h>

static bool loadSoundFromFile(const char* fileName, CWAV* sound, int simulPlays) {
    cwavFileLoad(sound, fileName, simulPlays);
    return (sound->loadStatus) == CWAV_SUCCESS;
}

Sound::Sound(const std::string& fileName, int simulPlays) {
    sound = (CWAV*) malloc(sizeof(CWAV));
    isLoaded = loadSoundFromFile(fileName.c_str(), sound, simulPlays);
}

Sound::~Sound() {
    cwavFileFree(sound);
    if (sound) free(sound);
}

bool Sound::IsLoaded() {
    return isLoaded;
}

void Sound::Play() {
    if(isLoaded)
        cwavPlay(sound, 0, -1);
}

void Sound::StereoPlay() {
    if(isLoaded)
    {
        sound->monoPan = -1.f;
        cwavPlay(sound, 0, -1);
        sound->monoPan = 1.f;
        cwavPlay(sound, 1, -1);
    }
        
}

void Sound::Stop() {
    if(isLoaded)
        cwavStop(sound, 0, -1);
}

bool Sound::IsPlaying() {
    if(isLoaded) {
        return cwavIsPlaying(sound);
    } else {
        return isLoaded;
    }
}

void Sound::SetVolume(float volume) {
    if(isLoaded) 
        sound->volume = volume;
}