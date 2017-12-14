cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};

#include "Light.hlsl"

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

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

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

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

Texture2D gtxtSkyBox : register(t0);

float4 PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBox.Sample(gSamplerState, input.uv);

	return (cColor);
}

#define _WITH_VERTEX_LIGHTING

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(cIllumination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(lerp(cColor, cIllumination, 0.5f));
}


struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
Texture2D gtxtTerrainBaseTexture : register(t0);
Texture2D gtxtTerrainDetailTexture : register(t1);
Texture2D gtxtTerrainDetailTextureNormal : register(t2);

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return (output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);

	float3 normal = float3(gtxtTerrainDetailTextureNormal.Sample(gSamplerState, input.uv1).xyz);
	normal = 2.0f * normal - 1.0f;
	float3 normalW = mul(normal, TBN);
	float leanFactor = dot(float3(0, 1, 0), normalW);
	if (leanFactor < 0) 
		leanFactor = -leanFactor;

	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gSamplerState, input.uv1);
	float4 cColor = saturate((cBaseTexColor * leanFactor) + (cDetailTexColor * (1.0f - leanFactor)));
	float4 cIllumination = Lighting(input.positionW, normalW);
	cColor = saturate((cColor * 0.5f) + (cIllumination * 0.5f));
	return (cColor);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
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

Texture2D gtxtBillBoard : register(t0);

GS_BILLBOARD_INPUT VSBillBoard(VS_BILLBOARD_INPUT input)
{
	GS_BILLBOARD_INPUT output;
	output.centerW = input.posW;
	output.sizeW = input.sizeW;
	return output;
}

[maxvertexcount(4)]
void GSBillBoard(
	point GS_BILLBOARD_INPUT					input[1]
	, uint										primID : SV_PrimitiveID
	, inout TriangleStream<PS_BILLBOARD_INPUT>	outStream)
{
	float3 vUp = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].centerW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);

	float fHalfW = input[0].sizeW.x * 0.5f;
	float fHalfH = input[0].sizeW.y * 0.5f;
	float3 vWidth = fHalfW * vRight;
	float3 vHeight = fHalfH * vUp;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].centerW + vWidth - vHeight, 1.0f);
	pVertices[1] = float4(input[0].centerW + vWidth + vHeight, 1.0f);
	pVertices[2] = float4(input[0].centerW - vWidth - vHeight, 1.0f);
	pVertices[3] = float4(input[0].centerW - vWidth + vHeight, 1.0f);

	float2 pUVs[4] =
	{
		float2(0.0f, 1.0f)
		, float2(0.0f, 0.0f)
		, float2(1.0f, 1.0f)
		, float2(1.0f, 0.0f)
	};

	PS_BILLBOARD_INPUT output;
	for (int i = 0; i < 4; i++)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.uv = pUVs[i];
		output.primID = primID;
		outStream.Append(output);
	}
}

float4 PSBillBoard(PS_BILLBOARD_INPUT input) : SV_Target
{
	float4 cTexture = gtxtBillBoard.Sample(gSamplerState, input.uv);
	if (cTexture.a < 0.01f) discard;

	float4 cIlumination = Lighting(input.posW, input.normalW);
	//float3 uvw = float3(input.uv, (input.primID % 6));
	//float4 cTexture = gtxtBillBoard.Sample(gWrapSamplerState, uvw);
	//float4 cColor = cIlumination * cTexture;
	float4 cColor = cTexture;
	cColor.a = cTexture.a;

	return (cColor);
	// return float4(1, 1, 1, 1);
}