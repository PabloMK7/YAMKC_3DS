#include <iostream>
#include <functional>
#include <string>
#include "Color.hpp"
#include "Obj.hpp"
#include "Kart.hpp"
#include "Lamp.hpp"
#include "Speedometer.hpp"
#include "Text.hpp"
#include "Chronometer.hpp"
#include "BottomMap.hpp"
#include "RaceEngine.hpp"
#include "CountdownDisplay.hpp"
#include "LapCounter.hpp"
#include "EndingDisplay.hpp"
#include <ctime>

// Constantes FPS
#define FPS 60
#define SPF 1.f / FPS

int g_renderMode = 0;

C3D_RenderTarget* targetLeft;
C3D_RenderTarget* targetRight;
C3D_RenderTarget* targetBottom;
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_X))

Obj* courseModel = nullptr;
Obj* skyboxModel = nullptr;
Kart* playerKart = nullptr;
Collision* collision = nullptr;
Speedometer* speedMeter = nullptr;
Chronometer* chrono = nullptr;
BottomMap* bottomMap = nullptr;
RaceEngine* raceEngine = nullptr;
CountdownDisplay* countdown = nullptr;
LapCounter* lapCounter = nullptr;
EndingDisplay* endingDisplay = nullptr;

#define LAMP_AMOUNT 4
Lamp* courseLamps[LAMP_AMOUNT];
Vector3 lampPositions[LAMP_AMOUNT] = {
    Vector3(-1211.054f, -0.3f, -788.372f),
    Vector3(-2655.251f, -0.3f, 539.328f),
    Vector3(-1434.451f, -0.3f, 588.533f),
    Vector3(108.654f, -0.3f, 508.025f)
};
Angle3 lampRotations[LAMP_AMOUNT] = {
    Angle3(Angle::Zero(), Angle::Zero(), Angle::Zero()),
    Angle3(Angle::Zero(), Angle::Zero(), Angle::Zero()),
    Angle3(Angle::Zero(), Angle::Zero(), Angle::Zero()),
    Angle3(Angle::Zero(), Angle::Zero(), Angle::Zero())
};

void sceneTopRender(C3D_RenderTarget* target, float iod)
{
    if (g_renderMode == 0) {

        playerKart->UpdateViewPort(iod);
        playerKart->UpdateCamera();

        skyboxModel->Draw();
        courseModel->Draw();
        for (int i = 0; i < LAMP_AMOUNT; i++) {
            courseLamps[i]->Draw();
        }
        playerKart->Draw();

        Graphics::StartUIDraw(target);

        speedMeter->Draw();
        chrono->Draw();
        countdown->Draw();
    } else if (g_renderMode == 1) {
        Graphics::StartUIDraw(target);
        C3D_RenderTargetClear(target, C3D_CLEAR_COLOR, 0, 0);
        endingDisplay->DrawTop();
    }
    raceEngine->Draw();
}

void sceneBottomRender(C3D_RenderTarget* target)
{
    if (g_renderMode == 0) {
        Graphics::StartUIDraw(target);
        bottomMap->Draw(playerKart->GetPosition(), playerKart->GetRotation().y);
        lapCounter->Draw();
    } else if (g_renderMode == 1) {
        Graphics::StartUIDraw(target);
        C3D_RenderTargetClear(target, C3D_CLEAR_COLOR, 0, 0);
        endingDisplay->DrawBottom();
    }
    raceEngine->Draw();
}

void sceneCalc() {

    if (g_renderMode == 0) {
        circlePosition pos;
        playerKart->KeyPress(hidKeysDown());
        playerKart->KeyRelease(hidKeysUp());
        hidCircleRead(&pos);
        playerKart->CirclePadState(pos.dx, pos.dy);
        playerKart->Calc(SPF * 1000.f);
        speedMeter->SetNeedleAngle(playerKart->GetSpeedometerAngle());
        chrono->Tick();
        countdown->Tick();
    } else if (g_renderMode == 1) {
        // Nothing to calc.
    }
    
    raceEngine->Calc();
}
 
void resourceInit() {
    courseModel = new Obj("romfs:/course_model/course_model.obj");
    skyboxModel = new Obj("romfs:/course_model/skybox_model_day.obj");

    collision = new Collision();

    playerKart = new Kart("romfs:/driver/kart.obj", "romfs:/driver/wheel.obj", "romfs:/driver/driver.obj", "romfs:/driver/shadow.obj", collision);
    playerKart->GetScale() = Vector3(1.35f, 1.35f, 1.35f);

    speedMeter = new Speedometer();
    chrono = new Chronometer();
    countdown = new CountdownDisplay();
    lapCounter = new LapCounter();
    bottomMap = new BottomMap();
    raceEngine = new RaceEngine(playerKart, chrono, countdown, lapCounter);
    endingDisplay = new EndingDisplay();

    collision->AddResource("romfs:/collision/collision.kcl", Vector3(0.f, 0.f, 0.f), Angle3());    

    for (int i = 0; i < LAMP_AMOUNT; i++) {
        Vector3 scale = Vector3(1.f, 1.f, 1.f);
        courseLamps[i] = new Lamp(lampPositions[i], lampRotations[i], scale, *collision);
    }
}

// Destrucci�n de los objetos.
void resourceExit() {
    for (int i = 0; i < LAMP_AMOUNT; i++) {
        if (courseLamps[i]) delete courseLamps[i];
    }
    if (courseModel) delete courseModel;
    if (skyboxModel) delete skyboxModel;
    if (playerKart) delete playerKart;
    if (speedMeter) delete speedMeter;
    if (collision) delete collision;
    if (chrono) delete chrono;
    if (bottomMap) delete bottomMap;
    if (raceEngine) delete raceEngine;
    if (countdown) delete countdown;
    if (endingDisplay) delete endingDisplay;
}

int main(int argc, char** argv)
{
    gfxInitDefault();
    gfxSet3D(true);
    romfsInit();
    ndspInit();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    //consoleInit(GFX_BOTTOM, NULL);
    
    targetLeft = C3D_RenderTargetCreate(240 * 2, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8); // Twice the size needed for anti-aliasing
    targetRight = C3D_RenderTargetCreate(240 * 2, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    targetBottom = C3D_RenderTargetCreate(240 * 2, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(targetLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
    C3D_RenderTargetSetOutput(targetRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
    C3D_RenderTargetSetOutput(targetBottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    Graphics::SceneInit();
    Text::Init();

    resourceInit();
    while (aptMainLoop())
    {
		float iod = osGet3DSliderState();

        hidScanInput();
        sceneCalc();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C3D_RenderTargetClear(targetLeft, C3D_CLEAR_DEPTH, 0, 0);
        C3D_FrameDrawOn(targetLeft);
        sceneTopRender(targetLeft, -iod);
        if (iod > 0.0f)
        {
            C3D_RenderTargetClear(targetRight, C3D_CLEAR_DEPTH, 0, 0);
            C3D_FrameDrawOn(targetRight);
            sceneTopRender(targetRight, iod);
        }
        C3D_RenderTargetClear(targetBottom, C3D_CLEAR_DEPTH, 0, 0);
        C3D_FrameDrawOn(targetBottom);
        sceneBottomRender(targetBottom);
        C3D_FrameEnd(0);
        if (hidKeysDown() & KEY_START)
            break;
    }

    resourceExit();
    Text::Terminate();
    Graphics::SceneExit();

    C3D_Fini();
    ndspExit();
    romfsExit();
    return 0;
}