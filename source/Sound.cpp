#include "Sound.hpp"
#include <3ds.h>
#include <cwav.h>
#include <cwav_file.h>

bool Sound::loadSoundFromFile(const char* fileName, CWAV* sound) {
    cwavFileLoad(sound, fileName, 5);
    return (sound->loadStatus) == CWAV_SUCCESS;
}

Sound::Sound(const std::string& fileName) {
    sound = (CWAV*) malloc(sizeof(CWAV));
    isLoaded = loadSoundFromFile(fileName.c_str(), sound);
}

Sound::~Sound() {
    if(isLoaded) 
        cwavFileFree(sound);
}

bool Sound::IsLoaded() {
    return isLoaded;
}

void Sound::Play() {
    if(isLoaded)
        cwavPlay(sound, 0, -1);
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