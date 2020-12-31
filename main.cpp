#include <iostream>
#include <functional>
#include <string>
#include <GL\freeglut.h>
#include "Color.hpp"
#include "Obj.hpp"
#include <ctime>
#include <freeimage/FreeImage.h>
#pragma comment(lib, "FreeImage.lib")

// Constantes matematicas y FPS
#define PI 3.14159265358979323846f
#define FPS 60
#define SPF 1.f / FPS

Obj* courseModel = NULL;

// Inicialización de los objetos.
void init() {
    // Inicializar la semilla del generador aleatorio con el tiempo actual.
    std::srand((unsigned int)std::time(NULL));

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(1.f, 1.f, 1.f, 1.f);

    try {
        courseModel = new Obj("data/course_model/course_model.obj");
    }
    catch (const char* msg) {
        std::cout << std::string("Failed to load course model: ") + msg << std::endl;
    }
}

// Destrucción de los objetos.
void exit() {
    if (courseModel) delete courseModel;
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

    gluLookAt(0, 50, CAMERADIST, 0, 50, 0, 0, 1, 0);

    // Draw functions
    if (courseModel) courseModel->Draw();

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
    
    

    glutPostRedisplay();
}

static bool spacePressed = false;

// Función de cambio de tamaño de la pantalla.
void reshape(GLint w, GLint h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float ratio = (float)w / h;

    // Mantener el angulo de visión para que la esfera unidad toque los límites verticales.
    // https://i.imgur.com/79iq15m.png
    gluPerspective(std::asinf(UNITRADIUS/CAMERADIST) * 2.0 * (180.0 / PI), ratio, 20, 10000);
}

// Función ejecutada cuando se pulsa una tecla especial.
void pressSpecialKey(int key, int x, int y)
{
    switch (key)
    {
    case 100:  xPressed = 1; break; // Izquierda
    case 102:  xPressed = -1; break; // Derecha
    case 101:  yPressed = 1;  break; // Arriba
    case 103:  yPressed = -1;  break; // Abajo
    }
}

// Función ejecutada cuando se pulsa una tecla.
void pressKey(unsigned char key, int x, int y)
{
    switch (key)
    {
    case  ' ':  spacePressed = true; break;
    }
}

// Función ejecutada cuando se suelta una tecla especial.
void releaseSpecialKey(int key, int x, int y)
{
    switch (key)
    {
    case 100:  xPressed = 0; break; // Izquierda
    case 102:  xPressed = 0; break; // Derecha
    case 101:  yPressed = 0;  break; // Arriba
    case 103:  yPressed = 0;  break; // Abajo
    }
}

int main(int argc, char** argv)
{
    FreeImage_Initialise();
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("YAMKC");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIgnoreKeyRepeat(1); // Ignorar las pulsaciones automáticas al mantener pulsadas las teclas.
    glutSpecialFunc(pressSpecialKey);
    glutKeyboardFunc(pressKey);
    glutSpecialUpFunc(releaseSpecialKey);

    glutTimerFunc((unsigned int)(SPF), onTimer, 0);
    previousTime = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();

    exit();
    FreeImage_DeInitialise();
    return 0;
}