cbuffer cbPerFrame {
	float gTime;
};

cbuffer cbPerObject {
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gDiffuseMap2;

SamplerState samAnisotropic {
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VertexIn {
	float3 PosL    : POSITION;
	float2 Tex     : TEXCOORD;
};

struct VertexOut {
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
	float2 Tex     : TEXCOORD;
};

VertexOut VS(VertexIn vin) {
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}
 
float4 PS(VertexOut pin) : SV_Target {

    float4 texColor1 = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
	float4 texColor2 = gDiffuseMap2.Sample(samAnisotropic, pin.Tex);

	float4 litColor = texColor1 * 2.0f + texColor2 * 0.45f;
	//float4 litColor =  max(texColor1, texColor2);

	

	float Contrast = 1.5f;
	litColor = float4(((litColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f, 1.0f);

	float Brightness = 0.2f;
	litColor.rgb += Brightness;
	
	
    return litColor;
}

technique11 Tech1 {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}