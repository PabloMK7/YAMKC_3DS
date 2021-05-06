#pragma once
#include "Obj.hpp"
#include "Light.hpp"
#include <atomic>
#include "Collision.hpp"

class Lamp
{
public:
	Lamp(Vector3& pos, Angle3& rot, Vector3& scal, Collision& col); //GLenum light);
	~Lamp();
	void setDayMode(bool isDay);
	void EnableLight(bool enable);

	void UpdateLight();
	void Draw();

private:

	Vector3 position;
	Angle3 rotation;
	Vector3 scale;
	Obj* lampObject;
	//Light* lampLight;
	bool dayMode;
};
