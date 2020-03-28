#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::WireframeRS = 0;
ID3D11RasterizerState* RenderStates::NoCullRS = 0;
ID3D11RasterizerState* RenderStates::NormalRS = 0;

ID3D11BlendState* RenderStates::AlphaToCoverageBS = 0;
ID3D11BlendState* RenderStates::TransparentBS = 0;
ID3D11BlendState* RenderStates::NormalBS = 0;
ID3D11BlendState* RenderStates::NormalDoubleBS = 0;

void RenderStates::InitAll(ID3D11Device* device) {
	// WireframeRS
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

	// NoCullRS
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

	// NormalRS
	D3D11_RASTERIZER_DESC normalRSDesc;
	ZeroMemory(&normalRSDesc, sizeof(D3D11_RASTERIZER_DESC));
	normalRSDesc.FillMode = D3D11_FILL_SOLID;
	normalRSDesc.CullMode = D3D11_CULL_BACK;
	normalRSDesc.FrontCounterClockwise = false;
	normalRSDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&normalRSDesc, &NormalRS));

	// AlphaToCoverageBS
	D3D11_BLEND_DESC alphaToCoverageDesc = {0};
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

	// TransparentBS
	D3D11_BLEND_DESC transparentDesc = {0};
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;
	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));

	// NormalBS
	D3D11_BLEND_DESC normalDesc = {0};
	normalDesc.AlphaToCoverageEnable = false;
	normalDesc.IndependentBlendEnable = false;
	normalDesc.RenderTarget[0].BlendEnable = false;
	normalDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	normalDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	normalDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	normalDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	normalDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	normalDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	normalDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&normalDesc, &NormalBS));

	// NormalDoubleBS
	D3D11_BLEND_DESC normalDoubleDesc = {0};
	normalDoubleDesc.AlphaToCoverageEnable = false;
	normalDoubleDesc.IndependentBlendEnable = true;
	normalDoubleDesc.RenderTarget[0].BlendEnable = false;
	normalDoubleDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	normalDoubleDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	normalDoubleDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	normalDoubleDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	normalDoubleDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	normalDoubleDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	normalDoubleDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	normalDoubleDesc.RenderTarget[1].BlendEnable = false;
	normalDoubleDesc.RenderTarget[1].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	normalDoubleDesc.RenderTarget[1].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	normalDoubleDesc.RenderTarget[1].BlendOp        = D3D11_BLEND_OP_ADD;
	normalDoubleDesc.RenderTarget[1].SrcBlendAlpha  = D3D11_BLEND_ONE;
	normalDoubleDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
	normalDoubleDesc.RenderTarget[1].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	normalDoubleDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&normalDoubleDesc, &NormalDoubleBS));
}

void RenderStates::DestroyAll() {
	ReleaseCOM(WireframeRS);
	ReleaseCOM(NoCullRS);
	ReleaseCOM(NormalRS);
	ReleaseCOM(AlphaToCoverageBS);
	ReleaseCOM(TransparentBS);
	ReleaseCOM(NormalBS);
	ReleaseCOM(NormalDoubleBS);
}