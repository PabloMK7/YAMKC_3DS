#pragma once
#include <GL\freeglut.h>
#include <functional>

// Representa una forma que se puede dibujar, hace uso de las listas de dibujo de opengl al tomar una función lambda en el constructor.
// Cada objeto geometrico en este programa contiene un objeto Shape para facilitar el dibujo.
// En un primer momento pensé de que fuera singleton para cada clase, pero para facilitar el uso de colores decidí no hacerlo.
class Shape
{
public:
    // Toma un lambda como argumento para ejecutar las órdenes que se compilarán y guardarán en la lista
    Shape(const std::function <void(void)>& openGlList) {
        list = glGenLists(1);
        glNewList(list, GL_COMPILE);
        openGlList();
        glEndList();
    }
    // Liberar la lista en el caso de destruir el objeto para evitar que se llene, importante!
    ~Shape() {
        glDeleteLists(list, 1);
    }
    void Draw() {
        glCallList(list);
    }
private:
    Shape();
    GLuint list;
};