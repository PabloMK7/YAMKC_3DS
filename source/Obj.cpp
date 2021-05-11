#include "Obj.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

static inline std::string& Ltrim(std::string& str)
{
	auto it = std::find_if(str.begin(), str.end(), [](char ch) { return (!std::isspace(ch)); });
	str.erase(str.begin(), it);
	return (str);
}

static inline std::string& Rtrim(std::string& str)
{
	auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) { return (!std::isspace(ch)); });
	str.erase(it.base(), str.end());
	return (str);
}

static inline std::string& Trim(std::string& str)
{
	return (Ltrim(Rtrim(str)));
}

static inline std::vector<std::string> Split(const std::string& str, char splitChar, bool ignoreEmpty = true) {
    std::stringstream origStr(str);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(origStr, segment, splitChar)) 
    {
        if (!(segment.empty() && ignoreEmpty)) seglist.push_back(segment);
    }
    return seglist;
}

static const char kPathSeparator = '/';

static inline std::pair<std::string, std::string> SplitFilename(const std::string& str)
{
    size_t found;
    found = str.find_last_of("/\\");
    return { str.substr(0, found), str.substr(found + 1) };
}

Obj::Obj(std::string filename)
{
    std::ifstream objFile(filename);
    if (!objFile.is_open())
        return;

    matLib = "";
    std::string line;
    std::string currMat = "Default";

    std::vector<Vector3> objVertices;
    std::vector<Vector3> objNormals;
    std::vector<Vector2> objTexCoords;

    while(std::getline(objFile, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto pieces = Split(line, ' ');
        if (pieces.empty()) continue;

        if (pieces[0] == "mtllib") {
	    if (pieces.size() != 2)
                continue;
            matLib = SplitFilename(filename).first + kPathSeparator + Trim(pieces[1]);
        }
        else if (pieces[0] == "usemtl") {
	    if (pieces.size() != 2)
                continue;
            currMat = Trim(pieces[1]);
        }
        else if (pieces[0] == "v") {
	    if (pieces.size() != 4)
                continue;
            objVertices.push_back(Vector3(std::stof(pieces[1]), std::stof(pieces[2]), std::stof(pieces[3])));
        }
        else if (pieces[0] == "vn") {
	    if (pieces.size() != 4)
                continue;
            Vector3 normal = Vector3(std::stof(pieces[1]), std::stof(pieces[2]), std::stof(pieces[3]));
            normal.Normalize();
            objNormals.push_back(normal);
        }
        else if (pieces[0] == "vt") {
	    if (pieces.size() != 3)
                continue;
            objTexCoords.push_back(Vector2(std::stof(pieces[1]), std::stof(pieces[2])));
        }
        else if (pieces[0] == "f") {
	    if (pieces.size() != 4)
                continue;
            int vertex[3], normals[3], texCoords[3];
	    bool error = false;
            for (int i = 0; i < 3; i++) {
                vertex[i] = normals[i] = texCoords[i] = -1;
                auto subParts = Split(pieces[i + 1], '/', false);
		if (subParts.empty() || subParts[0].empty())
		{
		    error = true;
		    break;
		}
                vertex[i] = std::stoi(subParts[0]) - 1;
		if (vertex[i] < 0)
		{
		    error = true;
		    break;
		}
                if ((subParts.size() == 3 || subParts.size() == 2) && !subParts[1].empty())
                {
                    texCoords[i] = std::stoi(subParts[1]) - 1;
		    if (texCoords[i] < 0)
		    {
		        error = true;
		        break;
		    }
                }
                if (subParts.size() == 3 && !subParts[2].empty())
                {
                    normals[i] = std::stoi(subParts[2]) - 1;
		    if (normals[i] < 0)
		    {
		        error = true;
		        break;
		    }
                }                
            }
	    if (error)
	        continue;
        Graphics::GPUVertex vInfo[3];
        for (int i = 0; i < 3; i++)
        {
            Vector3 position(0.f, 0.f, 0.f);
            Vector3 normal(0.f, 1.f, 0.f);
            Vector2 texCoord(0.f, 0.f);
            if (vertex[i] != -1)
                position = objVertices[vertex[i]];
            if (normals[i] != -1)
                normal = objNormals[normals[i]];
            if (texCoords[i] != -1)
                texCoord = objTexCoords[texCoords[i]];
            
            normal.Normalize();

            vInfo[i].position.x = position.x;
            vInfo[i].position.y = position.y;
            vInfo[i].position.z = position.z;
            vInfo[i].normal.x = normal.x;
            vInfo[i].normal.y = normal.y;
            vInfo[i].normal.z = normal.z;
            vInfo[i].texcoord.u = texCoord.x;
            vInfo[i].texcoord.v = texCoord.y;
            vInfo[i].color = {1.f, 1.f, 1.f, 1.f};
            
        }
        GetMaterial(currMat).AddFace(std::make_tuple(vInfo[0], vInfo[1], vInfo[2]));

        }
    }
    if (!matLib.empty())
        LoadMatlib(matLib);
    ConvertToVBO();
}

Obj::~Obj()
{
    for (auto it = materials.begin(); it != materials.end(); it++) {
        delete *it;
    }
    materials.clear();
}

void Obj::Draw()
{
    C3D_Mtx* m = Graphics::PushModelViewMtx();
    Mtx_Translate(m, position.x, position.y, position.z, true);
    Mtx_RotateX(m, rotation.x.AsRadians(), true);
    Mtx_RotateY(m, rotation.y.AsRadians(), true);
    Mtx_RotateZ(m, rotation.z.AsRadians(), true);
    Mtx_RotateX(m, preRotation.x.AsRadians(), true);
    Mtx_RotateY(m, preRotation.y.AsRadians(), true);
    Mtx_RotateZ(m, preRotation.z.AsRadians(), true);
    Mtx_Scale(m, scale.x, scale.y, scale.z);
    Graphics::UpdateModelViewMtx();

    for (auto it = materials.begin(); it != materials.end(); it++)
        (*it)->Draw();
    Graphics::PopModelViewMtx();
}

Vector3& Obj::GetPosition()
{
    return position;
}

Angle3& Obj::GetRotation()
{
    return rotation;
}

Angle3& Obj::GetPreRotation()
{
    return preRotation;
}

Vector3& Obj::GetScale()
{
    return scale;
}

std::vector<Material*>& Obj::Materials() {
    return materials;
}

Material& Obj::GetMaterial(std::string name)
{
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        if (materials[i]->GetName() == name)
            return *materials[i];
    }
    materials.push_back(new Material(name));
    return *materials.back();
}

void Obj::LoadMatlib(std::string filename)
{
    std::ifstream mtlFile(filename);
    if (!mtlFile.is_open())
        return;
    matLib = "";
    std::string line;
    std::string currMat = "Default";

    while (std::getline(mtlFile, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto pieces = Split(line, ' ');
        if (pieces.empty()) continue;

        if (pieces[0] == "newmtl") {
	        if (pieces.size() != 2)
                continue;
            currMat = Trim(pieces[1]);
        }
        else if (pieces[0] == "Kd") {
	        if (pieces.size() != 4)
                continue;
            float r = std::stof(pieces[1]);
            float g = std::stof(pieces[2]);
            float b = std::stof(pieces[3]);
            GetMaterial(currMat).SetFragmentColor(Material::FragmentColor::DIFFUSE, Color(r, g, b));
        }
        else if (pieces[0] == "Ka") {
	        if (pieces.size() != 4)
                continue;
            float r = std::stof(pieces[1]);
            float g = std::stof(pieces[2]);
            float b = std::stof(pieces[3]);
            GetMaterial(currMat).SetFragmentColor(Material::FragmentColor::AMBIENT, Color(r, g, b));
        }
        else if (pieces[0] == "Ks") {
	        if (pieces.size() != 4)
                continue;
            float r = std::stof(pieces[1]);
            float g = std::stof(pieces[2]);
            float b = std::stof(pieces[3]);
            GetMaterial(currMat).SetFragmentColor(Material::FragmentColor::SPECULAR0, Color(r, g, b));
        }
        else if (pieces[0] == "map_Kd") {
	    if (pieces.size() != 2)
                continue;
            std::string texFile = SplitFilename(filename).first + kPathSeparator + Trim(pieces[1]);
            GetMaterial(currMat).SetTexture(texFile);
        }
    }
}

void Obj::ConvertToVBO(void)
{
    for (auto it = materials.begin(); it != materials.end(); it++)
        (*it)->ConvertToVBO();
}
