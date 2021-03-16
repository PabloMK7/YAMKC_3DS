#pragma once

#include "Vector.hpp"

class Plane
{
public:
	Plane();
	~Plane();

	Vector3& GetPosition();
	Angle3& GetRotation();
	Vector3& GetScale();

	void Draw();

private:
	Vector3 vertices[4];
	Vector2 uvCoords[4];

	Vector3 position;
	Angle3 rotation;
	Vector3 scale = Vector3(1.f, 1.f, 1.f);
};