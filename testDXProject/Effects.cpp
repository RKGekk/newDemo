#include "Effects.h"
#include <regex>
#include <string>
#include <windows.h>
#include "TextUtil.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename, bool compile) : mFX(0) {
	if(compile) {
		DWORD shaderFlags = 0;
	#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif
	
		ID3D10Blob* compiledShader = 0;
		ID3D10Blob* compilationMsgs = 0;
	
		std::string path = ExePath() + std::string("\\") + ws2s(filename);
		std::string wpath = std::regex_replace(path, std::regex("\\\\"), std::string("/"));
	
		HRESULT hr = D3DX11CompileFromFile(s2ws(wpath).c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);
	
		if(compilationMsgs != 0) {
			MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationMsgs);
		}
	
		if(FAILED(hr)) {
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
		}
	
		HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, device, &mFX));
		ReleaseCOM(compiledShader);
	}
	else {
		std::ifstream fin(filename, std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> shader(size);

		fin.read(&shader[0], size);
		fin.close();

		HR(D3DX11CreateEffectFromMemory(&shader[0], size, 0, device, &mFX));
	}
}

Effect::~Effect() {
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region ColorEffect
ColorEffect::ColorEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

ColorEffect::~ColorEffect() {
}
#pragma endregion


#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	Light1Tech        = mFX->GetTechniqueByName("Light1");
	Light2Tech        = mFX->GetTechniqueByName("Light2");
	Light3Tech        = mFX->GetTechniqueByName("Light3");
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
}

BasicEffect::~BasicEffect() {
}
#pragma endregion

#pragma region BasicEffect32
BasicEffect32::BasicEffect32(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

BasicEffect32::~BasicEffect32() {
}
#pragma endregion

#pragma region BasicEffect32BF
BasicEffect32BF::BasicEffect32BF(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

BasicEffect32BF::~BasicEffect32BF() {
}
#pragma endregion

#pragma region BlurEffect
BlurEffect::BlurEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	HorzBlurTech = mFX->GetTechniqueByName("HorzBlur");
	VertBlurTech = mFX->GetTechniqueByName("VertBlur");

	Weights     = mFX->GetVariableByName("gWeights")->AsScalar();
	InputMap    = mFX->GetVariableByName("gInput")->AsShaderResource();
	OutputMap   = mFX->GetVariableByName("gOutput")->AsUnorderedAccessView();
}

BlurEffect::~BlurEffect() {
}
#pragma endregion

#pragma region InstancedBasicEffect
InstancedBasicEffect::InstancedBasicEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

InstancedBasicEffect::~InstancedBasicEffect() {
}
#pragma endregion

#pragma region Post
PostEffect::PostEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	Tech1			  = mFX->GetTechniqueByName("Tech1");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	DiffuseMap2        = mFX->GetVariableByName("gDiffuseMap2")->AsShaderResource();
}

PostEffect::~PostEffect() {
}
#pragma endregion

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	SkyTech       = mFX->GetTechniqueByName("SkyTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap       = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect() {
}
#pragma endregion

#pragma region InstancedBasicMTREffect
InstancedBasicMTREffect::InstancedBasicMTREffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename, true) {
	Tech1    = mFX->GetTechniqueByName("Tech1");
	Tech2    = mFX->GetTechniqueByName("Tech1");

	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
}

InstancedBasicMTREffect::~InstancedBasicMTREffect() {
}

#pragma region Effects
ColorEffect* Effects::ColorFX = 0;
BasicEffect* Effects::BasicFX = 0;
BasicEffect32* Effects::BasicFX32 = 0;
BasicEffect32BF* Effects::BasicFX32BF = 0;
BlurEffect* Effects::BlurFX = 0;
InstancedBasicEffect* Effects::InstancedBasicFX = 0;
PostEffect* Effects::PostFX = 0;
SkyEffect* Effects::SkyFX = 0;
InstancedBasicMTREffect* Effects::InstancedBasicMTRFX = 0;


void Effects::InitAll(ID3D11Device* device) {

	ColorFX = new ColorEffect(device, L"color.fx");
	BasicFX = new BasicEffect(device, L"Basic.fx");
	BasicFX32 = new BasicEffect32(device, L"Basic32.fx");
	BasicFX32BF = new BasicEffect32BF(device, L"Basic32BF.fx");
	BlurFX = new BlurEffect(device, L"Blur.fx");
	InstancedBasicFX = new InstancedBasicEffect(device, L"InstancedBasic32BF.fx");
	PostFX = new PostEffect(device, L"Post.fx");
	SkyFX = new SkyEffect(device, L"Sky.fx");
	InstancedBasicMTRFX = new InstancedBasicMTREffect(device, L"InstancedBasicMTR.fx");
}

void Effects::DestroyAll() {
	SafeDelete(ColorFX);
	SafeDelete(BasicFX);
	SafeDelete(BasicFX32);
	SafeDelete(BasicFX32BF);
	SafeDelete(BlurFX);
	SafeDelete(InstancedBasicFX);
	SafeDelete(PostFX);
	SafeDelete(SkyFX);
	SafeDelete(InstancedBasicMTRFX);
}
#pragma endregion