#include "Common.hlsl"

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (input.color);
#else
    input.normalW = normalize(input.normalW);
    gnMaterial = input.material;
    float4 color = Lighting(input.positionW, input.normalW);
    return (color);
#endif
}

float4 PSTextured(VS_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
/*
	float4 cColor;
	if (nPrimitiveID < 2)
		cColor = gtxtTextures[0].Sample(gWrapSamplerState, input.uv);
	else if (nPrimitiveID < 4)
		cColor = gtxtTextures[1].Sample(gWrapSamplerState, input.uv);
	else if (nPrimitiveID < 6)
		cColor = gtxtTextures[2].Sample(gWrapSamplerState, input.uv);
	else if (nPrimitiveID < 8)
		cColor = gtxtTextures[3].Sample(gWrapSamplerState, input.uv);
	else if (nPrimitiveID < 10)
		cColor = gtxtTextures[4].Sample(gWrapSamplerState, input.uv);
	else
		cColor = gtxtTextures[5].Sample(gWrapSamplerState, input.uv);
*/
    float4 cColor = gtxtTextures[NonUniformResourceIndex(nPrimitiveID / 2)].Sample(gWrapSamplerState, input.uv);

    return (cColor);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gWrapSamplerState, input.uv0);
    float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gWrapSamplerState, input.uv1);
    float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

    return (cColor);
}

float4 PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtSkyBox.Sample(gClampSamplerState, input.uv);

    return (cColor);
}

float4 PSBillBoard(PS_BILLBOARD_INPUT input) : SV_Target
{
    float4 cTexture = gtxtBillBoard[NonUniformResourceIndex(input.primID % 6)].Sample(gWrapSamplerState, input.uv);
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