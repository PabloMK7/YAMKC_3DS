#include "Speedometer.hpp"

const float Speedometer::screenCover = 0.3f;

Speedometer::Speedometer()
{
    meterTex = new Texture("romfs:/speedometer/meter.t3x");
    needleTex = new Texture("romfs:/speedometer/needle.t3x");
    meterPlane.GetScale() = Vector3(2.f * screenCover, 2.f * screenCover, 2.f * screenCover);
    needlePlane.GetScale() = Vector3(2.5f * screenCover, 2.5f * screenCover, 2.5f * screenCover);
    meterPlane.GetPosition() = Vector3(0.f, -(1 - screenCover / 1.f) + 0.05f, 0.f);
    needlePlane.GetPosition() = Vector3(0.f, -1 + 0.089f, 0.01f);
}

Speedometer::~Speedometer()
{
    if (meterTex) delete meterTex;
    if (needleTex) delete needleTex;
}

void Speedometer::SetNeedleAngle(const Angle& angle)
{
    needlePlane.GetRotation().z = angle * -1;
}

void Speedometer::Draw(int w, int h)
{
    /*
    float ratio = (float)w / h;

    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-ratio, ratio, -1.f, 1.f, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    meterPlane.GetPosition().x = (ratio - screenCover / 1.f) - 0.0537f;
    meterTex->Bind();
    meterPlane.Draw();

    needlePlane.GetPosition().x = ratio - 0.1f;
    needleTex->Bind();
    needlePlane.Draw();

    glDisable(GL_BLEND);
    */
}
