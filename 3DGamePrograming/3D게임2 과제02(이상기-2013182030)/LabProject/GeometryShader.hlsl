#include"Common.hlsl"

[maxvertexcount(4)]
void GSBillBoard(point GS_BILLBOARD_INPUT input[1]
    , uint primID : SV_PrimitiveID
    , inout TriangleStream<PS_BILLBOARD_INPUT> outStream)
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