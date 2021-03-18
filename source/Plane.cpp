#include "Plane.hpp"

static const Graphics::GPUVertex planeVInfo[4] = {
    {{-0.5f, -0.5f, 0.f}, {0.f, -1.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 1.f, 1.f}}, // xyz, uv, nxnynz, rgba
    {{-0.5f, 0.5f, 0.f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 1.f, 1.f}},
    {{0.5f, 0.5f, 0.f}, {1.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 1.f, 1.f}},
    {{0.5f, -0.5f, 0.f}, {1.f, -1.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 1.f, 1.f}}
};

Plane::Plane()
{
    scale = Vector3(1.f, 1.f, 1.f);
    mat.AddFace(std::make_tuple(planeVInfo[2], planeVInfo[1], planeVInfo[0]));
    mat.AddFace(std::make_tuple(planeVInfo[3], planeVInfo[2], planeVInfo[0]));
    mat.ConvertToVBO();
}

Plane::~Plane() {}

Vector3& Plane::GetPosition()
{
    return position;
}

Angle3& Plane::GetRotation()
{
    return rotation;
}

Vector3& Plane::GetScale()
{
    return scale;
}

Material& Plane::GetMaterial()
{
    return mat;
}

void Plane::Draw()
{
    C3D_Mtx* m = Graphics::PushModelViewMtx();
    Mtx_Translate(m, position.x, position.y, position.z, true);
    Mtx_RotateX(m, rotation.x.AsRadians(), true);
    Mtx_RotateY(m, rotation.y.AsRadians(), true);
    Mtx_RotateZ(m, rotation.z.AsRadians(), true);
    Mtx_Scale(m, scale.x, scale.y, scale.z);
    Graphics::UpdateModelViewMtx();

    mat.Draw();

    Graphics::PopModelViewMtx();
}
