#include "Kart.hpp"
#include <iostream>
#include "SoundLibrary.hpp"

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

const int Kart::collisionSpherePrecision = 200;
const float Kart::collisionSphereW = 6.45f;
const float Kart::collisionSphereH = 11.45f;

const Vector3 Kart::cameraOffset = Vector3(0, 36, 51);
const Vector3 Kart::cameraLookAtOffset = Vector3(0, 23, 0);
const float Kart::cameraFov = 55.f;
const float Kart::cameraRearViewMultiplyFactor = -1.25f;
const float Kart::cameraRotationCerpFactor = 0.025f;
const float Kart::cameraPositionCerpFactor = 0.3f;
const float Kart::cameraInterOcularDistanceMultiplier = 2.3f;
const float Kart::cameraFocalLength = 19.f;

const float Kart::engineAccelerationFactor = 2500.f;
const float Kart::backwardsAccelerationRatioFactor = -0.35f;
const float Kart::wheelResistanceFactor = 2.75f;
const float Kart::windResistanceFactor = 0.0092f;
const float Kart::kartMassFactor = 8.f;
const float Kart::maxWheelTurnAngle = 19.f;
const float Kart::turnWheelFrictionFactor = 1.f;
const float Kart::turnBySpeedFactor = 210.f;
const float Kart::maxTurnAmount = 0.9f;
const float Kart::inPlaceTurnFactor = 2.75f;
const float Kart::inPlaceStartMinSpeed = 5.f;
const float Kart::wheelSpinFactor = 6.f;
const float Kart::maxWheelSpinAmount = 15.f;
const float Kart::kartBounceFactor = 350.f;
const float Kart::kartGrowFactor = 400.f;
const float Kart::offroadFactor = 20.f;

const float Kart::realSpeedFactor = 0.178f;

Kart::Kart(std::string kartName, std::string wheelName, std::string driverName, std::string shadowName, Collision* col)
{
    pressedKeys = 0;
    prevPressedKeys = 0;
    currCameraPos = cameraOffset;
    speed = Vector3();
    collision = col;
    advancedAmount = 0;
    backAdvanceAmount = 0;
    canDrive = false;

    kartObj = new Obj(kartName);
    driverObj = new Obj(driverName);
    for (int i = 0; i < 4; i++) {
        wheelObjs[i] = new Obj(wheelName);
        wheelObjs[i]->GetPosition() = defaultWheelPositions[i];
        wheelObjs[i]->GetRotation() = defaultWheelRotations[i];
    }
    shadowObj = new Obj(shadowName);
    shadowObj->GetPosition() += Vector3(0.f, 0.15f, -0.5f);
    shadowObj->GetMaterial("mat_shadow").SetBlendMode(Material::BlendModeBlendedTransparency, nullptr).SetCostantColor(0, Color(0.f, 0.f, 0.f, 0.5f));

    fromTireAngles[0] = wheelObjs[0]->GetRotation().y;
    fromTireAngles[1] = wheelObjs[1]->GetRotation().y;

    toTireAngles[0] = fromTireAngles[0];
    toTireAngles[1] = fromTireAngles[1];

    // --- Sound --- //
    engine = new EngineSound();
    standStillSound = new Sound("romfs:/audio/kart/slide/standstill.bcwav", 1);
    sandSound = new Sound("romfs:/audio/kart/offroad/sand.bcwav", 1);
    grassSound = new Sound("romfs:/audio/kart/offroad/grass.bcwav", 1);
    turningSound = new Sound("romfs:/audio/kart/slide/turn.bcwav", 1);
    wallHitSound = new Sound("romfs:/audio/kart/wall/wall.bcwav", 1);
    turningSound->SetMasterVolume(0.9f);
    sandSound->SetMasterVolume(1.f);
    grassSound->SetMasterVolume(1.f);
    wallHitSound->SetMasterVolume(0.9f);
    // ------------- //
}

Kart::~Kart()
{
    delete kartObj;
    delete driverObj;
    for (int i = 0; i < 4; i++)
        delete wheelObjs[i];
    delete shadowObj;

    // --- Sound --- //
    delete engine;
    delete standStillSound;
    delete turningSound;
    delete grassSound;
    delete sandSound;
    // ------------- //
}

void Kart::UpdateCamera()
{  
    C3D_FVec pos;
    C3D_FVec lookat;
    C3D_FVec up;
    pos.x = currCameraPos.x;
    pos.y = currCameraPos.y;
    pos.z = currCameraPos.z;
    lookat.x = cameraLookAt.x;
    lookat.y = cameraLookAt.y;
    lookat.z = cameraLookAt.z;
    up.x = 0;
    up.y = 1;
    up.z = 0;
    Mtx_LookAt(Graphics::GetModelViewMtx(), pos, lookat, up, false);
    oldCameraRearView = cameraRearView;
}

void Kart::CalcCamera()
{
    static Angle3 oldRotation = GetRotation();
    
    bool shouldCerp = (oldCameraRearView == cameraRearView);

    Vector3 realCameraOffset = cameraOffset;
    realCameraOffset.z *= cameraRearView;
    Vector3 cameraPos = (GetPosition() + realCameraOffset);
    oldRotation.Cerp(GetRotation(), shouldCerp ? cameraRotationCerpFactor : 1.f);
    cameraPos.Rotate(oldRotation, GetPosition());
    currCameraPos.Cerp(cameraPos, shouldCerp ? cameraPositionCerpFactor : 1.f);
    cameraLookAt = GetPosition() + cameraLookAtOffset;
}

void Kart::UpdateViewPort(int w, int h, float iod)
{
    float ratio = (float)w / h;

    C3D_Mtx* p = Graphics::GetProjectionMtx();
    Mtx_Identity(p);
    Mtx_PerspStereoTilt(p, Angle::DegreesToRadians(cameraFov), ratio, 20, 10000, iod * cameraInterOcularDistanceMultiplier, cameraFocalLength, false);
    Graphics::UpdateProjectionMtx();
}

void Kart::Draw()
{
    C3D_Mtx* m = Graphics::PushModelViewMtx();
    Mtx_Translate(m, position.x, position.y, position.z, true);
    Mtx_RotateX(m, rotation.x.AsRadians(), true);
    Mtx_RotateY(m, rotation.y.AsRadians(), true);
    Mtx_RotateZ(m, rotation.z.AsRadians(), true);
    Mtx_Scale(m, scale.x, scale.y, scale.z);
    Graphics::UpdateModelViewMtx();

    shadowObj->Draw();

    for (int i = 0; i < 4; i++)
        wheelObjs[i]->Draw();

    kartObj->Draw();
    driverObj->Draw();
    
    Graphics::PopModelViewMtx();
}

void Kart::Calc(int elapsedMsec)
{
    totalElapsedTime += elapsedMsec;
    Vector3 newKartPosition;
    
    {
        float currSpeed = speed.Magnitude();
        kartObj->GetPosition().y = 0;
        driverObj->GetPosition().y = 0;

        if (currSpeed < kartBounceFactor) {
            float animAngleSin = Angle::FromDegrees(totalElapsedTime * 360.f / 200.f).Sin();
            kartObj->GetPosition().y = animAngleSin * 0.1f * (1.f - currSpeed / kartBounceFactor);
            shadowObj->GetScale().x = 1.05f + animAngleSin * 0.01f * (1.f - currSpeed / kartBounceFactor);
            shadowObj->GetScale().z = 0.95f + animAngleSin * 0.01f * (1.f -currSpeed / kartBounceFactor);
            driverObj->GetPosition().y = Angle::FromDegrees((totalElapsedTime + 20) * 360.f / 200.f).Sin() * 0.1f * (1.f - currSpeed / kartBounceFactor);
        }
        float realGrowFactor = currSpeed / kartGrowFactor;
        if (realGrowFactor > 1.f)
            realGrowFactor = 1.f;
        kartObj->GetScale().y = (1.f + realGrowFactor * 0.1f);
        kartObj->GetScale().z = (1.f + realGrowFactor * 0.1f);
    }

    float circleH = canDrive ? xCirclePad : 0.f; 

    fromTireAngles[0] = wheelObjs[0]->GetRotation().y;
    fromTireAngles[1] = wheelObjs[1]->GetRotation().y;
    toTireAngles[0] = defaultWheelRotations[0].y + Angle::FromDegrees(maxWheelTurnAngle) * circleH * -1.f;
    toTireAngles[1] = defaultWheelRotations[1].y + Angle::FromDegrees(maxWheelTurnAngle) * circleH * -1.f;

    Angle left = Angle::Zero();
    Angle right = Angle::Zero();

    left = fromTireAngles[0];
    right = fromTireAngles[1];
    left.Cerp(toTireAngles[0], 0.2f);
    right.Cerp(toTireAngles[1], 0.2f);

    Vector3 forward = GetForward();
    Vector3 speedVector = speed;
    float realAccelFactor = 0.f;
    float realMassFactor = kartMassFactor;
    bool inPlaceDrift = false;

    
    if ((pressedKeys & (unsigned int)KEY_B) && canDrive) {
        realAccelFactor = engineAccelerationFactor * backwardsAccelerationRatioFactor;
    } else if ((pressedKeys & (unsigned int)KEY_A) && canDrive) {
        realAccelFactor = engineAccelerationFactor;
        realMassFactor *= 1.2f;
    }

    if ((pressedKeys & (unsigned int)KEY_B) && (pressedKeys & (unsigned int)KEY_A) && canDrive && speed.Magnitude() < inPlaceStartMinSpeed) {
        inPlaceDrift = true;
        speed = Vector3(0.f, 0.f, 0.f);
    }

    isStandStill = inPlaceDrift;

    Vector3 engineForce = forward * realAccelFactor;
    float realWheelResistanceFactor = wheelResistanceFactor + (1.f - collisionSpeedMult) * offroadFactor;
    Vector3 tireResistance = speedVector * -(realWheelResistanceFactor + (abs(right.AsDegrees()) / maxWheelTurnAngle) * turnWheelFrictionFactor);
    Vector3 windResistance = speedVector * (-windResistanceFactor * speedVector.Magnitude());
    Vector3 totalAcceleration = (engineForce + tireResistance + windResistance) / realMassFactor;

    if (!inPlaceDrift) speed += (totalAcceleration * (elapsedMsec / 1000.f));

    Vector3 advanceNow;
    float wheelSpinAmount = 0.f;
    bool goingBackwards = false;
    turningAmount = 0.f;

    if (inPlaceDrift) {
        Angle rotateAngle = Angle::FromDegrees((right.AsDegrees() / maxWheelTurnAngle) * inPlaceTurnFactor);
        wheelSpinAmount = maxWheelSpinAmount;
        GetRotation().y += rotateAngle;
        newKartPosition = GetPosition();
    }
    else {
        Point speedPoint(0.f);
        Point maxPoint(maxTurnAmount);
        speedPoint.Cerp(maxPoint, speed.Magnitude() / turnBySpeedFactor);

        Angle rotateAngle = Angle::FromDegrees(right.AsDegrees() / maxWheelTurnAngle) * speedPoint.value;
        speed.Rotate(Angle3(Angle::Zero(), rotateAngle, Angle::Zero()));
        advanceNow = speed * (elapsedMsec / 1000.f);
        Angle angleWithFwd = forward.GetAngle(advanceNow);
        if (!isinf(angleWithFwd.AsDegrees()) && abs(angleWithFwd.AsDegrees()) > 90.f)
        {
            speed.Rotate(Angle3(Angle::Zero(), rotateAngle * -2.f, Angle::Zero()));
            advanceNow = advanceNow * -1.f;
            goingBackwards = true;
        }

        Angle kartAngle = rotateAngle * -2.f;
        float absoluteAngle = abs(kartAngle.AsDegrees()) / 3.5f;
        kartObj->GetRotation().z = kartAngle;
        driverObj->GetRotation().z = kartObj->GetRotation().z;
        kartObj->GetPosition().y += absoluteAngle;
        driverObj->GetPosition().y += absoluteAngle;
        wheelObjs[0]->GetRotation().y = rotateAngle * maxWheelTurnAngle + defaultWheelRotations[0].y;
        wheelObjs[1]->GetRotation().y = rotateAngle * maxWheelTurnAngle + defaultWheelRotations[1].y;

        turningAmount = fabsf(rotateAngle.AsRadians() / 0.0132f);

        Angle newAngle = Vector3(0.f, 0.f, -1.f).GetAngle(advanceNow);
        if (!isinf(newAngle.AsRadians()) && abs(advanceNow.GetAngle(forward).AsDegrees()) < 90.f)
            GetRotation().y = newAngle;

        newKartPosition = GetPosition() + (advanceNow * (goingBackwards ? -1.f : 1.f));
        wheelSpinAmount = (newKartPosition - GetPosition()).Magnitude() * wheelSpinFactor * (goingBackwards ? -1.f : 1.f);
    }

    if (abs(wheelSpinAmount) > maxWheelSpinAmount)
        wheelSpinAmount = maxWheelSpinAmount * copysign(1.f, wheelSpinAmount);
    for (int i = 0; i < 4; i++) {
        wheelObjs[i]->GetPreRotation().x += Angle::FromDegrees(wheelSpinAmount * ((i & 1) ? -1.f : 1.f));
    }

    if (pressedKeys & (unsigned int)KEY_X)
        cameraRearView = cameraRearViewMultiplyFactor;
    else
        cameraRearView = 1.f;

    CalcCamera();
    CalcCollision(newKartPosition, goingBackwards);
    engine->Calc(GetRealSpeedFactor(true));
    CalcProgress();
    // --- Sound --- //
    CalcSounds();
    // ------------- //
    prevPressedKeys = pressedKeys;
}

void Kart::KeyPress(int key)
{
    pressedKeys |= (unsigned int)key;
}

void Kart::KeyRelease(int key)
{
    pressedKeys &= ~(unsigned int)key;
}

void Kart::CirclePadState(s16 xVal, s16 yVal)
{
    xCirclePad = xVal / 156.f;
    yCirclePad = yVal / 156.f;
}

void Kart::EnableDrive(bool enable) {
    canDrive = enable;
}

int Kart::GetLap() {
    return currentLap;
}

void Kart::StopSounds() {
    standStillSound->SetMasterVolume(0.f);
    sandSound->SetMasterVolume(0.f);
    grassSound->SetMasterVolume(0.f);
    turningSound->SetMasterVolume(0.f);
    wallHitSound->SetMasterVolume(0.f);
    engine->Terminate();
}

float Kart::GetRealSpeedFactor(bool includeBackwards)
{
    float calcAmount = (backAdvanceAmount != 0 && includeBackwards) ? backAdvanceAmount : advancedAmount;
    return (calcAmount / (16.666f / 1000.f)) * realSpeedFactor;
}

Angle Kart::GetSpeedometerAngle()
{
    float angle = GetRealSpeedFactor(false) * (9.f/8.f);
    if (angle > 90.f)
        angle = 90.f;
    return Angle::FromDegrees(angle);
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

Obj* Kart::GetKartObj()
{
    return kartObj;
}

Obj** Kart::GetWheelObjs()
{
    return wheelObjs;
}

unsigned int Kart::KeysJustPressed()
{
    return (pressedKeys ^ prevPressedKeys) & pressedKeys;
}

void Kart::CalcCollision(Vector3 newKartPosition, bool goingBackwards)
{
    float slowestGround = 1.f;
    Vector3 advance = newKartPosition - GetPosition();

    CollisionResult col = collision->CheckSphere(newKartPosition + Vector3(0.f, 2.f, 0.f), 9.f);
    for (u32 j = 0; j < col.length; j++) {
        Collision::KCLValueProperties val(col.prisms[j]->attribute);
        if (val.checkpointID > -1 && val.checkpointID < 4) {
            checkpoint = val.checkpointID;
        }
        if (val.roadType >= 0 && val.roadType != currColType && !lastColChangeFrames) {
            currColType = val.roadType;
            lastColChangeFrames = 10;
        }
        if (val.speedMultiplier < slowestGround)
            slowestGround = val.speedMultiplier;
        if (val.isWall) {
            if (!prevHitWallFrames) {
                justHitWall = true;
                prevHitWallFrames = 20;
            }
            Vector3 wallNormal = collision->GetNormal(col.prisms[j]->fNrmIdx, col.serverID[j]) * -1.f;
            wallNormal.Normalize();
            if ((speed).Dot(wallNormal) < 0) continue;
            float dotVal = wallNormal.Dot(advance);
            if (dotVal > 0) {
                advance -= wallNormal * dotVal;
                speed *= (12.f - dotVal) / 12.f;
            }
        }
    }

    isInAsphalt = slowestGround == 1.f;
    
    newKartPosition = GetPosition() + advance;
    advancedAmount = (newKartPosition - GetPosition()).Magnitude();
    backAdvanceAmount = 0.f;
    if (abs((newKartPosition - GetPosition()).GetAngle(GetForward()).AsRadians()) > Angle::DegreesToRadians(90.f) || goingBackwards) {
        backAdvanceAmount = -advancedAmount;
        advancedAmount = 0.f;
    }
    GetPosition() = newKartPosition;
    collisionSpeedMult = slowestGround;
}

void Kart::CalcProgress() {
    if (checkpoint != prevCheckpoint) {

        float integ;
        float fract = modff(lapProgress, &integ);
        
        float newFract = checkpoint / 4.f;
        if (fract == 0.75f && newFract == 0.f && !badLap) { // New lap
            integ++;
        } else if (fract == 0.f && newFract == 0.75f) {
            badLap = true;
        } else if (fract == 0.f && newFract == 0.25f) {
            badLap = false;
        }

        fract = newFract;
        lapProgress = integ + fract;
        currentLap = integ;
        prevCheckpoint = checkpoint;   
    }
}

// --- Sound --- //
void Kart::CalcSounds() {
    if (isStandStill && !wasStandStill && isInAsphalt) {
        standStillSound->EnsurePlaying();
        standStillSound->SetVolume(1.f);
    }
    if (!isStandStill && wasStandStill) {
        standStillSound->SetTargetVolume(0.f, 10);
        standStillSound->SetTargetStop(10);
    }
    if (currColType != prevColType) {
        if (currColType == 0) { // Road
            sandSound->SetTargetVolume(0.f, 10);
            sandSound->SetTargetStop(10);
            grassSound->SetTargetVolume(0.f, 10);
            grassSound->SetTargetStop(10);
        } else if (currColType == 0x5) { // Offroad
            sandSound->EnsurePlaying();
            grassSound->SetTargetVolume(0.f, 10);
            grassSound->SetTargetStop(10);
        } else if (currColType == 0x6) { // Heavy offroad
            grassSound->EnsurePlaying();
            sandSound->SetTargetVolume(0.f, 10);
            sandSound->SetTargetStop(10);
        }

    }
    float realSpeedFactor = fabsf(GetRealSpeedFactor(true));
    float offroadVol = std::min(realSpeedFactor / 20.f, 1.f);
    if (currColType == 0x5) {
        sandSound->SetVolume(offroadVol);
    } else if (currColType == 0x6) {
        grassSound->SetVolume(offroadVol);
    }

    if (turningAmount > 0.75f && isTurningStopped && isInAsphalt && !isStandStill) {
        turningSound->SetTargetVolume(1.f, 0);
        turningSound->EnsurePlaying();
        isTurningStopped = false;
    } else if ((turningAmount <= 0.75f || !isInAsphalt || isStandStill) && !isTurningStopped) {
        turningSound->SetTargetVolume(0.f, 10);
        turningSound->SetTargetStop(10);
        isTurningStopped = true;
    }

    float turningBySpeedMult = GetRealSpeedFactor(false) / 60.f;
    turningSound->SetVolume(std::min(((std::max(turningAmount, 0.75f) - 0.75f) / 0.25f) * std::min(turningBySpeedMult * turningBySpeedMult, 1.f), 1.f));

    if (justHitWall && prevRealSpeed - realSpeedFactor > 5.f) {
        wallHitSound->EnsurePlaying();
    }
        

    standStillSound->Tick();
    turningSound->Tick();
    sandSound->Tick();
    grassSound->Tick();
    wallHitSound->Tick();

    wasStandStill = isStandStill;
    prevColType = currColType;
    if (!prevRealSpeedFrames) {
        prevRealSpeed = realSpeedFactor;
        prevRealSpeedFrames = 5;
    }    
    justHitWall = false;
    if (lastColChangeFrames) lastColChangeFrames--;
    if (prevHitWallFrames) prevHitWallFrames--;
    if (prevRealSpeedFrames) prevRealSpeedFrames--;
}
// ------------- //