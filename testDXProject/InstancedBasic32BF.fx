struct DirectionalLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight { 
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec) {
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction;

	// Add ambient term.
	ambient = mat.Ambient * L.Ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if(diffuseFactor > 0.0f) {
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec) {
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if(d > L.Range)
		return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Ambient term.
	ambient = mat.Ambient * L.Ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if(diffuseFactor > 0.0f) {
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}

	// Attenuate
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));

	diffuse *= att;
	spec    *= att;
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a spotlight.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec) {
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if(d > L.Range)
		return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Ambient term.
	ambient = mat.Ambient * L.Ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if(diffuseFactor > 0.0f) {
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}
	
	// Scale by spotlight factor and attenuate.
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

	// Scale by spotlight factor and attenuate.
	float att = spot / dot(L.Att, float3(1.0f, d, d * d));

	ambient *= spot;
	diffuse *= att;
	spec    *= att;
}

cbuffer cbPerFrame {
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject {
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

SamplerState samAnisotropic {
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn {
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float2 Tex      : TEXCOORD;
	row_major float4x4 World  : WORLD;
	float4 Color    : COLOR;
	uint InstanceId : SV_InstanceID;
};

struct VertexOut {
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
	float4 Color   : COLOR;
};

VertexOut VS(VertexIn vin) {
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), vin.World).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)vin.World);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex   = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vout.Color = vin.Color;

	return vout;
}
 
float4 PS(VertexOut pin, uniform int gLightCount, uniform bool gUseTexure, uniform bool gAlphaClip, uniform bool gFogEnabled) : SV_Target {
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
    if(gUseTexure) {
		// Sample texture.
		texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);

		if(gAlphaClip) {
			// Discard pixel if texture alpha < 0.1.  Note that we do this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(texColor.a - 0.1f);
		}
	}
	 
	// Lighting.
	float4 litColor = texColor;
	if(gLightCount > 0) {  
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
		[unroll]
		for(int i = 0; i < gLightCount; ++i) {
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye, A, D, S);

			ambient += A * pin.Color;
			diffuse += D * pin.Color;
			spec    += S;
		}

		// Modulate with late add.
		litColor = texColor * (ambient + diffuse) + spec;
	}

	// Fogging
	if(gFogEnabled) {
		float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 

		// Blend the fog color and the lit color.
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
}

technique11 Light1 {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, false)));
    }
}

technique11 Light2 {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, false)));
    }
}

technique11 Light3 {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, false)));
    }
}

technique11 Light0Tex {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, false)));
    }
}

technique11 Light1Tex {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, false)));
    }
}

technique11 Light2Tex {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, false)));
    }
}

technique11 Light3Tex {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, false)));
    }
}

technique11 Light0TexAlphaClip {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, false)));
    }
}

technique11 Light1TexAlphaClip {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, false)));
    }
}

technique11 Light2TexAlphaClip {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, false)));
    }
}

technique11 Light3TexAlphaClip {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, false)));
    }
}

technique11 Light1Fog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, true)));
    }
}

technique11 Light2Fog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, true)));
    }
}

technique11 Light3Fog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, true)));
    }
}

technique11 Light0TexFog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, true)));
    }
}

technique11 Light1TexFog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, true)));
    }
}

technique11 Light2TexFog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, true)));
    }
}

technique11 Light3TexFog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, true)));
    }
}

technique11 Light0TexAlphaClipFog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, true)));
    }
}

technique11 Light1TexAlphaClipFog {
    pass P0 {
        SetVertexShader(CompileShader( vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, true)));
    }
}

technique11 Light2TexAlphaClipFog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, true)));
    }
}

technique11 Light3TexAlphaClipFog {
    pass P0 {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, true)));
    }
}