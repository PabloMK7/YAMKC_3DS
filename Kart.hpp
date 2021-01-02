#pragma once
#include <string>
#include "Obj.hpp"

class Kart
{
public:
	Kart(std::string kartName, std::string wheelName, std::string driverName, std::string shadowName);
	~Kart();

	enum Key {
		KEY_A = (1 << 0),
		KEY_B = (1 << 1),
		KEY_X = (1 << 2),
		KEY_Left = (1 << 3),
		KEY_Right = (1 << 4)
	};

	void UpdateCamera();
	void UpdateViewPort(int w, int h);
	void Draw();
	void Calc(int elapsedMsec);
	void KeyPress(Key key);
	void KeyRelease(Key key);

	Vector3& GetPosition();
	Angle3& GetRotation();
	Vector3& GetScale();
	Vector3 GetForward();

	Obj* GetDriverObj();

private:
	Obj* kartObj;
	Obj* driverObj;
	Obj* wheelObjs[4];
	Obj* shadowObj;

	Vector3 position;
	Angle3 rotation;
	Vector3 scale = Vector3(1.f, 1.f, 1.f);

	static const Vector3 defaultWheelPositions[4];
	static const Angle3 defaultWheelRotations[4];

	static const Vector3 cameraOffset;
	static const Vector3 cameraLookAtOffset;
	static const float cameraFov;

	int totalElapsedTime = 0;
	unsigned int pressedKeys;
	unsigned int prevPressedKeys;

	int tiresRotateTimer;
	Angle fromTireAngles[2] = { Angle::Zero(), Angle::Zero() };
	Angle toTireAngles[2] = { Angle::Zero(), Angle::Zero() };

	float cameraRearView = 1.f;

	unsigned int KeysJustPressed();
};