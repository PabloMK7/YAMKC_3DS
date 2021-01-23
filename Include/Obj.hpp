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
		enum class TextureDirection
		{
			DIR_S,
			DIR_T
		};
		enum class ColorType
		{
			AMBIENT,
			DIFFUSE,
			SPECULAR
		};
		enum class RenderMode
		{
			NORMAL,
			MULTIPLICATIVE,
			ADDITIVE
		};
		Material(Obj* parent, std::string name);
		~Material();
		std::string& GetName();
		void AddFace(const Face& face);
		void SetColor(ColorType t, const Color& color);
		Color& GetColor(ColorType t);
		void SetTexture(const std::string& fileName);
		void SetTextureRepeatMode(TextureDirection dir, int mode);
		void ForceDisableFog(bool disable);
		void SetRenderMode(RenderMode mode);
		void SetVisible(bool visible);
		void Draw();

	private:
		Color diffuseColor;
		Color ambientColor;
		Color specularColor;
		Texture* texture = nullptr;
		Obj* parent;
		std::string name;
		std::vector<Obj::Face> faces;
		int textureSMode;
		int textureTMode;
		bool fogDisabled;
		RenderMode renderMode;
		bool isVisible;
	};

	Obj(std::string filename);
	~Obj();

	void Draw();
	Vector3& GetPosition();
	Angle3& GetRotation();
	Angle3& GetPreRotation();
	Vector3& GetScale();
	std::vector<Obj::Material>& Materials();
	Material& GetMaterial(std::string name);

private:
	std::string matLib;
	std::vector<Material> materials;
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texCoords;
	void LoadMatlib(std::string filename);
	Vector3 position;
	Angle3 rotation;
	Angle3 preRotation;
	Vector3 scale = Vector3(1.f, 1.f, 1.f);
protected:
	Vector3& GetVertexFromID(int id);
	Vector2& GetTexCoordFromID(int id);
	Vector3& GetNormalFromID(int id);
};
