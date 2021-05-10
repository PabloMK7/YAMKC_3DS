#include "Chronometer.hpp"
#include <string>

Chronometer::Chronometer() : frames(0), isPaused(true) {
    text.SetPosition(Vector3(10.f, 6.f, 0.f));
    text.SetColor(Color(1.f, 0.5f, 0.f, 1.f), true);
    text.SetColor(Color(1.f, 1.f, 0.f, 1.f), false);
    text.SetScale(Vector2(0.85f, 0.85f));
    fade.GetPosition() = Vector3(5.f + 64.f, 5.f + 14.f, 0.f);
    fade.SetFade(0.4f);
    fade.GetScale() = Vector3(128.f, 27.f, 0.f);
}

Chronometer::~Chronometer() {
}

void Chronometer::Restart() {
    frames = 0;
}

void Chronometer::Tick() {
    if (!isPaused) frames++;
    if (frames > 359999) frames = 359999;
}

void Chronometer::Pause() {
    isPaused = true;
}

void Chronometer::Play() {
    isPaused = false;
}

int Chronometer::GetElapsed() {
    return frames;
}

void Chronometer::Draw() {
    u32 totSec = frames / 60;
    int sec = totSec % 60;
    int min = totSec / 60;
    int milli = (frames % 60) * (1000.f / 60);
    char buf[20];
    sprintf(buf, "%02d:%02d:%03d", min, sec, milli); // Pretty inefficient but whatever
    text.SetText(buf);
    fade.Draw();
    text.Draw();
}
