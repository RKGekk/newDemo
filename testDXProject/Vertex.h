#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

namespace Vertex {
	struct PosColor {
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct PosNormal {
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};

	struct Basic32 {
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct Post {
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};

	struct Position {
		XMFLOAT3 Pos;
	};
}

namespace Instance {
	struct InstancedData {
		XMFLOAT4X4 World;
		XMFLOAT4 Color;
	};
}

class InputLayoutDesc {
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC PosNormal[2];
	static const D3D11_INPUT_ELEMENT_DESC PosColor[2];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC InstancedBasic32[8];
	static const D3D11_INPUT_ELEMENT_DESC Post[2];
	static const D3D11_INPUT_ELEMENT_DESC Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC InstancedBasicMTR[6];
};

class InputLayouts {
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* PosColor;
	static ID3D11InputLayout* PosNormal;
	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* InstancedBasic32;
	static ID3D11InputLayout* Post;
	static ID3D11InputLayout* Pos;
	static ID3D11InputLayout* InstancedMTR;
};

#endif // VERTEX_H