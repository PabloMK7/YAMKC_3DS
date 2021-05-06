#include "Speedometer.hpp"

const float Speedometer::screenCover = 0.4f;
const float Speedometer::screenMargin = 5.f; // Pixels
const Vector3 Speedometer::needleOffset = Vector3(470 - 256, 480 - 256, 0.f); // As if the image was 512x512

static void SpdBlendMode(Material& mat, void* data)
{
    C3D_FragOpMode(GPU_FRAGOPMODE_GL);
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ZERO, GPU_ZERO);
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    C3D_DepthTest(false, GPU_ALWAYS, GPU_WRITE_COLOR);
}

Speedometer::Speedometer()
{
    meterPlane.GetMaterial().SetTexture("romfs:/speedometer/meter.t3x").SetBlendMode(SpdBlendMode, nullptr);
    needlePlane.GetMaterial().SetTexture("romfs:/speedometer/needle.t3x").SetBlendMode(SpdBlendMode, nullptr);
    meterPlane.GetScale() = Vector3(240.f * screenCover, 240.f * screenCover, 1.f);
    needlePlane.GetScale() = Vector3(240.f * screenCover * 1.25f, 240.f * screenCover * 1.25f, 1.f);
    meterPlane.GetPosition() = Vector3(400.f - (240.f * screenCover) + ((240.f * screenCover) / 2) - screenMargin, 240.f - (240.f * screenCover) + ((240.f * screenCover) / 2) - screenMargin, 0.f);
    needlePlane.GetPosition() = meterPlane.GetPosition() + needleOffset * (240.f * screenCover / 512.f);
}

Speedometer::~Speedometer()
{
}

void Speedometer::SetNeedleAngle(const Angle& angle)
{
    needlePlane.GetRotation().z = angle;
}

void Speedometer::Draw()
{
    meterPlane.Draw();
    needlePlane.Draw();
}
