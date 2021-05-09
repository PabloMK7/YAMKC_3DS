#include "Sound.hpp"
#include <3ds.h>
#include <cwav.h>
#include <cwav_file.h>
#include "cstring"

static bool loadSoundFromFile(const char* fileName, CWAV* sound, int simulPlays) {
    cwavFileLoad(sound, fileName, simulPlays);
    return (sound->loadStatus) == CWAV_SUCCESS;
}

Sound::Sound(const std::string& fileName, int simulPlays) {
    sound = (CWAV*) malloc(sizeof(CWAV));
    isLoaded = loadSoundFromFile(fileName.c_str(), sound, simulPlays);
    toVolAmount = -1.f;
    toPitchAmount = -1.f;
}

Sound::~Sound() {
    cwavFileFree(sound);
    if (sound) free(sound);
}

bool Sound::IsLoaded() {
    return isLoaded;
}

void Sound::Play() {
    lastPlayFrames = 5;
    if(isLoaded) {
        float bak = sound->volume;
        sound->volume *= masterVolume;
        cwavPlayResult res = cwavPlay(sound, 0, -1);
        sound->volume = bak;
        if (res.playStatus == CWAV_SUCCESS)
        {
            lChann = res.monoLeftChannel;
            rChann = -1;
        }
    }
}

void Sound::EnsurePlaying() {
    currStopFrame = 0;
    if (!IsPlaying()) Play();
}

void Sound::StereoPlay() {
    if(isLoaded)
    {
        cwavPlayResult res = cwavPlay(sound, 0, 1);
        if (res.playStatus == CWAV_SUCCESS)
        {
            lChann = res.monoLeftChannel;
            rChann = res.rightChannel;
        }
    }        
}

void Sound::Stop() {
    if(isLoaded)
        cwavStop(sound, 0, -1);
}

bool Sound::IsPlaying() {
    if (lastPlayFrames) return true;
    if(isLoaded) {
        return cwavIsPlaying(sound);
    } else {
        return isLoaded;
    }
}

void Sound::SetPitch(float amount, bool affect) {
    if (!isLoaded)
        return;
    if (affect)
        sound->pitch = amount;
    else {
        fromPitchAmount = amount;
        toDPitchAmount = amount;
    }        
    if (IsPlaying() && affect)
    {
        if (rChann != -1)
        {
            ndspChnSetRate(rChann, sound->sampleRate * amount);
        }
        ndspChnSetRate(lChann, sound->sampleRate * amount);
    }
}

void Sound::SetMasterVolume(float volume) {
    masterVolume = volume;
}

void Sound::SetVolume(float volume) {
    if (!isLoaded)
        return;
    float mix[12] = {0};
    sound->volume = volume;
    if (IsPlaying())
    {
        if (rChann == -1)
        {
            float rightPan = (sound->monoPan + 1.f) / 2.f;
            float leftPan = 1.f - rightPan;
            mix[0] = 0.8f * leftPan * volume * masterVolume; // Left front
            mix[2] = 0.2f * leftPan * volume * masterVolume; // Left back
            mix[1] = 0.8f * rightPan * volume * masterVolume; // Right front
            mix[3] = 0.2f * rightPan * volume * masterVolume; // Right back
            ndspChnSetMix(lChann, mix);
        } else {
            
            mix[0] = 0.8f * volume * masterVolume; // Left front
            mix[2] = 0.2f * volume * masterVolume; // Left back
            ndspChnSetMix(lChann, mix);
            memset(mix, 0, sizeof(mix));
            mix[1] = 0.8f  * volume * masterVolume; // Right front
            mix[3] = 0.2f  * volume * masterVolume; // Right back
            ndspChnSetMix(rChann, mix);
        }
        
    }
        
}

void Sound::SetTargetVolume(float amount, int frames) {
    if (amount == toVolAmount)
        return;
    currVolFrame = frames;
    targetVolFrame = frames;
    fromVolAmount = sound->volume;
    toVolAmount = amount;
}
void Sound::SetCreatePitch(float amount, int frames) {
    if (amount == toPitchAmount)
        return;
    currPitchFrame = frames;
    targetPitchFrame = frames;
    fromPitchAmount = sound->pitch;
    toPitchAmount = amount;
}
void Sound::SetDecayPitch(float amount, int frames) {
    if (amount == toDPitchAmount)
        return;
    currDPitchFrame = frames;
    targetDPitchFrame = frames;
    fromDPitchAmount = sound->pitch;
    toDPitchAmount = amount;
}

void Sound::SetTargetStop(int frames) {
    if (currStopFrame == 0 && IsPlaying())
        currStopFrame = frames;
}

void Sound::Tick() {
    if (lastPlayFrames) lastPlayFrames--;
    if (currVolFrame != 0) {
        float prog = 1.f - (--currVolFrame / (float)targetVolFrame);
        float newVol = fromVolAmount + (toVolAmount - fromVolAmount) * prog;
        SetVolume(newVol);
    }
    if (currPitchFrame != 0) {
        float prog = 1.f - (--currPitchFrame / (float)targetPitchFrame);
        float newPitch = fromPitchAmount + (toPitchAmount - fromPitchAmount) * prog;
        SetPitch(newPitch, true);
    }
    if (currDPitchFrame != 0) {
        float prog = 1.f - (--currDPitchFrame / (float)targetDPitchFrame);
        float newPitch = fromDPitchAmount + (toDPitchAmount - fromDPitchAmount) * prog;
        SetPitch(newPitch, true);
    }
    if (currStopFrame != 0) {
        if (!--currStopFrame)
            Stop();
    }
}