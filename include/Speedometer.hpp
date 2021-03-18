#pragma once
#include "Texture.hpp"
#include "Plane.hpp"

class Speedometer
{
public:
	Speedometer();
	~Speedometer();

	void SetNeedleAngle(const Angle& angle);

	void Draw(C3D_RenderTarget* target, int w, int h);

private:
	Plane meterPlane;
	Plane needlePlane;

	static const float screenCover;
};
