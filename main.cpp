#include <iostream>
#include <functional>
#include <string>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include "Color.hpp"
#include "Obj.hpp"
#include "Kart.hpp"
#include "Light.hpp"
#include <ctime>
#include <freeimage/FreeImage.h>
#pragma comment(lib, "FreeImage.lib")

// Constantes FPS
#define FPS 60
#define SPF 1.f / FPS

Obj* courseModel = nullptr;
Obj* skyboxModels[2] = { nullptr, nullptr };
Kart* playerKart = nullptr;
Collision* collision = nullptr;
Light* dayLight = nullptr;
Light* nightLight = nullptr;

bool isDayMode = true;
bool isFogMode = false;
Color dayGlobalAmbientColor = Color(0.85f, 0.85f, 0.85f);
Color nightGlobalAmbientColor = Color(0.2f, 0.2f, 0.2f);

void setLightFogMode(bool isDay) {
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
        if (isFogMode) skyboxModels[1]->GetMaterial("mat_moon").GetColor(Obj::Material::ColorType::DIFFUSE) = Color(0.25f, 0.25f, 0.25f);
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
    prevDayMode = isDay;
}

// Inicialización de los objetos.
void init() {
    // Inicializar la semilla del generador aleatorio con el tiempo actual.
    std::srand((unsigned int)std::time(NULL));

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    dayLight = new Light(GL_LIGHT0);
    nightLight = new Light(GL_LIGHT0);

    dayLight->SetPosition(Vector3(0.001f, 1.001f, 0.001f));
    dayLight->SetColor(Light::ColorType::AMBIENT, Color(0.9f, 0.9f, 0.9f));
    dayLight->SetColor(Light::ColorType::DIFFUSE, Color(0.9f, 0.9f, 0.9f));

    nightLight->SetPosition(Vector3(-1.f, 1.f, 0.01f));
    nightLight->SetColor(Light::ColorType::AMBIENT, Color(0.7f, 0.8f, 0.9f));
    nightLight->SetColor(Light::ColorType::DIFFUSE, Color(0.7f, 0.8f, 0.9f));

    try {
        courseModel = new Obj("data/course_model/course_model.obj");
        skyboxModels[0] = new Obj("data/course_model/skybox_model_day.obj");
        skyboxModels[1] = new Obj("data/course_model/skybox_model_night.obj");
    }
    catch (const char* msg) {
        std::cout << std::string("Failed to load course model: ") + msg << std::endl;
    }
    collision = new Collision("data/collision/map.png", "data/collision/UIMapPos.bin");
    playerKart = new Kart("data/driver/kart.obj", "data/driver/wheel.obj", "data/driver/driver.obj", "data/driver/shadow.obj", collision);
    playerKart->GetScale() = Vector3(1.2f, 1.2f, 1.2f);
    playerKart->GetDriverObj()->GetMaterial("mat_driver_body").SetTextureRepeatMode(Obj::Material::TextureDirection::DIR_S, GL_MIRRORED_REPEAT);
    playerKart->GetDriverObj()->GetMaterial("mat_driver_body").SetTextureRepeatMode(Obj::Material::TextureDirection::DIR_T, GL_MIRRORED_REPEAT);
    playerKart->GetDriverObj()->GetMaterial("mat_driver_eyes").SetTextureRepeatMode(Obj::Material::TextureDirection::DIR_S, GL_MIRRORED_REPEAT);
    skyboxModels[1]->GetMaterial("mat_moon").ForceDisableFog(true);

    setLightFogMode(isDayMode);
}

// Destrucción de los objetos.
void exit() {
    if (dayLight) delete dayLight;
    if (nightLight) delete nightLight;
    if (courseModel) delete courseModel;
    if (skyboxModels[0]) delete skyboxModels[0];
    if (skyboxModels[1]) delete skyboxModels[1];
    if (playerKart) delete playerKart;
    if (collision) delete collision;
}

// Distancia de la camara al origen de coordenadas.
#define CAMERADIST 5.f
// Tamaño de la esfera unidad.
#define UNITRADIUS 1.f

// Parámetros para la rotación controlada por el usuario.
float maxRot = 30;
#define ROTSPEED 1
int xPressed;
int yPressed;

// Dibujado de los objetos.
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Light::GlobalAmbientColor = (isDayMode) ? dayGlobalAmbientColor : nightGlobalAmbientColor;

    playerKart->UpdateCamera();

    if (isDayMode)
        dayLight->Update();
    else
        nightLight->Update();

    // Draw functions
    Light::GlobalDisable();
    if (!isFogMode) glDisable(GL_FOG);
    else glEnable(GL_FOG);
    if (isDayMode && skyboxModels[0]) skyboxModels[0]->Draw();
    else if (!isDayMode && skyboxModels[1]) skyboxModels[1]->Draw();

    Light::GlobalEnable();
    if (!isFogMode) glEnable(GL_FOG);
    if (courseModel) courseModel->Draw();
    if (playerKart) playerKart->Draw();

    glutSwapBuffers();
}

unsigned int previousTime = 0;
unsigned int currentTime = 0;

// Callback ejecutada cada x mseg.
void onTimer(int val) {

    currentTime = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc((unsigned int)(SPF * 1000), onTimer, 0);
    unsigned int elapsed = currentTime - previousTime;
    previousTime = currentTime;

    // Llamada a la función calc con el tiempo transcurrido a cada objeto.
    // De esta forma, cada objeto es responsable de actualizar sus parámetros.
    playerKart->Calc(elapsed);

    glutSetWindowTitle(("YAMKC - FPS: " + std::to_string((1.f / (float)elapsed) * 1000.f)).c_str());

    glutPostRedisplay();
}

// Función de cambio de tamaño de la pantalla.
void reshape(GLint w, GLint h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    playerKart->UpdateViewPort(w, h);
}

// Función ejecutada cuando se pulsa una tecla especial.
void pressSpecialKey(int key, int x, int y)
{
    switch (key)
    {
    case 100:  playerKart->KeyPress(Kart::Key::KEY_Left); break; // Izquierda
    case 102:  playerKart->KeyPress(Kart::Key::KEY_Right); break; // Derecha
    case 101:  playerKart->KeyPress(Kart::Key::KEY_A); break; // Arriba
    case 103:  playerKart->KeyPress(Kart::Key::KEY_B); break; // Abajo
    }
}

// Función ejecutada cuando se pulsa una tecla.
void pressKey(unsigned char key, int x, int y)
{
    switch (key)
    {
    case  'r':
    case  'R':  playerKart->KeyPress(Kart::Key::KEY_X); break;
    case  'l':
    case  'L':  setLightFogMode(!isDayMode); break;
    case  'n':
    case  'N':  isFogMode = !isFogMode; setLightFogMode(isDayMode); break;
    }
}

// Función ejecutada cuando se suelta una tecla especial.
void releaseSpecialKey(int key, int x, int y)
{
    switch (key)
    {
    case 100:  playerKart->KeyRelease(Kart::Key::KEY_Left); break; // Izquierda
    case 102:  playerKart->KeyRelease(Kart::Key::KEY_Right); break; // Derecha
    case 101:  playerKart->KeyRelease(Kart::Key::KEY_A);  break; // Arriba
    case 103:  playerKart->KeyRelease(Kart::Key::KEY_B);  break; // Abajo
    }
}

void releaseKey(unsigned char key, int x, int y)
{
    switch (key)
    {
    case  'r':
    case  'R':  playerKart->KeyRelease(Kart::Key::KEY_X); break;
    }
}

int main(int argc, char** argv)
{
    FreeImage_Initialise();
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutInitWindowSize((int)(600 * 1.666666), 600);
    glutCreateWindow("YAMKC");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIgnoreKeyRepeat(1); // Ignorar las pulsaciones automáticas al mantener pulsadas las teclas.
    glutSpecialFunc(pressSpecialKey);
    glutKeyboardFunc(pressKey);
    glutSpecialUpFunc(releaseSpecialKey);
    glutKeyboardUpFunc(releaseKey);

    previousTime = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc((unsigned int)(SPF), onTimer, 0);

    glutMainLoop();

    exit();
    FreeImage_DeInitialise();
    return 0;
}