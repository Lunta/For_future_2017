
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

static matrix gmtxViewProjection = mul(gmtxView, gmtxProjection);
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


Texture2D gtxtTextures[6] : register(t1);

SamplerState gWrapSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);

struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTexture : register(t2);

struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtSkyBox : register(t1);

////////////////////////////////////////////////////////////////////
struct VS_BILLBOARD_INPUT
{
    float3 posW : POSITION;
    float2 sizeW : SIZE;
};

struct GS_BILLBOARD_INPUT
{
    float3 centerW : POSITION;
    float2 sizeW : SIZE;
};

struct PS_BILLBOARD_INPUT
{
    float4 posH : SV_POSITION;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

Texture2D gtxtBillBoard[6] : register(t1);