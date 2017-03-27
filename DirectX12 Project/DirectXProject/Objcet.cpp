#include "stdafx.h"

#include "Objcet.h"


CObjcet::CObjcet(ID3D11Device *pD3D_Device, ID3D11DeviceContext *pD3D_DC)
{
	InitPipeline(pD3D_Device, pD3D_DC);
}

CObjcet::~CObjcet()
{
	m_pLayout->Release();
	m_pVS->Release();
	m_pPS->Release();
}

void CObjcet::InitPipeline(ID3D11Device * pD3D_Device, ID3D11DeviceContext * pD3D_DC)
{
	// load and compile the two shaders
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"Shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"Shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	// encapsulate both shaders into shader objects
	pD3D_Device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVS);
	pD3D_Device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPS);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	pD3D_Device->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &m_pLayout);

	// set the shader objects
	pD3D_DC->VSSetShader(m_pVS, 0, 0);
	pD3D_DC->PSSetShader(m_pPS, 0, 0);
	pD3D_DC->IASetInputLayout(m_pLayout);
}
