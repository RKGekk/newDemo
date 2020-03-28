#include "Vertex.h"
#include "Effects.h"

#pragma region InputLayoutDesc
const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosColor[2] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};


const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormal[2] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Basic32[3] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::InstancedBasic32[8] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64,  D3D11_INPUT_PER_INSTANCE_DATA, 1 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Post[2] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Pos[1] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::InstancedBasicMTR[6] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64,  D3D11_INPUT_PER_INSTANCE_DATA, 1 }
};
#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::PosColor = 0;
ID3D11InputLayout* InputLayouts::PosNormal = 0;
ID3D11InputLayout* InputLayouts::Basic32 = 0;
ID3D11InputLayout* InputLayouts::InstancedBasic32 = 0;
ID3D11InputLayout* InputLayouts::Post = 0;
ID3D11InputLayout* InputLayouts::Pos = 0;
ID3D11InputLayout* InputLayouts::InstancedMTR = 0;

void InputLayouts::InitAll(ID3D11Device* device) {
	
	D3DX11_PASS_DESC passDesc;

	// PosColor
	Effects::ColorFX->mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::PosColor, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &PosColor));

	// PosNormal
	Effects::BasicFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::PosNormal, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &PosNormal));

	// Basic32
	Effects::BasicFX32->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32, 3, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Basic32));

	// InstancedBasic32
	Effects::InstancedBasicFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::InstancedBasic32, 8, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &InstancedBasic32));

	// Post
	Effects::PostFX->Tech1->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Post, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Post));

	// Pos
	Effects::SkyFX->SkyTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Pos, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Pos));

	// InstancedMTR
	Effects::InstancedBasicMTRFX->Tech1->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::InstancedBasicMTR, 6, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &InstancedMTR));
}

void InputLayouts::DestroyAll() {
	ReleaseCOM(PosColor);
	ReleaseCOM(PosNormal);
	ReleaseCOM(Basic32);
	ReleaseCOM(InstancedBasic32);
	ReleaseCOM(Post);
	ReleaseCOM(Pos);
	ReleaseCOM(InstancedMTR);
}

#pragma endregion