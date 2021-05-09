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
    void EnsurePlaying();
    void StereoPlay();
    void Stop();
    void SetMasterVolume(float volume);
    void SetVolume(float volume);
    void SetPitch(float amount, bool affect = false);
    void SetTargetVolume(float amount, int frames);
    void SetCreatePitch(float amount, int frames);
    void SetDecayPitch(float amount, int frames);
    void SetTargetStop(int frames);
    void Tick();

    private:
    CWAV* sound;
    bool isLoaded;
    float masterVolume = 1.f;
    int lastPlayFrames = 0;
    int lChann = -1, rChann = -1;
    int targetVolFrame, targetPitchFrame, targetDPitchFrame;
    int currVolFrame = 0, currPitchFrame = 0, currDPitchFrame = 0, currStopFrame = 0;
    float fromVolAmount, fromPitchAmount, fromDPitchAmount;
    float toVolAmount, toPitchAmount, toDPitchAmount;
};