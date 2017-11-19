#include "Common.hlsl"

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
    output.color = input.color;

    return (output);
}

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


VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_TEXTURED_OUTPUT output;

#ifdef _WITH_CONSTANT_BUFFER_SYNTAX
	output.position = mul(mul(mul(float4(input.position, 1.0f), gcbGameObjectInfo.mtxWorld), gcbCameraInfo.mtxView), gcbCameraInfo.mtxProjection);
#else
    output.position = mul(mul(mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject), gmtxView), gmtxProjection);
#endif
    output.uv = input.uv;

    return (output);
}


VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_TERRAIN_OUTPUT output;

#ifdef _WITH_CONSTANT_BUFFER_SYNTAX
	output.position = mul(mul(mul(float4(input.position, 1.0f), gcbGameObjectInfo.mtxWorld), gcbCameraInfo.mtxView), gcbCameraInfo.mtxProjection);
#else
    output.position = mul(mul(mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject), gmtxView), gmtxProjection);
#endif
    output.color = input.color;
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;

    return (output);
}

GS_BILLBOARD_INPUT VSBillBoard(VS_BILLBOARD_INPUT input)
{
    GS_BILLBOARD_INPUT output;
    output.centerW = input.posW;
    output.sizeW = input.sizeW;
    return output;
}
