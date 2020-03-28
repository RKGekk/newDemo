#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iostream>
#include "TextUtil.h"
#include <xnamath.h>
#include <string>

class ObjLoader {
public:

	struct Vertex {
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct Material {
		std::string Name;
		XMFLOAT3 AmbientColor;
		XMFLOAT3 DiffuseColor;
		XMFLOAT3 SpecularColor;
		float SpecularCoefficient;
        float OpticalDensity;
		float Transparency;
        int IlluminationModel;
        std::string AmbientTextureMap;
		std::string DiffuseTextureMap;
        std::string SpecularTextureMap;
        std::string SpecularHighlightTextureMap;
        std::string BumpMap;
        std::string DisplacementMap;
        std::string StencilDecalMap;
        std::string AlphaTextureMap;
	};

	struct FaceVertex {
		int VertexIndex;
        int TextureIndex;
        int NormalIndex;
		//V = 0, VT = 1, VN = 2, VTN = 3
		int FaceType;
	};

	struct Face {
		std::vector<FaceVertex> vertices;
	};

	struct Group {
		std::string Name;
		Material Material;
		std::vector<Face> Faces;
	};

	struct ObjectContainer {
		std::string Name;
		Material Material;
		std::vector<Face> Faces;
		std::vector<Group> Groups;
	};

	ObjLoader(std::string fileName) {
		LoadObjFile(fileName);
	}

	std::vector<Vertex> Vertices;
	std::vector<XMFLOAT2> Textures;
	std::vector<XMFLOAT3> Normals;
	std::vector<Face> Faces;
	std::vector<int> Indices;
	std::vector<Group> Groups;
	std::vector<ObjectContainer> Objects;
	std::vector<Material> Materials;

private:
	void LoadObjFile(std::string fileName);

	void LoadMtlFile(std::string fileName);
};

#endif