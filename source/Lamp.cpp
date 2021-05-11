#include "Lamp.hpp"

Lamp::Lamp(Vector3& pos, Angle3& rot, Vector3& scal, Collision& col) //GLenum light)
{
    position = pos;
    rotation = rot;
    scale = scal;

    lampObject = new Obj("romfs:/lamp/lamp_model.obj");
    col.AddResource("romfs:/lamp/lamp_collision.kcl", pos, rot);
    lampObject->GetMaterial("mat_lamp_cast").SetVisible(false);
    dayMode = true;
}

Lamp::~Lamp()
{
    delete lampObject;
}

void Lamp::setDayMode(bool isDay)
{
    dayMode = isDay;
}

void Lamp::Draw()
{
    C3D_Mtx* m = Graphics::PushModelViewMtx();
    Mtx_Translate(m, position.x, position.y, position.z, true);
    Mtx_RotateX(m, rotation.x.AsRadians(), true);
    Mtx_RotateY(m, rotation.y.AsRadians(), true);
    Mtx_RotateZ(m, rotation.z.AsRadians(), true);
    Mtx_Scale(m, scale.x, scale.y, scale.z);
    Graphics::UpdateModelViewMtx();

    lampObject->Draw();

    Graphics::PopModelViewMtx();
}
