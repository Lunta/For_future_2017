
cbuffer cbPlayerInfo : register(b0)
{
    matrix gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

struct GAMEOBJECTINFO
{
    matrix m_mtxGameObject;
    uint m_nMaterial;
};

StructuredBuffer<GAMEOBJECTINFO> gGameObjectInfos : register(t0);

static uint gnMaterial;

#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSED_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
    output.color = input.color;

    return (output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    uint material : MATERIAL;
//	nointerpolation float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
    
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
    gnMaterial = gGameObjectInfos[nInstanceID].m_nMaterial;
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#else
    output.material = gGameObjectInfos[nInstanceID].m_nMaterial;
#endif
    return (output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (input.color);
#else
	input.normalW = normalize(input.normalW);
    gnMaterial = input.material;
	float4 color = Lighting(input.positionW, input.normalW);
	return(color);
#endif
}