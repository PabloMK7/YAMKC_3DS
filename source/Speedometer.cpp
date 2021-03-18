#include "Speedometer.hpp"

const float Speedometer::screenCover = 0.3f;

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
    meterPlane.GetScale() = Vector3(2.f * screenCover, 2.f * screenCover, 2.f * screenCover);
    needlePlane.GetScale() = Vector3(2.5f * screenCover, 2.5f * screenCover, 2.5f * screenCover);
    meterPlane.GetPosition() = Vector3(0.f, -(1 - screenCover / 1.f) + 0.05f, 0.f);
    needlePlane.GetPosition() = Vector3(0.f, -1 + 0.089f, 0.01f);
}

Speedometer::~Speedometer()
{
}

void Speedometer::SetNeedleAngle(const Angle& angle)
{
    needlePlane.GetRotation().z = angle * -1.f;
}

static const C3D_FVec laPos = {0.f, 1.f, 0.f, 0.f};
static const C3D_FVec laLookAt = {0.f, 0.f, 0.f, 0.f};
static const C3D_FVec laUp = {0.f, 0.f, 1.f, 0.f};

void Speedometer::Draw(C3D_RenderTarget* target, int w, int h)
{
    float ratio = (float)w / h;
    C3D_RenderTargetClear(target, C3D_CLEAR_DEPTH, 0, 0);

    Mtx_LookAt(Graphics::GetModelViewMtx(), laPos, laLookAt, laUp, false);

    C3D_Mtx* p = Graphics::GetProjectionMtx();
    Mtx_OrthoTilt(p, -ratio, ratio, -1.f, 1.f, -1.f, 1.f, false);
    Graphics::UpdateProjectionMtx();

    meterPlane.GetPosition().x = (ratio - screenCover / 1.f) - 0.0537f;
    meterPlane.Draw();

    needlePlane.GetPosition().x = ratio - 0.1f;
    needlePlane.Draw();
}
