#pragma once
#include "Texture.hpp"
#include "Plane.hpp"

class Speedometer
{
public:
	Speedometer();
	~Speedometer();

	void SetNeedleAngle(const Angle& angle);

	void Draw(int w, int h);

private:
	Plane meterPlane;
	Plane needlePlane;
	Texture* meterTex;
	Texture* needleTex;

	static const float screenCover;
};
