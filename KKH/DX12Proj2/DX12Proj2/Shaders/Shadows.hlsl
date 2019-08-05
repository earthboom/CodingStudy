

#include "Common.hlsl"

struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 TangentW : TANGENT;
};