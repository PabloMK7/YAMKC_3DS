#include <iostream>
#include <functional>
#include <string>
#include "Color.hpp"
#include "Obj.hpp"
#include "Kart.hpp"
#include "Light.hpp"
#include "Lamp.hpp"
#include "Speedometer.hpp"
#include <ctime>

// Constantes FPS
#define FPS 60
#define SPF 1.f / FPS

bool hudEnabled = true;
bool birdsView = false;

int windowW = 400;
int windowH = 240;

C3D_RenderTarget* targetLeft;
C3D_RenderTarget* targetRight;
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_X))

Obj* courseModel = nullptr;
Obj* skyboxModels[2] = { nullptr, nullptr };
Kart* playerKart = nullptr;
Collision* collision = nullptr;
Speedometer* speedMeter = nullptr;

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
    Angle3(Angle::Zero(), Angle::FromDegrees(32.724f), Angle::Zero()),
    Angle3(Angle::Zero(), Angle::FromDegrees(-16.907f), Angle::Zero())
};

bool isDayMode = true;
bool isFogMode = false;
Color dayGlobalAmbientColor = Color(0.85f, 0.85f, 0.85f);
Color nightGlobalAmbientColor = Color(0.2f, 0.2f, 0.2f);

void setLightFogMode(bool isDay) {
    /*
    static bool prevDayMode = isDayMode;
    float scale;

    isDayMode = isDay;

    glFogf(GL_FOG_DENSITY, isFogMode ? 0.001f : 0.00005f);
    if (isDay)
    {
        GLfloat fogColor[4] = { 1.f, 1.f, 1.f, 1.0f };
        glFogfv(GL_FOG_COLOR, fogColor);

        glClearColor(1.f, 1.f, 1.f, 1.f);
    }
    else
    {
        GLfloat fogColor[4] = { 0.1f, 0.15f, 0.17f, 1.f };
        glFogfv(GL_FOG_COLOR, fogColor);
        if (isFogMode) skyboxModels[1]->GetMaterial("mat_moon").GetColor(Obj::Material::ColorType::DIFFUSE) = Color(0.35f, 0.35f, 0.35f);
        else skyboxModels[1]->GetMaterial("mat_moon").GetColor(Obj::Material::ColorType::DIFFUSE) = Color(1.f, 1.f, 1.f);

        glClearColor(0.f, 0.f, 0.f, 1.f);
    }


    if (!prevDayMode && isDay)
        scale = 1.f / 0.55f;
    else if (prevDayMode && !isDay)
        scale = 0.55f;
    else
        scale = 1.f;

    for (auto& mat : courseModel->Materials()) {
        mat.GetColor(Obj::Material::ColorType::AMBIENT).Scale(scale, false);
        mat.GetColor(Obj::Material::ColorType::DIFFUSE).Scale(scale, false);
    }
    for (auto& mat : playerKart->GetDriverObj()->Materials()) {
        mat.GetColor(Obj::Material::ColorType::AMBIENT).Scale(scale, false);
        mat.GetColor(Obj::Material::ColorType::DIFFUSE).Scale(scale, false);
    }
    for (auto& mat : playerKart->GetKartObj()->Materials()) {
        mat.GetColor(Obj::Material::ColorType::AMBIENT).Scale(scale, false);
        mat.GetColor(Obj::Material::ColorType::DIFFUSE).Scale(scale, false);
    }
    for (int i = 0; i < 4; i++) {
        for (auto& mat : playerKart->GetWheelObjs()[i]->Materials()) {
            mat.GetColor(Obj::Material::ColorType::AMBIENT).Scale(scale, false);
            mat.GetColor(Obj::Material::ColorType::DIFFUSE).Scale(scale, false);
        }
    }

    for (int i = 0; i < LAMP_AMOUNT; i++) {
        courseLamps[i]->setDayMode(isDay);
    }

    prevDayMode = isDay;
    */
}

// Dibujado de los objetos.
void sceneRender(C3D_RenderTarget* target, float iod)
{
    playerKart->UpdateViewPort(windowW, windowH, iod);

    playerKart->UpdateCamera();

    if (isDayMode && skyboxModels[0] && !birdsView) skyboxModels[0]->Draw();
    else if (!isDayMode && skyboxModels[1] && !birdsView) skyboxModels[1]->Draw();

    if (courseModel) courseModel->Draw();

    for (int i = 0; i < LAMP_AMOUNT; i++) {
        courseLamps[i]->Draw();
    }

    if (playerKart) playerKart->Draw();

    if (hudEnabled) speedMeter->Draw(target, windowW, windowH);
}

unsigned int previousTime = 0;
unsigned int currentTime = 0;

// Callback ejecutada cada x mseg.
void sceneCalc() {

    static int frameCount = 0;
    static int timePassed = 0;

    currentTime = previousTime + (SPF) * 1000.f;
    unsigned int elapsed = currentTime - previousTime;
    previousTime = currentTime;

    // Llamada a la función calc con el tiempo transcurrido a cada objeto.
    // De esta forma, cada objeto es responsable de actualizar sus par�metros.
    playerKart->Calc(elapsed);
    speedMeter->SetNeedleAngle(playerKart->GetSpeedometerAngle(elapsed));

    if (frameCount >= FPS / 4) {
        timePassed = 0;
        frameCount = 0;
    }
    else {
        timePassed += elapsed;
        frameCount++;
    }
}

// Inicializaci�n de los objetos. 
void resourceInit() {
    std::string currRes = "course_model";
    std::cout << currRes << std::endl;

    courseModel = new Obj("romfs:/course_model/course_model.obj");
    currRes = "sky_box_day";
    std::cout << currRes << std::endl;
    skyboxModels[0] = new Obj("romfs:/course_model/skybox_model_day.obj");
    currRes = "sky_box_night";
    std::cout << currRes << std::endl;
    skyboxModels[1] = new Obj("romfs:/course_model/skybox_model_night.obj");
    currRes = "collision";
    std::cout << currRes << std::endl;
    collision = new Collision("romfs:/collision/collision.kcl");
    currRes = "kart";
    std::cout << currRes << std::endl;
    playerKart = new Kart("romfs:/driver/kart.obj", "romfs:/driver/wheel.obj", "romfs:/driver/driver.obj", "romfs:/driver/shadow.obj", collision);

    for (int i = 0; i < LAMP_AMOUNT; i++) {
        Vector3 scale = Vector3(1.f, 1.f, 1.f);
        courseLamps[i] = new Lamp(lampPositions[i], lampRotations[i], scale);
    }

    speedMeter = new Speedometer();

    playerKart->GetScale() = Vector3(1.2f, 1.2f, 1.2f);
    playerKart->GetDriverObj()->GetMaterial("mat_driver_body").SetTextureWrapMode(GPU_MIRRORED_REPEAT, GPU_MIRRORED_REPEAT);
    playerKart->GetDriverObj()->GetMaterial("mat_driver_eyes").SetTextureWrapMode(GPU_MIRRORED_REPEAT, GPU_REPEAT);
    //skyboxModels[1]->GetMaterial("mat_moon").ForceDisableFog(true);

    setLightFogMode(isDayMode);
}

// Destrucci�n de los objetos.
void exit() {
    for (int i = 0; i < LAMP_AMOUNT; i++) {
        if (courseLamps[i]) delete courseLamps[i];
    }
    if (courseModel) delete courseModel;
    if (skyboxModels[0]) delete skyboxModels[0];
    if (skyboxModels[1]) delete skyboxModels[1];
    if (playerKart) delete playerKart;
    if (speedMeter) delete speedMeter;
    if (collision) delete collision;
}


int main(int argc, char** argv)
{
    gfxInitDefault();
    gfxSet3D(true);
    romfsInit();
    ndspInit();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    consoleInit(GFX_BOTTOM, NULL);
    
    targetLeft = C3D_RenderTargetCreate(240 * 2, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8); // Twice the size needed for anti-aliasing
    targetRight = C3D_RenderTargetCreate(240 * 2, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(targetLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
    C3D_RenderTargetSetOutput(targetRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);

    Graphics::SceneInit();

    resourceInit();
    previousTime = 0;
    while (aptMainLoop())
    {
		float iod = osGet3DSliderState();

        hidScanInput();
        playerKart->KeyPress(hidKeysDown());
        playerKart->KeyRelease(hidKeysUp());
        sceneCalc();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C3D_RenderTargetClear(targetLeft, C3D_CLEAR_DEPTH, 0, 0);
        C3D_FrameDrawOn(targetLeft);
        sceneRender(targetLeft, -iod);
        if (iod > 0.0f)
        {
            C3D_RenderTargetClear(targetRight, C3D_CLEAR_DEPTH, 0, 0);
            C3D_FrameDrawOn(targetRight);
            sceneRender(targetRight, iod);
        }
        C3D_FrameEnd(0);
    }

    ndspExit();
    romfsExit();
    return 0;
}