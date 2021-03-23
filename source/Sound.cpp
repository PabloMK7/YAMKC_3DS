#include "Sound.hpp"
#include <3ds.h>
#include <cwav.h>
#include <cwav_file.h>

bool Sound::loadSoundFromFile(const char* fileName, CWAV* sound) {
    sound = (CWAV*) malloc(sizeof(CWAV));
    cwavFileLoad(sound, fileName, 5);
    return (sound->loadStatus) == CWAV_SUCCESS;
}

Sound::Sound(const std::string& fileName) {
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
    return isLoaded && cwavIsPlaying(sound);
}