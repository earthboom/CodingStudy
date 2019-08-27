
// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "Common.hlsl"

//struct InstanceData
//{
//	float4x4 World;
//	float4x4 TexTransform;
//	uint     MaterialIndex;
//	uint     InstPad0;
//	uint     InstPad1;
//	uint     InstPad2;
//};

struct VertexIn
{
	float3 PosL		    :	POSITION;
	float3 NormalL	    :	NORMAL;
	float2 TexC		    :	TEXCOORD;
	float3 TangentL	    :	TANGENT;
#ifdef SKINNED
    float3 BoneWeights  : WEIGHTS;
    uint4 BoneIndices   : BONEINDICES;
#endif
};

struct VertexOut
{
	float4 PosH			:	SV_POSITION;
	float4 ShadowPosH	:	POSITION0;
    float4 SsaoPosH     :   POSITION1;
	float3 PosW			:	POSITION2;
	float3 NormalW		:	NORMAL;
	float3 TangentW		:	TANGENT;
	float2 TexC			:	TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// Fetch the instance data.
	//InstanceData instData = gInstanceData[instanceID];
	//float4x4 world = instData.World;
	//float4x4 texTransform = instData.TexTransform;
	//uint matIndex = instData.MaterialIndex;

	//vout.MatIndex = matIndex;

	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];

#ifdef SKINNED
    float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    weights[0]= vin.BoneWeights.x;
    weights[1]= vin.BoneWeights.y;
    weights[2]= vin.BoneWeights.z;
    weights[3]= 1.0f - weights[0] - weights[1] - weights[2];

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);

    for(int i=0; i<4; ++i)
    {
        posL += weights[i] * mul(float4(vin.posL, 1.0f), gBoneTransform[vin.BoneIndices[i]]).xyz;
        normalL += weights[i] * mul(vin.NormalL, (float3x3)gBoneTransform[vin.BoneIndices[i]]);
        tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3)gBoneTransform[vin.BoneIndices[i]]);
    }

    vin.posL = posL;
    vin.normalL = normalL;
    vin.tangentL = tangentL;
#endif

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(posW, gViewProj);

    vout.SsaoPosH = mul(posW, gViewProjTex);

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

	vout.ShadowPosH = mul(posW, gShadowTransform);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float  roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;
	uint normalMapIndex = matData.NormalMapIndex;

	// Dynamically look up the texture in the array.
	diffuseAlbedo *= gTextureMaps[diffuseTexIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#ifdef ALPHA_TEST
	clip(diffuseAlbedo.a - 0.1f);
#endif 

	// Interpolating normal can unnormalize it, so renormalize it.
	pin.NormalW = normalize(pin.NormalW);

	float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	// Vector from point being lit to eye. 
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Light terms.
	float4 ambient = gAmbientLight * diffuseAlbedo;

	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);

	const float shininess = (1.0f - roughness) * normalMapSample.a;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		bumpedNormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	float3 r = reflect(-toEyeW, bumpedNormalW);
	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
	float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

	// Common convention to take alpha from diffuse albedo.
	litColor.a = diffuseAlbedo.a;

	return litColor;
}


