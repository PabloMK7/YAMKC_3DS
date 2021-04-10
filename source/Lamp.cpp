#include "Lamp.hpp"

Lamp::Lamp(Vector3& pos, Angle3& rot, Vector3& scal) //GLenum light)
{
    position = pos;
    rotation = rot;
    scale = scal;

    lampObject = new Obj("romfs:/lamp/lamp_model.obj");
    /*lampObject->GetMaterial("mat_lamp_cast").SetRenderMode(Obj::Material::RenderMode::ADDITIVE);*/
    lampObject->GetMaterial("mat_lamp_cast").SetVisible(false);
    /*lampObject->GetMaterial("mat_lamp_cast").ForceDisableFog(true);*/

    /*lampLight = new Light(light);
    lampLight->SetType(Light::LightType::SPOT);
    lampLight->SetPosition(Vector3(0.f, 130.f, 0.f));
    lampLight->SetColor(Light::ColorType::AMBIENT, Color(0.0f, 0.0f, 0.0f));
    lampLight->SetColor(Light::ColorType::DIFFUSE, Color(2.5f, 2.5f, 0.0f));
    lampLight->SetSpotParams(Vector3(0.f, -1.f, 0.f), Angle::FromDegrees(80.f), 2.5f);
    lampLight->Disable();*/
    dayMode = true;
}

Lamp::~Lamp()
{
    //delete lampLight;
    delete lampObject;
}

void Lamp::setDayMode(bool isDay)
{
    /*float scale;
    if (!dayMode && isDay)
        scale = 1.f / 0.55f;
    else if (dayMode && !isDay)
        scale = 0.55f;
    else
        scale = 1.f;

    lampObject->GetMaterial("mat_lamp_body").GetColor(Obj::Material::ColorType::AMBIENT).Scale(scale, false);
    lampObject->GetMaterial("mat_lamp_cast").SetVisible(!isDay);*/

    dayMode = isDay;
}
/*
void Lamp::UpdateLight()
{
    
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.x.AsDegrees(), 1.f, 0.f, 0.f);
    glRotatef(rotation.y.AsDegrees(), 0.f, 1.f, 0.f);
    glRotatef(rotation.z.AsDegrees(), 0.f, 0.f, 1.f);
    glScalef(scale.x, scale.y, scale.z);

    lampLight->Update();

    glPopMatrix();
    
}

void Lamp::EnableLight(bool enable)
{
    if (enable && !dayMode)
        lampLight->Enable();
    else
        lampLight->Disable();
}
*/

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
