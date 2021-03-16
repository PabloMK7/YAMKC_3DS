#include "Plane.hpp"

Plane::Plane()
{
    vertices[0] = Vector3(-0.5f, -0.5f, 0.f);
    vertices[1] = Vector3(-0.5f, 0.5f, 0.f);
    vertices[2] = Vector3(0.5f, 0.5f, 0.f);
    vertices[3] = Vector3(0.5f, -0.5f, 0.f);

    uvCoords[0] = Vector2(0.f, -1.f);
    uvCoords[1] = Vector2(0.f, 0.f);
    uvCoords[2] = Vector2(1.f, 0.f);
    uvCoords[3] = Vector2(1.f, -1.f);
}

Plane::~Plane() {}

Vector3& Plane::GetPosition()
{
    return position;
}

Angle3& Plane::GetRotation()
{
    return rotation;
}

Vector3& Plane::GetScale()
{
    return scale;
}

void Plane::Draw()
{
    /*
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.x.AsDegrees(), 1.f, 0.f, 0.f);
    glRotatef(rotation.y.AsDegrees(), 0.f, 1.f, 0.f);
    glRotatef(rotation.z.AsDegrees(), 0.f, 0.f, 1.f);
    glScalef(scale.x, scale.y, scale.z);

    glBegin(GL_TRIANGLE_FAN);

    glColor4f(1.f, 1.f, 1.f, 1.f);

    for (int i = 0; i < 4; i++) {
        glTexCoord2f(uvCoords[i].x, uvCoords[i].y);
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
    }

    glEnd();

    glPopMatrix();
    */
}
