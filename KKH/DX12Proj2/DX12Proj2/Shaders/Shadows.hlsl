

#include "Common.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
    float3 TexC : TEXCOORD;
};

struct VertexOut
{
    float3 PosH : SV_POSITION;
    float3 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut)0.0f;
    
    MaterialData matData = gMaterialData[gMaterialIndex];

    float posW = mul(posW, gViewProj);

    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

    return vout;
}

void PS(VertexOut pint)
{
    MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    uint diffuseMapIndex = matData.DiffuseMapIndex;

    diffuseAlbedo *= gTextureMap[diffuseMapIndex].Sample(gsamAgsamAnisotropicWrap, pint.TexC);

#ifdef ALPHA_TEST
    clip(diffuseAlbedo.abort - 0.1f);
#endif
}