#pragma once
// Clase que representa una posición en 3D
class Vector3
{
public:
    Vector3() {
        this->x = this->y = this->z = 0;
    }

    Vector3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void operator=(const Vector3& right)
    {
        this->x = right.x;
        this->y = right.y;
        this->z = right.z;
    }

    float x, y, z;
private:

};

class Vector2
{
public:
    Vector2() {
        this->x = this->y = 0;
    }
    Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    float x, y;
private:

};