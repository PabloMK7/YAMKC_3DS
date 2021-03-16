#pragma once
#include <string>
#include "Obj.hpp"
#include "Collision.hpp"

class Kart
{
public:
	Kart(std::string kartName, std::string wheelName, std::string driverName, std::string shadowName, Collision* col);
	~Kart();

	enum class Key {
		KEY_A = (1 << 0),
		KEY_B = (1 << 1),
		KEY_X = (1 << 2),
		KEY_Left = (1 << 3),
		KEY_Right = (1 << 4)
	};

	void UpdateCamera(bool birdsView);
	void UpdateViewPort(int w, int h);
	void Draw();
	void Calc(int elapsedMsec);
	void KeyPress(Key key);
	void KeyRelease(Key key);

	Angle GetSpeedometerAngle(int elapsedMsec);

	Vector3& GetPosition();
	Angle3& GetRotation();
	Vector3& GetScale();
	Vector3 GetForward();

	Obj* GetDriverObj();
	Obj* GetKartObj();
	Obj** GetWheelObjs();

private:
	Obj* kartObj;
	Obj* driverObj;
	Obj* wheelObjs[4];
	Obj* shadowObj;
	Collision* collision;	

	Vector3 position;
	Vector3 speed;
	Angle3 rotation;
	Vector3 scale = Vector3(1.f, 1.f, 1.f);
	Vector3 currCameraPos;

	float advancedAmount;

	static const Vector3 defaultWheelPositions[4];
	static const Angle3 defaultWheelRotations[4];

	static const int collisionSpherePrecision;
	static const float collisionSphereW;
	static const float collisionSphereH;

	static const Vector3 cameraOffset;
	static const Vector3 cameraLookAtOffset;
	static const float cameraFov;
	static const float cameraRearViewMultiplyFactor;
	static const float cameraRotationCerpFactor;
	static const float cameraPositionCerpFactor;

	static const float engineAccelerationFactor;
	static const float backwardsAccelerationRatioFactor;
	static const float wheelResistanceFactor;
	static const float windResistanceFactor;
	static const float kartMassFactor;
	static const float maxWheelTurnAngle;
	static const float turnWheelFrictionFactor;
	static const float turnBySpeedFactor;
	static const float maxTurnAmount;
	static const float inPlaceStartMinSpeed;
	static const float inPlaceTurnFactor;
	static const float wheelSpinFactor;
	static const float maxWheelSpinAmount;
	static const float kartBounceFactor;
	static const float kartGrowFactor;
	static const float offroadFactor;
	static const float speedometerSpeedFactor;

	int totalElapsedTime = 0;
	unsigned int pressedKeys;
	unsigned int prevPressedKeys;

	int tiresRotateTimer;
	Angle fromTireAngles[2] = { Angle::Zero(), Angle::Zero() };
	Angle toTireAngles[2] = { Angle::Zero(), Angle::Zero() };

	float cameraRearView = 1.f;

	unsigned int KeysJustPressed();
	Vector3 CalcCollision(const Vector3& advancePos);
};