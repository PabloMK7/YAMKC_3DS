#pragma once
#include <string>
#include <vector>
#include "Vector.hpp"
#include "Color.hpp"
#include "Texture.hpp"
#include "Graphics.hpp"
#include <tuple>

class Obj
{
public:
	using Face = std::tuple<Graphics::GPUVertex, Graphics::GPUVertex, Graphics::GPUVertex>;
	class Material
	{
	public:
		
		Material(Obj* parent, std::string name);
		~Material();
		std::string& GetName();
		void AddFace(const Face& face);
		void SetTexture(const std::string& fileName);
		void SetVisible(bool visible);
		void Draw();

		inline void SetTextureWrapMode(GPU_TEXTURE_WRAP_PARAM sWrap, GPU_TEXTURE_WRAP_PARAM tWrap) {texture->Wrap(sWrap, tWrap);}

	friend class Obj;
	private:
		Texture* texture = nullptr;
		Obj* parent;
		std::string name;
		std::vector<Obj::Face> faces;
		bool isVisible;
		Graphics::VertexArray* vArray;

		void ConvertToVBO(void);
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
	void LoadMatlib(std::string filename);
	Vector3 position;
	Angle3 rotation;
	Angle3 preRotation;
	Vector3 scale = Vector3(1.f, 1.f, 1.f);

	void ConvertToVBO(void);
};
