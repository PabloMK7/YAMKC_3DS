#pragma once
#include <string>
#include <vector>
#include "Vector.hpp"
#include "Color.hpp"
#include "Texture.hpp"

class Obj
{
public:
	class Face
	{
	public:
		Face(int vIds[3], int tIds[3], int nIds[3]);
		Vector3 GetVertex(Obj* parent, int point);
		Vector2 GetTexCoord(Obj* parent, int point);
		Vector3 GetNormal(Obj* parent, int point);

	private:
		int verticeIds[3];
		int textureCoordIds[3];
		int normalIds[3];
	};

	class Material
	{
	public:
		Material(Obj* parent, std::string name);
		~Material();
		std::string& GetName();
		void AddFace(const Face& face);
		void SetColor(const Color& color);
		void SetTexture(const std::string& fileName);
		void Draw();

	private:
		Color diffuseColor;
		Texture* texture = nullptr;
		Obj* parent;
		std::string name;
		std::vector<Obj::Face> faces;
	};

	Obj(std::string filename);
	~Obj();
	void Draw();

private:
	std::string matLib;
	std::vector<Material> materials;
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texCoords;
	Material& GetMaterial(std::string name);
	void LoadMatlib(std::string filename);
protected:
	Vector3& GetVertexFromID(int id);
	Vector2& GetTexCoordFromID(int id);
	Vector3& GetNormalFromID(int id);
};
