cbuffer cbPerFrame {
	float3 gEyePosW;
	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject {
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gViewProj;
};

struct VertexIn {
	float3 PosL     : POSITION;
	row_major float4x4 World  : WORLD;
	float4 Color    : COLOR;
	uint InstanceId : SV_InstanceID;
};

struct VertexOut {
	float4 PosH		: SV_POSITION;
    float3 PosW		: POSITION;
	float4 Color	: COLOR;
};

struct PixelOut {
    float4 Color0	: SV_Target0;
    float4 Color1	: SV_Target1;
};

VertexOut VS(VertexIn vin) {
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), vin.World).xyz;
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Color = vin.Color;

	return vout;
}
 
PixelOut PS(VertexOut pin, uniform bool gFogEnabled) {
	PixelOut pout;

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
	float4 litColor = pin.Color;;

	// Fogging
	if(gFogEnabled) {
		float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

		// Blend the fog color and the lit color.
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	pout.Color0 = litColor;
	pout.Color1 = litColor;

	return pout;
}

technique11 Tech1 {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(false)));
    }
}

technique11 Tech2 {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }
}