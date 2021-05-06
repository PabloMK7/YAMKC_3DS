#pragma once
#include "Texture.hpp"
#include "Plane.hpp"
#include "Vector.hpp"

class Speedometer
{
public:
	Speedometer();
	~Speedometer();

	void SetNeedleAngle(const Angle& angle);

	void Draw();

private:
	Plane meterPlane;
	Plane needlePlane;

	static const float screenCover;
	static const float screenMargin;
	static const Vector3 needleOffset;
};
