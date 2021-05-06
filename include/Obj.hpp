#pragma once
#include <string>
#include <vector>
#include "Vector.hpp"
#include "Color.hpp"
#include "Texture.hpp"
#include "Graphics.hpp"
#include "Material.hpp"

class Obj
{
public:

	Obj(std::string filename);
	~Obj();

	void Draw();
	Vector3& GetPosition();
	Angle3& GetRotation();
	Angle3& GetPreRotation();
	Vector3& GetScale();
	std::vector<Material*>& Materials();
	Material& GetMaterial(std::string name);

private:
	std::string matLib;
	std::vector<Material*> materials;
	void LoadMatlib(std::string filename);
	Vector3 position;
	Angle3 rotation;
	Angle3 preRotation;
	Vector3 scale = Vector3(1.f, 1.f, 1.f);

	void ConvertToVBO(void);
};
