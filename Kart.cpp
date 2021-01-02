#include "Kart.hpp"
#include <GL/freeglut.h>
#include <iostream>

const Vector3 Kart::defaultWheelPositions[4] = {
    Vector3(-4.524f, 2.129f, -5.311f), // Front-left
    Vector3(4.524f, 2.129f, -5.311f), // Front-right
    Vector3(-4.524f, 2.129f, 4.693f), // Back-left
    Vector3(4.524f, 2.129f, 4.693f) // Back-right
};

const Angle3 Kart::defaultWheelRotations[4] = {
    Angle3(Angle::Zero(), Angle::FromDegrees(180.f), Angle::Zero()), // Front-left
    Angle3(), // Front-right
    Angle3(Angle::Zero(), Angle::FromDegrees(180.f), Angle::Zero()), // Back-left
    Angle3() // Back-right
};

const Vector3 Kart::cameraOffset = Vector3(0, 35, 50);
const Vector3 Kart::cameraLookAtOffset = Vector3(0, 20, 0);
const float Kart::cameraFov = 55.f;

Kart::Kart(std::string kartName, std::string wheelName, std::string driverName, std::string shadowName)
{
    pressedKeys = 0;
    prevPressedKeys = 0;
    tiresRotateTimer = -1;

    try
    {
        kartObj = new Obj(kartName);
        driverObj = new Obj(driverName);
        for (int i = 0; i < 4; i++) {
            wheelObjs[i] = new Obj(wheelName);
            wheelObjs[i]->GetPosition() = defaultWheelPositions[i];
            wheelObjs[i]->GetRotation() = defaultWheelRotations[i];
        }
        shadowObj = new Obj(shadowName);
        shadowObj->GetPosition() += Vector3(0.f, 0.f, -0.5f);
    }
    catch (const char* msg) {
        std::cout << std::string("Failed to load kart: ") + msg << std::endl;
    }

    fromTireAngles[0] = wheelObjs[0]->GetRotation().y;
    fromTireAngles[1] = wheelObjs[1]->GetRotation().y;

    toTireAngles[0] = fromTireAngles[0];
    toTireAngles[1] = fromTireAngles[1];
}

Kart::~Kart()
{
    delete kartObj;
    delete driverObj;
    for (int i = 0; i < 4; i++)
        delete wheelObjs[i];
    delete shadowObj;
}

void Kart::UpdateCamera()
{
    Vector3 realCameraOffset = cameraOffset;
    realCameraOffset.z *= cameraRearView;
    Vector3 cameraPos = (GetPosition() + realCameraOffset);
    cameraPos.Rotate(GetRotation(), GetPosition());
    Vector3 cameraLookAt = GetPosition() + cameraLookAtOffset;

    gluLookAt(cameraPos.x , cameraPos.y, cameraPos.z, cameraLookAt.x, cameraLookAt.y, cameraLookAt.z, 0, 1, 0);
}

void Kart::UpdateViewPort(int w, int h)
{
    float ratio = (float)w / h;

    gluPerspective(cameraFov, ratio, 20, 10000);
}

void Kart::Draw()
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.x.AsDegrees(), 1.f, 0.f, 0.f);
    glRotatef(rotation.y.AsDegrees(), 0.f, 1.f, 0.f);
    glRotatef(rotation.z.AsDegrees(), 0.f, 0.f, 1.f);
    glScalef(scale.x, scale.y, scale.z);

    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    shadowObj->Draw();
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 4; i++)
        wheelObjs[i]->Draw();

    kartObj->Draw();
    driverObj->Draw();
    
    glPopMatrix();
}

void Kart::Calc(int elapsedMsec)
{
    totalElapsedTime += elapsedMsec;
    
    float animAngleSin = Angle::FromDegrees(totalElapsedTime * 360.f / 200.f).Sin();
    kartObj->GetPosition().y = animAngleSin * 0.1f;
    shadowObj->GetScale().x = 1.05f + animAngleSin * 0.01f;
    shadowObj->GetScale().z = 0.95f + animAngleSin * 0.01f;
    driverObj->GetPosition().y = Angle::FromDegrees((totalElapsedTime + 20) * 360.f / 200.f).Sin() * 0.1f;

    if ((prevPressedKeys & Key::KEY_Left) != (pressedKeys & Key::KEY_Left) || (prevPressedKeys & Key::KEY_Right) != (pressedKeys & Key::KEY_Right))
    {
        tiresRotateTimer = 0;
        fromTireAngles[0] = wheelObjs[0]->GetRotation().y;
        fromTireAngles[1] = wheelObjs[1]->GetRotation().y;
        if (pressedKeys & Key::KEY_Left) {
            toTireAngles[0] = defaultWheelRotations[0].y + Angle::FromDegrees(20);
            toTireAngles[1] = defaultWheelRotations[1].y + Angle::FromDegrees(20);
        }
        else if (pressedKeys & Key::KEY_Right) {
            toTireAngles[0] = defaultWheelRotations[0].y - Angle::FromDegrees(20);
            toTireAngles[1] = defaultWheelRotations[1].y - Angle::FromDegrees(20);
        }
        else {
            toTireAngles[0] = defaultWheelRotations[0].y;
            toTireAngles[1] = defaultWheelRotations[1].y;
        }
    }

    Angle left = Angle::Zero();
    Angle right = Angle::Zero();

    if (tiresRotateTimer >= 0 && tiresRotateTimer < 200) {
        left = fromTireAngles[0];
        right = fromTireAngles[1];
        left.Cerp(toTireAngles[0], tiresRotateTimer / 200.f);
        right.Cerp(toTireAngles[1], tiresRotateTimer / 200.f);

        tiresRotateTimer += elapsedMsec;
    }
    else
    {
        left = toTireAngles[0];
        right = toTireAngles[1];
        tiresRotateTimer = -1;
    }
    wheelObjs[0]->GetRotation().y = left;
    wheelObjs[1]->GetRotation().y = right;

    Vector3 forward = GetForward() * 5;
    if (pressedKeys & Key::KEY_Left)
        GetRotation().y += Angle::FromDegrees(1);
    else if (pressedKeys & Key::KEY_Right)
        GetRotation().y -= Angle::FromDegrees(1);
    if (pressedKeys & Key::KEY_A) {
        GetPosition() += forward;
    }
    else if (pressedKeys & Key::KEY_B) {
        GetPosition() -= forward;
    }

    if (pressedKeys & Key::KEY_X)
        cameraRearView = -1.f;
    else
        cameraRearView = 1.f;

    prevPressedKeys = pressedKeys;
}

void Kart::KeyPress(Kart::Key key)
{
    pressedKeys |= (unsigned int)key;
}

void Kart::KeyRelease(Key key)
{
    pressedKeys &= ~(unsigned int)key;
}

Vector3& Kart::GetPosition()
{
    return position;
}

Angle3& Kart::GetRotation()
{
    return rotation;
}

Vector3& Kart::GetScale()
{
    return scale;
}

Vector3 Kart::GetForward()
{
    Vector3 unit(0, 0, -1);
    unit.Rotate(GetRotation());
    unit.Normalize();
    return unit;
}

Obj* Kart::GetDriverObj()
{
    return driverObj;
}

unsigned int Kart::KeysJustPressed()
{
    return (pressedKeys ^ prevPressedKeys) & pressedKeys;
}
