
#include "LightingUtil.hlsl"

Texture2D gDiffuseMap : register(t0);

SamplerState gsamPointWrap			:	register(s0);
SamplerState gsamPointClamp			:	register(s1);
SamplerState gsamLinearWrap			:	register(s2);
SamplerState gsamLinearClamp		:	register(s3);
SamplerState gsamAnisotropicWrap	:	register(s4);
SamplerState gsamAnisotropicClamp	:	register(s5);

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
};

cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;

	float4 gFogColor;
	float gFogStart;
	float gFogRange;
	float2 cbPerObjectPad2;

	Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float gRoughness;
	float4x4 gMatTransform;
};

//=======[ VS ] =========
struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float3 PosL	: POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosL = vin.PosL;

	return vout;
}
//=======================
//=======[ HS ] =========
struct PatchTess
{
	float EdgeTess[4]	:	SV_TessFactor;
	float InsideTess[2]	:	SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<VertexOut, 16> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	pt.EdgeTess[0] = 25;
	pt.EdgeTess[1] = 25;
	pt.EdgeTess[2] = 25;
	pt.EdgeTess[3] = 25;

	pt.InsideTess[0] = 25;
	pt.InsideTess[1] = 25;

	return pt;
}

struct HullOut
{
	float3 PosL : POSITION;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 16 > p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	hout.PosL = p[i].PosL;

	return hout;
}

//=======================
//=======[ DS ] =========

struct DomainOut
{
	float4 PosH : SV_POSITION;
};

float4 BernsteinBasis(float t)
{
	float invT = 1.0f - t;

	return float4(invT * invT * invT,
					3.0f * t * invT * invT,
					3.0f * t * t * invT,
					t * t * t);
}

float3 CubicBezierSum(const OutputPatch<HullOut, 16> bazpatch, float4 basisU, float4 basisV)
{
	float3 sum = float3(0.0f, 0.0f, 0.0f);

	sum  = basisV.x * (basisU.x * bazpatch[0].PosL  + basisU.y * bazpatch[1].PosL + basisU.z  * bazpatch[2].PosL  + basisU.w * bazpatch[3].PosL);
	sum += basisV.y * (basisU.x * bazpatch[4].PosL  + basisU.y * bazpatch[5].PosL + basisU.z  * bazpatch[6].PosL  + basisU.w * bazpatch[7].PosL);
	sum += basisV.z * (basisU.x * bazpatch[8].PosL  + basisU.y * bazpatch[9].PosL + basisU.z  * bazpatch[10].PosL + basisU.w * bazpatch[11].PosL);
	sum += basisV.w * (basisU.x * bazpatch[12].PosL + basisU.y * bazpatch[13].PosL + basisU.z * bazpatch[14].PosL + basisU.w * bazpatch[15].PosL);

	return sum;
}

float dBernsteinBasis(float t)
{
	float invT = 1.0f - t;

	return float4(-3 * invT * invT,
					3 * invT * invT - 6 * t * invT,
					6 * t * invT - 3 * t * t,
					3 * t * t);
}

[domain("quad")]
DomainOut DS(PatchTess patchTess,
	float2 uv : SV_DomainLocation,
	const OutputPatch<HullOut, 16> bezPatch)
{
	DomainOut dout;

	float4 basisU = BernsteinBasis(uv.x);
	float4 basisV = BernsteinBasis(uv.y);

	float3 p = CubicBezierSum(bezPatch, basisU, basisV);

	float4 posW = mul(float4(p, 1.0f), gWorld);
	dout.PosH = mul(posW, gViewProj);

	return dout;
}

//=======================
//=======[ PS ] =========

float4 PS(DomainOut pin) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}