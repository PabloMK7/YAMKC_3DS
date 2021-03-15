#pragma once
/*
#include "Color.hpp"
#include "Vector.hpp"

class Light
{
public:
	enum class LightType
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};
	enum class ColorType
	{
		AMBIENT,
		DIFFUSE,
		SPECULAR
	};
	Light(GLenum light);
	~Light();

	void Enable();
	void Disable();
	void SetType(LightType type);
	void SetPosition(const Vector3& pos);
	void SetSpotParams(const Vector3& dir, const Angle& cutOff, float exponent);
	void SetColor(ColorType type, const Color& color);

	void Update();

	static void GlobalEnable();
	static void GlobalDisable();
	static Color GlobalAmbientColor;
private:
	GLenum light;
	LightType type;
	Vector3 position;
	Vector3 direction;
	Angle spotCuttOff = Angle::Zero();
	float spotExponent;
	bool enabled;
	Color AmbientColor;
	Color DiffuseColor;
	Color SpecularColor;
};
*/