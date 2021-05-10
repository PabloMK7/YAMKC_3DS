#include "CountdownDisplay.hpp"

CountdownDisplay::CountdownDisplay() : text(8) {
    text.SetColor(Color(1.f, 0.25f, 0.f, 1.f), true);
    text.SetColor(Color(1.f, 1.f, 0.f, 1.f), false);
    frameCounter = 0;
}

void CountdownDisplay::SetText(const char* t) {
    frameCounter = 60;
    text.SetText(t);
}

CountdownDisplay::~CountdownDisplay() {}

void CountdownDisplay::Tick() {
    if (frameCounter) {
        float prog = frameCounter / 60.f;
        text.SetScale(Vector2(4.f * prog, 4.f * prog));
        float sizeX, sizeY;
        text.GetTextSize(sizeX, sizeY);
        float posX = (400 - sizeX) / 2;
        float posY = (240 - sizeY) / 2;
        text.SetPosition(Vector3(posX, posY, 0.f));
        frameCounter--;
    }
}

void CountdownDisplay::Draw() {
    if (frameCounter)
        text.Draw();
}