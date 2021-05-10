#include "EndingDisplay.hpp"

EndingDisplay::EndingDisplay() {
    yourTime.SetText("TIME");
    yourTime.SetPosition(Vector3(0.f, 90.f, 0.1f));
    yourTime.CenterH(true);
    yourTime.SetColor(Color(0.75f, 0.75f, 0.75f), true);
    theTime.SetPosition(Vector3(0.f, 120.f, 0.1f));
    theTime.SetColor(Color(0.75f, 0.75f, 0.75f), true);
    SetTime(0);
    gameName.SetText("Yet Another Mario Kart Clone");
    gameName.SetScale(Vector2(2.f/3.f, 2.f/3.f));
    gameName.SetPosition(Vector3(0.f, 80.f, 0.1f));
    gameName.CenterH(false);
    gameName.SetColor(Color(0.75f, 0.75f, 0.75f), true);
    credits.SetText("by PabloMK7, Ragnarok, JoanCoCo");
    credits.SetScale(Vector2(1.5f/3.f, 1.5f/3.f));
    credits.SetPosition(Vector3(0.f, 100.f, 0.1f));
    credits.CenterH(false);
    credits.SetColor(Color(0.75f, 0.75f, 0.75f), true);
    pressToExit.SetText("Press START to exit.");
    pressToExit.SetScale(Vector2(2.f/3.f, 2.f/3.f));
    pressToExit.SetPosition(Vector3(0.f, 140.f, 0.1f));
    pressToExit.CenterH(false);
    pressToExit.SetColor(Color(0.75f, 0.75f, 0.75f), true);
}

EndingDisplay::~EndingDisplay() {

}

void EndingDisplay::SetTime(int frames) {
    u32 totSec = frames / 60;
    int sec = totSec % 60;
    int min = totSec / 60;
    int milli = (frames % 60) * (1000.f / 60);
    char buf[20];
    sprintf(buf, "%02d:%02d:%03d", min, sec, milli); // Pretty inefficient but whatever
    theTime.SetText(buf);
    theTime.CenterH(true);
}

void EndingDisplay::DrawTop() {
    yourTime.Draw();
    theTime.Draw();
}

void EndingDisplay::DrawBottom() {
    gameName.Draw();
    credits.Draw();
    pressToExit.Draw();
}