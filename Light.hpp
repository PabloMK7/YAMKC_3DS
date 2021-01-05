#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
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
	void SetPosition(const Vector3& postion);
	void SetColor(ColorType type, const Color& color);

	void Update();

	static void GlobalEnable();
	static void GlobalDisable();
	static Color GlobalAmbientColor;
private:
	GLenum light;
	LightType type;
	Vector3 position;
	bool enabled;
	Color AmbientColor;
	Color DiffuseColor;
	Color SpecularColor;
};