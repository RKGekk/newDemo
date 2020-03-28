#include <windows.h>
#include <xnamath.h>
#include "xnacollision.h"
#include <iostream>
#include <d3dApp.h>
#include <d3dx11effect.h>
#include <MathHelper.h>
#include <regex>
#include <string>
#include <GeometryGenerator.h>
#include <Waves.h>
#include <LightHelper.h>
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "BlurFilter.h"
#include "Camera.h"
#include "Sky.h"
#include "TextUtil.h"
#include "ObjLoader.h"

using namespace std;

ostream& operator<<(ostream& os, FXMVECTOR v) {

	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);

	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";

	return os;
}

ostream& operator<<(ostream& os, CXMMATRIX m) {

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			os << m(i, j) << "\t";
		}
		os << endl;
	}

	return os;
}

enum RenderOptions {
	Lighting = 0,
	Textures = 1,
	TexturesAndFog = 2
};

class BoxApp : public D3DApp {

public:
	BoxApp(HINSTANCE hInstance);
	~BoxApp();

	bool Init();

	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void UpdateWaves();
	void DrawWrapper();
	void DrawWrapper2();
	void DrawScreenQuad();

	void BuildCrateGeometryBuffers();
	void BuildSheepGeometryBuffers();
	void BuildInstancedBuffer();
	void BuildScreenQuadGeometryBuffers();
	void BuildOffscreenViews();

	ID3D11Buffer* mLandVB;
	ID3D11Buffer* mLandIB;
	ID3D11Buffer* mInstancedBuffer;
	std::vector<Instance::InstancedData> mInstancedData;
	ID3D11ShaderResourceView* mDiffuseMapSRV;

	ID3D11Buffer* mSheepVB;
	ID3D11Buffer* mSheepIB;
	int mSheepIndexCount;



	XNA::AxisAlignedBox mSphereBox;
	XNA::AxisAlignedBox mSheepBox;
	XNA::Frustum mCamFrustum;

	UINT mVisibleObjectCount;
	bool mFrustumCullingEnabled;

	ID3D11Buffer* mSphereVB;
	ID3D11Buffer* mSphereIB;
	int mSphereIndexCount;

	ID3D11Buffer* mScreenQuadVB;
	ID3D11Buffer* mScreenQuadIB;

	ID3D11ShaderResourceView* mOffscreenSRV;
	ID3D11UnorderedAccessView* mOffscreenUAV;
	ID3D11RenderTargetView* mOffscreenRTV;

	ID3D11ShaderResourceView* mOffscreenSRV2;
	ID3D11UnorderedAccessView* mOffscreenUAV2;
	ID3D11RenderTargetView* mOffscreenRTV2;

	BlurFilter mBlur;
	Sky* mSky;

	DirectionalLight mDirLights[3];
	Material mSphereMat;

	XMFLOAT4X4 mSphereWorld;
	XMFLOAT4X4 mSheepWorld;

	RenderOptions mRenderOptions;

	Camera mCam;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	if(!XMVerifyCPUSupport()) {
		cout << "xna math not supported" << endl;
		return 0;
	}

	BoxApp theApp(hInstance);

	if(!theApp.Init()) {
		return 0;
	}

	return theApp.Run();
}

BoxApp::BoxApp(HINSTANCE hInstance) : D3DApp(hInstance), mSky(0), mLandVB(0), mLandIB(0), mSphereVB(0), mSphereIB(0), mDiffuseMapSRV(0), mInstancedBuffer(0), mScreenQuadVB(0), mScreenQuadIB(0), mOffscreenSRV(0), mOffscreenUAV(0), mOffscreenRTV(0), mOffscreenSRV2(0), mOffscreenUAV2(0), mOffscreenRTV2(0), mSphereIndexCount(0), mRenderOptions(RenderOptions::TexturesAndFog), mFrustumCullingEnabled(false) {
	mMainWndCaption = L"Box Demo";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(0.0f, 2.0f, -15.0f);

	XMMATRIX I = XMMatrixIdentity();

	XMMATRIX boxScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&mSphereWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX sheepScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX sheepOffset = XMMatrixTranslation(15.0f, 3.0f, -20.0f);
	XMStoreFloat4x4(&mSheepWorld, XMMatrixMultiply(sheepScale, sheepOffset));

	mDirLights[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.87f, 0.85f, 0.95f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient  = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.50f, 0.50f, 0.50f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient  = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.85f, 0.85f, 0.85f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	mSphereMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mSphereMat.Diffuse  = XMFLOAT4(0.978f, 0.965f, 0.995f, 1.0f);
	mSphereMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}

BoxApp::~BoxApp() {
	md3dImmediateContext->ClearState();

	SafeDelete(mSky);

	ReleaseCOM(mLandVB);
	ReleaseCOM(mLandIB);
	ReleaseCOM(mSphereVB);
	ReleaseCOM(mSphereIB);
	ReleaseCOM(mScreenQuadVB);
	ReleaseCOM(mScreenQuadIB);
	ReleaseCOM(mDiffuseMapSRV);
 
	ReleaseCOM(mOffscreenSRV);
	ReleaseCOM(mOffscreenUAV);
	ReleaseCOM(mOffscreenRTV);

	ReleaseCOM(mOffscreenSRV2);
	ReleaseCOM(mOffscreenUAV2);
	ReleaseCOM(mOffscreenRTV2);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool BoxApp::Init() {
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	mSky = new Sky(md3dDevice, L"10.dds", 5000.0f);

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"f8bb7ef71b1e29a6cabf59dfb6f9e38f.jpg", 0, 0, &mDiffuseMapSRV, 0 ));

	BuildCrateGeometryBuffers();
	BuildSheepGeometryBuffers();
	BuildInstancedBuffer();
	BuildScreenQuadGeometryBuffers();
	BuildOffscreenViews();

	return true;
}

void BoxApp::OnResize() {
	D3DApp::OnResize();

	// Recreate the resources that depend on the client area size.
	BuildOffscreenViews();
	mBlur.Init(md3dDevice, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	mCam.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	// Build the frustum from the projection matrix in view space.
	ComputeFrustumFromProjection(&mCamFrustum, &mCam.Proj());
}

void BoxApp::UpdateScene(float dt) {
	// Control the camera.
	if( GetAsyncKeyState('W') & 0x8000 ) {
		mCam.Walk(10.0f * dt);
	}

	if( GetAsyncKeyState('S') & 0x8000 ) {
		mCam.Walk(-10.0f * dt);
	}

	if( GetAsyncKeyState('A') & 0x8000 ) {
		mCam.Strafe(-10.0f * dt);
	}

	if( GetAsyncKeyState('D') & 0x8000 ) {
		mCam.Strafe(10.0f * dt);
	}

	// Switch the render mode based in key input.
	if( GetAsyncKeyState('1') & 0x8000 ) {
		mRenderOptions = RenderOptions::Lighting; 
	}

	if( GetAsyncKeyState('2') & 0x8000 ) {
		mRenderOptions = RenderOptions::Textures;
	}

	if( GetAsyncKeyState('3') & 0x8000 ) {
		mRenderOptions = RenderOptions::TexturesAndFog;
	}

	// Perform frustum culling.
	mCam.UpdateViewMatrix();
	mVisibleObjectCount = 0;

	if(mFrustumCullingEnabled) {
		XMVECTOR detView = XMMatrixDeterminant(mCam.View());
		XMMATRIX invView = XMMatrixInverse(&detView, mCam.View());
	
		D3D11_MAPPED_SUBRESOURCE mappedData; 
		md3dImmediateContext->Map(mInstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		Instance::InstancedData* dataView = reinterpret_cast<Instance::InstancedData*>(mappedData.pData);

		for(UINT i = 0; i < mInstancedData.size(); ++i) {
			XMMATRIX W = XMLoadFloat4x4(&mInstancedData[i].World);
			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

			// View space to the object's local space.
			XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);
		
			// Decompose the matrix into its individual parts.
			XMVECTOR scale;
			XMVECTOR rotQuat;
			XMVECTOR translation;
			XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

			// Transform the camera frustum from view space to the object's local space.
			XNA::Frustum localspaceFrustum;
			XNA::TransformFrustum(&localspaceFrustum, &mCamFrustum, XMVectorGetX(scale), rotQuat, translation);

			// Perform the box/frustum intersection test in local space.
			if(XNA::IntersectAxisAlignedBoxFrustum(&mSphereBox, &localspaceFrustum) != 0) {
				// Write the instance data to dynamic VB of the visible objects.
				dataView[mVisibleObjectCount++] = mInstancedData[i];
			}
		}

		md3dImmediateContext->Unmap(mInstancedBuffer, 0);
	}
	else { // No culling enabled, draw all objects.

		/*
		D3D11_MAPPED_SUBRESOURCE mappedData; 
		md3dImmediateContext->Map(mInstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		Instance::InstancedData* dataView = reinterpret_cast<Instance::InstancedData*>(mappedData.pData);

		for(UINT i = 0; i < mInstancedData.size(); ++i) {
			dataView[mVisibleObjectCount++] = mInstancedData[i];
		}

		md3dImmediateContext->Unmap(mInstancedBuffer, 0);*/
		mVisibleObjectCount = mInstancedData.size();
	}

	std::wostringstream outs;   
	outs.precision(6);
	outs << L"    " << mVisibleObjectCount << L" objects drawn ";
	mMainWndCaption = outs.str();
}

void BoxApp::DrawScene() {
	// Render to our offscreen texture.  Note that we can use the same depth/stencil buffer
	// we normally use since our offscreen texture matches the dimensions.

	//ID3D11RenderTargetView* renderTargets[1] = {mOffscreenRTV};
	ID3D11RenderTargetView* renderTargets[2] = {mOffscreenRTV2, mOffscreenRTV};
	//ID3D11RenderTargetView* renderTargets[1] = {mRenderTargetView};
	md3dImmediateContext->OMSetRenderTargets(2, renderTargets, mDepthStencilView);

	md3dImmediateContext->ClearRenderTargetView(mOffscreenRTV, reinterpret_cast<const float*>(&Colors::Black2));
	md3dImmediateContext->ClearRenderTargetView(mOffscreenRTV2, reinterpret_cast<const float*>(&Colors::Black2));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Draw the scene to the offscreen texture
	DrawWrapper2();
	DrawWrapper();

	mSky->Draw(md3dImmediateContext, mCam);

	// Restore the back buffer.  The offscreen render target will serve as an input into
	// the compute shader for blurring, so we must unbind it from the OM stage before we
	// can use it as an input into the compute shader.
	renderTargets[0] = mRenderTargetView;
	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);

	mBlur.BlurInPlace(md3dImmediateContext, mOffscreenSRV, mOffscreenUAV, 8);

	// Draw fullscreen quad with texture of blurred scene on it.
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::DarkDirtyGreen2));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawScreenQuad();

	HR(mSwapChain->Present(0, 0));
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y) {
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y) {
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y) {
	if( (btnState & MK_LBUTTON) != 0 ) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxApp::DrawWrapper() {
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	md3dImmediateContext->IASetInputLayout(InputLayouts::InstancedMTR);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride[2] = {sizeof(Vertex::Position), sizeof(Instance::InstancedData)};
    UINT offset[2] = {0, 0};

	ID3D11Buffer* vbs[2] = {mSphereVB, mInstancedBuffer};
 
	XMMATRIX view     = mCam.View();
	XMMATRIX proj     = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();

	// Set per frame constants.
	Effects::InstancedBasicMTRFX->SetEyePosW(mCam.GetPosition());
	Effects::InstancedBasicMTRFX->SetFogColor(Colors::Silver);
	Effects::InstancedBasicMTRFX->SetFogStart(5.0f);
	Effects::InstancedBasicMTRFX->SetFogRange(100.0f);
 
	ID3DX11EffectTechnique* activeTech = Effects::InstancedBasicMTRFX->Tech1;
	 
    D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p) {
		md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
		md3dImmediateContext->IASetIndexBuffer(mSphereIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&mSphereWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

		Effects::InstancedBasicMTRFX->SetWorld(world);
		Effects::InstancedBasicMTRFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicMTRFX->SetViewProj(viewProj);

		//activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		//md3dImmediateContext->DrawIndexed(mSphereIndexCount, 0, 0);

		md3dImmediateContext->OMSetBlendState(RenderStates::NormalDoubleBS, blendFactor, 0xffffffff);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexedInstanced(mSphereIndexCount, mVisibleObjectCount, 0, 0, 0);

		md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
		md3dImmediateContext->RSSetState(0);
	}

	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	Effects::InstancedBasicFX->SetDiffuseMap(nullSRV[0]);
	activeTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
}

void BoxApp::DrawWrapper2() {

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormal);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT stride = sizeof(Vertex::PosNormal);
    UINT offset = 0;

	XMMATRIX view     = mCam.View();
	XMMATRIX proj     = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();
	XMMATRIX identity = XMMatrixIdentity();
 
	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
 
	// Figure out which technique to use.
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3Tech;
    D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc(&techDesc);
    for(UINT p = 0; p < techDesc.Passes; ++p) {
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mSheepVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mSheepIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&mSheepWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * view * proj;

		Effects::BasicFX->SetWorld(identity);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(mSphereMat);

		//md3dImmediateContext->RSSetState(RenderStates::NoCullRS);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mSheepIndexCount, 0, 0);

		//md3dImmediateContext->RSSetState(0);
    }
}

void BoxApp::DrawScreenQuad() {
	md3dImmediateContext->IASetInputLayout(InputLayouts::Post);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride = sizeof(Vertex::Post);
    UINT offset = 0;
 
	XMMATRIX identity = XMMatrixIdentity();
 
	ID3DX11EffectTechnique* texOnlyTech = Effects::PostFX->Tech1;
	D3DX11_TECHNIQUE_DESC techDesc;

	texOnlyTech->GetDesc(&techDesc);
	for(UINT p = 0; p < techDesc.Passes; ++p) {
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);

		Effects::PostFX->SetWorld(identity);
		Effects::PostFX->SetWorldInvTranspose(identity);
		Effects::PostFX->SetWorldViewProj(identity);
		Effects::PostFX->SetTexTransform(identity);
		//Effects::PostFX->SetDiffuseMap(mBlur.GetBlurredOutput());
		//Effects::PostFX->SetDiffuseMap2(mBlur.GetBlurredOutput());
		Effects::PostFX->SetDiffuseMap(mOffscreenSRV);
		Effects::PostFX->SetDiffuseMap2(mOffscreenSRV2);

		texOnlyTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(6, 0, 0);
    }

	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	Effects::PostFX->SetDiffuseMap(nullSRV[0]);
	Effects::PostFX->SetDiffuseMap2(nullSRV[0]);
	texOnlyTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
}

void BoxApp::BuildSheepGeometryBuffers() {

	ObjLoader loader = ObjLoader(std::string("sheep.obj"));

	std::vector<Vertex::PosNormal> vertices;
	std::vector<int> indices;
	mSheepIndexCount = loader.Groups[0].Faces.size() * 3;

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	int cc = 0;
	for(UINT i = 0; i < loader.Faces.size(); ++i) {
		ObjLoader::Face f = loader.Faces[i];
		for(UINT j = 0; j < f.vertices.size(); ++j) {
			ObjLoader::FaceVertex fv = f.vertices[j];
			Vertex::PosNormal v;
			v.Pos = loader.Vertices[fv.VertexIndex - 1].Pos;
			v.Normal = loader.Normals[fv.NormalIndex - 1];
			vertices.push_back(v);
			indices.push_back(cc++);

			XMVECTOR P = XMLoadFloat3(&v.Pos);

			vMin = XMVectorMin(vMin, P);
			vMax = XMVectorMax(vMax, P);
		}
	}

	XMStoreFloat3(&mSheepBox.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&mSheepBox.Extents, 0.5f * (vMax - vMin));

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormal) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices.data();

	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSheepVB));

	// Pack the indices of all the meshes into one index buffer.
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mSheepIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSheepIB));
}

void BoxApp::BuildCrateGeometryBuffers() {
	GeometryGenerator::MeshData sphere;

	GeometryGenerator geoGen;
	geoGen.CreateGeosphere(0.3f, 1, sphere);

	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	std::vector<Vertex::Position> vertices(sphere.Vertices.size());
	mSphereIndexCount = sphere.Indices.size();

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	for(UINT i = 0; i < sphere.Vertices.size(); ++i) {
		vertices[i].Pos    = sphere.Vertices[i].Position;

		XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	XMStoreFloat3(&mSphereBox.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&mSphereBox.Extents, 0.5f * (vMax - vMin));

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Position) * sphere.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSphereVB));

	// Pack the indices of all the meshes into one index buffer.
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * sphere.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &sphere.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSphereIB));
}

void BoxApp::BuildInstancedBuffer() {
	const int n = 20;
	mInstancedData.resize(n * n * n);
	
	float width = 20.0f;
	float height = 15.0f;
	float depth = 20.0f;
	
	float sqize = 0.25f;

	float x = -0.5f * width;
	float y = -0.5f * height * sqize;
	float z = -0.5f * depth;
	float dx = width / (n - 1);
	float dy = height / (n - 1) * sqize;
	float dz = depth / (n - 1);

	for(int k = 0; k < n; ++k) {
		for(int i = 0; i < n; ++i) {
			for(int j = 0; j < n; ++j) {
				// Position instanced along a 3D grid.
				float scale = MathHelper::RandF(0.025f, 0.15f);
				float cx = x + j * dx;
				float cy = y + i * dy;
				float cz = z + k * dz;
				XMVECTOR point = XMVectorSet(cx, cy, cz, 1.0f);
				XMVECTOR len = XMVector3Length(point);
				XMFLOAT4 lenf;
				XMStoreFloat4(&lenf, len);
				float grouping = lenf.x / 10.0f;
				grouping *= grouping * MathHelper::RandF(0.95f, 1.0f);
				XMMATRIX world(
					scale,	0.0f,	0.0f,	0.0f,
					0.0f,	scale,	0.0f,	0.0f,
					0.0f,	0.0f,	scale,	0.0f,
					cx * grouping,		cy * grouping,		cz * grouping,		1.0f
				);
				XMMATRIX rot = XMMatrixRotationY(MathHelper::Pi / 180.0f * lenf.x * 20.0f * MathHelper::RandF(0.765f, 0.965f));
				world = XMMatrixMultiply(world, rot);

				XMStoreFloat4x4(&mInstancedData[k * n * n + i * n + j].World, world);

				// Random color.
				mInstancedData[k * n * n + i * n + j].Color.x = MathHelper::RandF(0.965f, 0.985f);
				mInstancedData[k * n * n + i * n + j].Color.y = MathHelper::RandF(0.965f, 0.985f);
				mInstancedData[k * n * n + i * n + j].Color.z = MathHelper::RandF(0.998f + grouping, 0.996f);
				mInstancedData[k * n * n + i * n + j].Color.w = 1.0f;
			}
		}
	}
	
	D3D11_BUFFER_DESC vbd;
    //vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Instance::InstancedData) * mInstancedData.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &mInstancedData[0];
	
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mInstancedBuffer));
}

void BoxApp::BuildScreenQuadGeometryBuffers() {
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);

	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	std::vector<Vertex::Post> vertices(quad.Vertices.size());

	for(UINT i = 0; i < quad.Vertices.size(); ++i) {
		vertices[i].Pos    = quad.Vertices[i].Position;
		vertices[i].Tex    = quad.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Post) * quad.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mScreenQuadVB));

	// Pack the indices of all the meshes into one index buffer.
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * quad.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &quad.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mScreenQuadIB));
}

void BoxApp::BuildOffscreenViews() {
	// We call this function everytime the window is resized so that the render target is a quarter
	// the client area dimensions.  So Release the previous views before we create new ones.
	ReleaseCOM(mOffscreenSRV);
	ReleaseCOM(mOffscreenRTV);
	ReleaseCOM(mOffscreenUAV);

	D3D11_TEXTURE2D_DESC texDesc;
	
	texDesc.Width     = mClientWidth;
	texDesc.Height    = mClientHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;  
	texDesc.Usage          = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = 0;

	ID3D11Texture2D* offscreenTex = 0;
	HR(md3dDevice->CreateTexture2D(&texDesc, 0, &offscreenTex));

	// Null description means to create a view to all mipmap levels using 
	// the format the texture was created with.
	HR(md3dDevice->CreateShaderResourceView(offscreenTex, 0, &mOffscreenSRV));
	HR(md3dDevice->CreateRenderTargetView(offscreenTex, 0, &mOffscreenRTV));
	HR(md3dDevice->CreateUnorderedAccessView(offscreenTex, 0, &mOffscreenUAV));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(offscreenTex);

	// We call this function everytime the window is resized so that the render target is a quarter
	// the client area dimensions.  So Release the previous views before we create new ones.
	ReleaseCOM(mOffscreenSRV2);
	ReleaseCOM(mOffscreenRTV2);
	ReleaseCOM(mOffscreenUAV2);

	D3D11_TEXTURE2D_DESC texDesc2;
	
	texDesc2.Width     = mClientWidth;
	texDesc2.Height    = mClientHeight;
	texDesc2.MipLevels = 0;
	texDesc2.ArraySize = 1;
	texDesc2.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc2.SampleDesc.Count   = 1;  
	texDesc2.SampleDesc.Quality = 0;  
	texDesc2.Usage          = D3D11_USAGE_DEFAULT;
	texDesc2.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc2.CPUAccessFlags = 0; 
	texDesc2.MiscFlags      = 0;

	ID3D11Texture2D* offscreenTex2 = 0;
	HR(md3dDevice->CreateTexture2D(&texDesc2, 0, &offscreenTex2));

	// Null description means to create a view to all mipmap levels using 
	// the format the texture was created with.
	HR(md3dDevice->CreateShaderResourceView(offscreenTex2, 0, &mOffscreenSRV2));
	HR(md3dDevice->CreateRenderTargetView(offscreenTex2, 0, &mOffscreenRTV2));
	HR(md3dDevice->CreateUnorderedAccessView(offscreenTex2, 0, &mOffscreenUAV2));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(offscreenTex2);
}