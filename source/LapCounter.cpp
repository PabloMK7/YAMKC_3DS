#include "LapCounter.hpp"
#include <string>

LapCounter::LapCounter() {
    text.SetPosition(Vector3(224.f, 6.f, 0.32f));
    text.SetColor(Color(1.f, 0.5f, 0.f, 1.f), true);
    text.SetColor(Color(1.f, 1.f, 0.f, 1.f), false);
    text.SetScale(Vector2(0.85f, 0.85f));
    fade.GetPosition() = Vector3(219.f + 48.f, 5.f + 14.f, 0.3f);
    fade.SetFade(0.4f);
    fade.GetScale() = Vector3(95.f, 27.f, 0.f);
}

LapCounter::~LapCounter() {
}

void LapCounter::SetLap(int lap) {
    if (lap >= 1 && lap <= 3)
        currLap = lap;
}

void LapCounter::Draw() {
    char buf[20];
    sprintf(buf, "LAP %d/3", currLap); // Pretty inefficient but whatever
    text.SetText(buf);
    fade.Draw();
    text.Draw();
}
