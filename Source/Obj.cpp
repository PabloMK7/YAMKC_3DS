#include "Obj.hpp"
#include <fstream>
#include <iostream>
#include <sstream> 
#include <GL\glew.h>
#include <GL/freeglut.h>

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

static const char kPathSeparator =
#ifdef _WIN32
'\\';
#else
'/';
#endif

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
        throw "File not found.";

    matLib = "";
    std::string line;
    std::string currMat = "Default";

    while(std::getline(objFile, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto pieces = Split(line, ' ');
        if (pieces.empty()) continue;

        if (pieces[0] == "mtllib") {
            if (pieces.size() != 2)
                throw "Invalid matlib dimensions.";
            matLib = SplitFilename(filename).first + kPathSeparator + Trim(pieces[1]);
        }
        else if (pieces[0] == "usemtl") {
            if (pieces.size() != 2)
                throw "Invalid usemtl dimensions.";
            currMat = Trim(pieces[1]);
        }
        else if (pieces[0] == "v") {
            if (pieces.size() != 4)
                throw "Invalid vertex dimensions.";
            try
            {
                vertices.push_back(Vector3(std::stof(pieces[1]), std::stof(pieces[2]), std::stof(pieces[3])));
            }
            catch (const std::exception&)
            {
                throw "Invalid vertex values.";
            }
        }
        else if (pieces[0] == "vn") {
            if (pieces.size() != 4)
                throw "Invalid normal dimensions.";
            try
            {
                Vector3 normal = Vector3(std::stof(pieces[1]), std::stof(pieces[2]), std::stof(pieces[3]));
                normal.Normalize();
                normals.push_back(normal);
            }
            catch (const std::exception&)
            {
                throw "Invalid normal values.";
            }
        }
        else if (pieces[0] == "vt") {
            if (pieces.size() != 3)
                throw "Invalid texCoord dimensions.";
            try
            {
                texCoords.push_back(Vector2(std::stof(pieces[1]), std::stof(pieces[2])));
            }
            catch (const std::exception&)
            {
                throw "Invalid texCoord values.";
            }
        }
        else if (pieces[0] == "f") {
            if (pieces.size() != 4)
                throw "Invalid face dimensions.";
            int vertex[3], normals[3], texCoords[3];
            for (int i = 0; i < 3; i++) {
                vertex[i] = normals[i] = texCoords[i] = -1;
                auto subParts = Split(pieces[i + 1], '/', false);
                if (subParts.empty() || subParts[0].empty())
                    throw "Invalid face formatting.";
                try
                {
                    vertex[i] = std::stoi(subParts[0]) - 1;
                    if (vertex[i] < 0)
                        throw std::exception("");
                }
                catch (const std::exception&)
                {
                    throw "Invalid face formatting.";
                }
                if ((subParts.size() == 3 || subParts.size() == 2) && !subParts[1].empty())
                {
                    try
                    {
                        texCoords[i] = std::stoi(subParts[1]) - 1;
                        if (texCoords[i] < 0)
                            throw std::exception("");
                    }
                    catch (const std::exception&)
                    {
                        throw "Invalid face formatting.";
                    }
                }
                if (subParts.size() == 3 && !subParts[2].empty())
                {
                    try
                    {
                        normals[i] = std::stoi(subParts[2]) - 1;
                        if (normals[i] < 0)
                            throw std::exception("");
                    }
                    catch (const std::exception&)
                    {
                        throw "Invalid face formatting.";
                    }
                }                
            }
            GetMaterial(currMat).AddFace(Face(vertex, texCoords, normals));

        }
    }
    if (!matLib.empty())
        LoadMatlib(matLib);
}

Obj::~Obj()
{
}

void Obj::Draw()
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.x.AsDegrees(), 1.f, 0.f, 0.f);
    glRotatef(rotation.y.AsDegrees(), 0.f, 1.f, 0.f);
    glRotatef(rotation.z.AsDegrees(), 0.f, 0.f, 1.f);
    glRotatef(preRotation.x.AsDegrees(), 1.f, 0.f, 0.f);
    glRotatef(preRotation.y.AsDegrees(), 0.f, 1.f, 0.f);
    glRotatef(preRotation.z.AsDegrees(), 0.f, 0.f, 1.f);
    glScalef(scale.x, scale.y, scale.z);

    for (auto it = materials.begin(); it != materials.end(); it++)
        (*it).Draw();
    glPopMatrix();
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

std::vector<Obj::Material>& Obj::Materials() {
    return materials;
}

Obj::Material& Obj::GetMaterial(std::string name)
{
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        if (materials[i].GetName() == name)
            return materials[i];
    }
    materials.push_back(Material(this, name));
    return materials.back();
}

void Obj::LoadMatlib(std::string filename)
{
    std::ifstream mtlFile(filename);
    if (!mtlFile.is_open())
        throw "File not found.";

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
                throw "Invalid usemtl dimensions.";
            currMat = Trim(pieces[1]);
        }
        else if (pieces[0] == "Kd") {
            if (pieces.size() != 4)
                throw "Invalid color dimensions.";
            try
            {
                float r = std::stof(pieces[1]);
                float g = std::stof(pieces[2]);
                float b = std::stof(pieces[3]);
                if (r < 0.f || r > 1.f || g < 0.f || g > 1.f || b < 0.f || b > 1.f)
                    throw std::exception("");
                GetMaterial(currMat).SetColor(Material::ColorType::DIFFUSE, Color(r,g,b));
            }
            catch (const std::exception&)
            {
                throw "Invalid color format.";
            }            
        }
        else if (pieces[0] == "Ka") {
            if (pieces.size() != 4)
                throw "Invalid color dimensions.";
            try
            {
                float r = std::stof(pieces[1]);
                float g = std::stof(pieces[2]);
                float b = std::stof(pieces[3]);
                if (r < 0.f || r > 1.f || g < 0.f || g > 1.f || b < 0.f || b > 1.f)
                    throw std::exception("");
                GetMaterial(currMat).SetColor(Material::ColorType::AMBIENT, Color(r, g, b));
            }
            catch (const std::exception&)
            {
                throw "Invalid color format.";
            }
        }
        else if (pieces[0] == "Ks") {
            if (pieces.size() != 4)
                throw "Invalid color dimensions.";
            try
            {
                float r = std::stof(pieces[1]);
                float g = std::stof(pieces[2]);
                float b = std::stof(pieces[3]);
                if (r < 0.f || r > 1.f || g < 0.f || g > 1.f || b < 0.f || b > 1.f)
                    throw std::exception("");
                GetMaterial(currMat).SetColor(Material::ColorType::SPECULAR, Color(r, g, b));
            }
            catch (const std::exception&)
            {
                throw "Invalid color format.";
            }
        }
        else if (pieces[0] == "map_Kd") {
            if (pieces.size() != 2)
                throw "Invalid map_Kd dimensions.";
            std::string texFile = SplitFilename(filename).first + kPathSeparator + Trim(pieces[1]);
            GetMaterial(currMat).SetTexture(texFile);
        }
    }
}

Vector3& Obj::GetVertexFromID(int id)
{
    return vertices[id];
}

Vector2& Obj::GetTexCoordFromID(int id)
{
    return texCoords[id];
}

Vector3& Obj::GetNormalFromID(int id)
{
    return normals[id];
}

Obj::Face::Face(int vIds[3], int tIds[3], int nIds[3])
{
    memcpy(verticeIds, vIds, sizeof(verticeIds));
    memcpy(textureCoordIds, tIds, sizeof(textureCoordIds));
    memcpy(normalIds, nIds, sizeof(normalIds));
}

Vector3 Obj::Face::GetVertex(Obj* parent, int point)
{
    if (verticeIds[point] != -1)
        return parent->GetVertexFromID(verticeIds[point]);
    return Vector3();
}

Vector2 Obj::Face::GetTexCoord(Obj* parent, int point)
{
    if (textureCoordIds[point] != -1)
        return parent->GetTexCoordFromID(textureCoordIds[point]);
    return Vector2();
}

Vector3 Obj::Face::GetNormal(Obj* parent, int point)
{
    if (normalIds[point] != -1)
        return parent->GetNormalFromID(normalIds[point]);
    return Vector3();
}

Obj::Material::Material(Obj* parent, std::string name)
{
    this->parent = parent;
    this->name = name;
    fogDisabled = false;
    renderMode = RenderMode::NORMAL;
    isVisible = true;
    textureSMode = GL_REPEAT;
    textureTMode = GL_REPEAT;
}

Obj::Material::~Material()
{
    if (texture)
        delete texture;
}

std::string& Obj::Material::GetName()
{
    return name;
}

void Obj::Material::AddFace(const Obj::Face& face)
{
    faces.push_back(face);
}

void Obj::Material::SetColor(ColorType t, const Color& color)
{
    switch (t)
    {
    case Obj::Material::ColorType::AMBIENT:
        ambientColor = color;
        break;
    case Obj::Material::ColorType::DIFFUSE:
        diffuseColor = color;
        break;
    case Obj::Material::ColorType::SPECULAR:
        specularColor = color;
        break;
    default:
        break;
    }
}

Color dummyColor;
Color& Obj::Material::GetColor(ColorType t)
{
    switch (t)
    {
    case Obj::Material::ColorType::AMBIENT:
        return ambientColor;
        break;
    case Obj::Material::ColorType::DIFFUSE:
        return diffuseColor;
        break;
    case Obj::Material::ColorType::SPECULAR:
        return specularColor;
        break;
    default:
        return dummyColor;
        break;
    }
}

void Obj::Material::SetTexture(const std::string& fileName)
{
    if (texture)
        delete texture;
    texture = new Texture(fileName);
}

void Obj::Material::SetTextureRepeatMode(TextureDirection dir, int mode)
{
    switch (dir)
    {
    case Obj::Material::TextureDirection::DIR_S:
        textureSMode = mode;
        break;
    case Obj::Material::TextureDirection::DIR_T:
        textureTMode = mode;
        break;
    default:
        break;
    }
}

void Obj::Material::ForceDisableFog(bool disable)
{
    fogDisabled = disable;
}

void Obj::Material::SetRenderMode(RenderMode mode)
{
    renderMode = mode;
}

void Obj::Material::SetVisible(bool visible)
{
    isVisible = visible;
}

void Obj::Material::Draw()
{
    if (!isVisible)
        return;

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    
    if (texture) {
        glEnable(GL_TEXTURE_2D);
        if (texture->HasTransparency() && renderMode == RenderMode::NORMAL)
        {
            glAlphaFunc(GL_GREATER, 0.5);
            glEnable(GL_ALPHA_TEST);
        }

        texture->Bind();
    }
    else
        glBindTexture(GL_TEXTURE_2D, 0);

    if (renderMode == RenderMode::MULTIPLICATIVE) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
    }
    else if (renderMode == RenderMode::ADDITIVE) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureSMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureTMode);
    
    bool lightEnabled = glIsEnabled(GL_LIGHTING) == GL_TRUE;
    if (lightEnabled) {
        GLfloat args[4];
        args[0] = ambientColor.r; args[1] = ambientColor.g; args[2] = ambientColor.b; args[3] = ambientColor.a;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, args);
        args[0] = diffuseColor.r; args[1] = diffuseColor.g; args[2] = diffuseColor.b; args[3] = diffuseColor.a;
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, args);
        args[0] = specularColor.r; args[1] = specularColor.g; args[2] = specularColor.b; args[3] = specularColor.a;
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, args);
        args[0] = 0.f; args[1] = 0.f; args[2] = 0.f; args[3] = 0.f;
        glMaterialfv(GL_FRONT, GL_EMISSION, args);
    }
    else
        glColor4f(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
    

    bool wasFogEnabled = glIsEnabled(GL_FOG) == GL_TRUE;
    if (wasFogEnabled && fogDisabled)
        glDisable(GL_FOG);
    for (auto it = faces.begin(); it != faces.end(); it++) {
        Face &currFace = *it;
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; i++) {
            Vector3 normal = currFace.GetNormal(parent, i);
            Vector2 texCoord = currFace.GetTexCoord(parent, i);
            Vector3 vert = currFace.GetVertex(parent, i);

            glNormal3f(normal.x, normal.y, normal.z);
            glTexCoord2f(texCoord.x, texCoord.y);
            glVertex3f(vert.x, vert.y, vert.z);
        }
        glEnd();
    }

    if (wasFogEnabled && fogDisabled)
        glEnable(GL_FOG);

    if (renderMode != RenderMode::NORMAL) {
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
