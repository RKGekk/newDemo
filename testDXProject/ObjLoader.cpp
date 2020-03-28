#include "ObjLoader.h"
#include <xnamath.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

void ObjLoader::LoadObjFile(std::string fileName) {
	std::ifstream in(fileName);
	std::string line;
	if(in.is_open()) {

		Group currentGroup;
		Material currentMaterial;
		std::vector<Face> currentFaces;
		std::vector<std::string> tokens;
		bool firstGroup = true;
		while(std::getline(in, line)) {
			
			split(line, tokens, " ");
			int tokensCount = tokens.size();
			std::string keyword = tokens[0];
			
			if(keyword == "v" && tokensCount > 1) {
				float x = std::stof(tokens[1]);
				float y = std::stof(tokens[2]);
				float z = std::stof(tokens[3]);
				if(tokensCount > 4) {
					float r = std::stof(tokens[4]);
					float g = std::stof(tokens[5]);
					float b = std::stof(tokens[6]);
					Vertex vpos = {
						XMFLOAT3(x, y, z),
						XMFLOAT4(r, g, b, 1.0f),
					};
					Vertices.push_back(vpos);
				}
				else {
					Vertex vpos = {
						XMFLOAT3(x, y, z),
						XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
					};
					Vertices.push_back(vpos);
				}
				continue;
			}

			if(keyword == "vt" && tokensCount > 1) {
				float u = std::stof(tokens[1]);
				float v = std::stof(tokens[2]);
				XMFLOAT2 vtex = XMFLOAT2(u, v);
				Textures.push_back(vtex);
				continue;
			}

			if(keyword == "vn" && tokensCount > 1) {
				float x = std::stof(tokens[1]);
				float y = std::stof(tokens[2]);
				float z = std::stof(tokens[3]);
				XMFLOAT3 vnor = XMFLOAT3(x, y, z);
				Normals.push_back(vnor);
				continue;
			}
			
			if(keyword == "f" && tokensCount > 1) {
				Face vface;
				std::vector<std::string> sindices;
				for(auto it = tokens.begin() + 1; it != tokens.end(); ++it) {
					FaceVertex vfaceInd;
					split(*it, sindices, "/");
					std::string sIP = sindices[0];
					std::string sIT;
					std::string sIN;
					if(sindices.size() > 1) {
						sIT = sindices[1];
						if(sindices.size() > 2) {
							sIN = sindices[2];
						}
					}
					if(sIP != "") vfaceInd.VertexIndex = std::stoi(sIP);
					if(sIT != "") vfaceInd.TextureIndex = std::stoi(sIT); else vfaceInd.TextureIndex = 0;
					if(sIN != "") vfaceInd.NormalIndex = std::stoi(sIN); else vfaceInd.NormalIndex = 0;
					//V = 0, VT = 1, VN = 2, VTN = 3
					if(sIP != "" && sIT != "" && sIN != "") {
						vfaceInd.FaceType = 3;
					}
					else if(sIP != "" && sIT != "" && sIN == "") {
						vfaceInd.FaceType = 1;
					}
					else if(sIP != "" && sIT == "" && sIN != "") {
						vfaceInd.FaceType = 2;
					}
					else if(sIP != "" && sIT == "" && sIN == "") {
						vfaceInd.FaceType = 0;
					}
					vface.vertices.push_back(vfaceInd);
					Indices.push_back(vfaceInd.VertexIndex - 1);
				}
				Faces.push_back(vface);
				currentFaces.push_back(vface);
				continue;
			}

			if(keyword == "mtllib" && tokensCount > 1) {
				std::string mtlFilePath = tokens[1];
				LoadMtlFile(mtlFilePath);
				continue;
			}

			if(keyword == "usemtl" && tokensCount > 1) {
				std::string mtlName = tokens[1];
				for(int i = 0; i < Materials.size(); ++i){
					if(Materials[i].Name == mtlName){
						currentMaterial = Materials[i];
					}
					break;
				}
				continue;
			}
			
			if(keyword == "g" && tokensCount > 1) {
				std::string groupName = tokens[1];
				currentGroup.Name = groupName;
				if(!firstGroup) {
					copy(currentFaces.begin(), currentFaces.end(), std::back_inserter(currentGroup.Faces));
					currentFaces.clear();
					currentGroup.Material = currentMaterial;
					Groups.push_back(currentGroup);
					currentGroup = Group();
				}
				firstGroup = false;
				continue;
			}
		}
		copy(currentFaces.begin(), currentFaces.end(), std::back_inserter(currentGroup.Faces));
		currentGroup.Material = currentMaterial;
		Groups.push_back(currentGroup);
	}
	in.close();
}

void ObjLoader::LoadMtlFile(std::string fileName) {
	std::ifstream in(fileName);
	std::string line;
	if(in.is_open()) {
		Material newMaterial;
		std::vector<std::string> tokens;
		while(std::getline(in, line)) {
			
			split(line, tokens, " ");
			int tokensCount = tokens.size();
			if(tokensCount == 0) {
				continue;
			}
			std::string keyword = tokens[0];
			
			if(keyword == "newmtl" && tokensCount > 1) {
				newMaterial.Name = tokens[1];
				continue;
			}
			// Ambient Color
			if(keyword == "Ka" && tokensCount > 1) {
				float x = std::stof(tokens[1]);
				float y = std::stof(tokens[2]);
				float z = std::stof(tokens[3]);
				newMaterial.AmbientColor = XMFLOAT3(x, y, z);
				continue;
			}
			// Diffuse Color
			if(keyword == "Kd" && tokensCount > 1) {
				float x = std::stof(tokens[1]);
				float y = std::stof(tokens[2]);
				float z = std::stof(tokens[3]);
				newMaterial.DiffuseColor = XMFLOAT3(x, y, z);
				continue;
			}
			// Specular Color
			if(keyword == "Ks" && tokensCount > 1) {
				float x = std::stof(tokens[1]);
				float y = std::stof(tokens[2]);
				float z = std::stof(tokens[3]);
				newMaterial.SpecularColor = XMFLOAT3(x, y, z);
				continue;
			}
			// Specular Exponent
			if(keyword == "Ns" && tokensCount > 1) {
				float ns = std::stof(tokens[1]);
				newMaterial.SpecularCoefficient = ns;
				continue;
			}
			// Optical Density
			if(keyword == "Ni" && tokensCount > 1) {
				float ni = std::stof(tokens[1]);
				newMaterial.OpticalDensity = ni;
				continue;
			}
			// Dissolve transparency
			if(keyword == "d" && tokensCount > 1) {
				float d = std::stof(tokens[1]);
				newMaterial.Transparency = d;
				continue;
			}
			// Illumination model
			if(keyword == "illum" && tokensCount > 1) {
				int i = std::stoi(tokens[1]);
				newMaterial.IlluminationModel = i;
				continue;
			}
			// Ambient Texture Map
			if(keyword == "map_Ka" && tokensCount > 1) {
				newMaterial.AmbientTextureMap = tokens[1];
				continue;
			}
			// Diffuse Texture Map
			if(keyword == "map_Kd" && tokensCount > 1) {
				newMaterial.DiffuseTextureMap = tokens[1];
				continue;
			}
			// Specular Texture Map
			if(keyword == "map_Ks" && tokensCount > 1) {
				newMaterial.SpecularTextureMap = tokens[1];
				continue;
			}
			// Specular Hightlight Map
			if(keyword == "map_Ns" && tokensCount > 1) {
				newMaterial.SpecularHighlightTextureMap = tokens[1];
				continue;
			}
			// Alpha Texture Map
			if(keyword == "map_d" && tokensCount > 1) {
				newMaterial.AlphaTextureMap = tokens[1];
				continue;
			}
			// Bump Map
			if(keyword == "map_Bump" && tokensCount > 1) {
				newMaterial.BumpMap = tokens[1];
				continue;
			}
		}
		Materials.push_back(newMaterial);
	}
	in.close();
}