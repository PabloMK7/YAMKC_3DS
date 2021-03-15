#pragma once
#include "Texture.hpp"
#include "Plane.hpp"

class Speedometer
{
public:
	Speedometer();
	~Speedometer();

	void SetNeedleAngle(const Angle& angle);

	void Draw(GLint w, GLint h);

private:
	Plane meterPlane;
	Plane needlePlane;
	Texture* meterTex;
	Texture* needleTex;

	static const float screenCover;
};
