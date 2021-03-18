#pragma once

#include "Vector.hpp"
#include "Graphics.hpp"
#include "Material.hpp"

class Plane
{
public:
	Plane();
	~Plane();

	Vector3& GetPosition();
	Angle3& GetRotation();
	Vector3& GetScale();
	Material& GetMaterial();

	void Draw();

private:
	Material mat;

	Vector3 position;
	Angle3 rotation;
	Vector3 scale;
};