#pragma once
#include <string>
#include "Obj.hpp"
#include "Collision.hpp"
#include "Sound.hpp"
#include "EngineSound.hpp"
class Kart
{
public:
	Kart(std::string kartName, std::string wheelName, std::string driverName, std::string shadowName, Collision* col);
	~Kart();

	void UpdateCamera();
	void Draw();
	void Calc(int elapsedMsec);
	void KeyPress(int key);
	void KeyRelease(int key);
	void CirclePadState(s16 xVal, s16 yVal);
	void EnableDrive(bool canDrive);

	int GetLap();
	void StopSounds();

	float GetRealSpeedFactor(bool includeBackwards);
	Angle GetSpeedometerAngle();

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
	Vector3 cameraLookAt;

	float advancedAmount;
	float backAdvanceAmount;

	static const Vector3 defaultWheelPositions[4];
	static const Angle3 defaultWheelRotations[4];

	static const int collisionSpherePrecision;
	static const float collisionSphereW;
	static const float collisionSphereH;

	static const Vector3 cameraOffset;
	static const Vector3 cameraLookAtOffset;
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
	static const float realSpeedFactor;

	int totalElapsedTime = 0;
	bool canDrive;
	int checkpoint = 0;
	int prevCheckpoint = 0;
	float lapProgress = 0.f;
	int currentLap = 0;
	bool badLap = false;

	unsigned int pressedKeys;
	unsigned int prevPressedKeys;
	float xCirclePad;
	float yCirclePad;

	int tiresRotateTimer;
	Angle fromTireAngles[2] = { Angle::Zero(), Angle::Zero() };
	Angle toTireAngles[2] = { Angle::Zero(), Angle::Zero() };

	float cameraRearView = 1.f;
	float oldCameraRearView = 1.f;

	float collisionSpeedMult = 1.f;

	void CalcCamera();
	unsigned int KeysJustPressed();
	void CalcCollision(Vector3 newKartPosition, bool goingBackwards);
	void CalcProgress();

	// --- Sound --- //
	EngineSound* engine;
	Sound* standStillSound;
	Sound* grassSound;
	Sound* sandSound;
	Sound* turningSound;
	Sound* wallHitSound;
	bool wasStandStill = false;
	bool isStandStill = false;
	bool isInAsphalt = true;
	int currColType = 0;
	int prevColType = 0;
	int lastColChangeFrames = 0;
	float turningAmount = 0.f;
	bool isTurningStopped = true;
	float prevRealSpeed = 0.f;
	int prevRealSpeedFrames = 0;
	int prevHitWallFrames = 0;
	bool justHitWall = false;
	void CalcSounds();
	// ------------- //
};