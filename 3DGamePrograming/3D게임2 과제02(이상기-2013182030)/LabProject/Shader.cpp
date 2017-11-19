#include "stdafx.h"
#include "Shader.h"

CShader::CShader()
{
}
CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		delete[] m_ppd3dPipelineStates;
	}
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(
	  WCHAR *pszFileName
	, LPCSTR pszShaderName
	, LPCSTR pszShaderProfile
	, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> pd3dErrorBlob;
	if (FAILED(::D3DCompileFromFile(
		pszFileName
		, NULL
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, pszShaderName
		, pszShaderProfile
		, nCompileFlags
		, 0
		, ppd3dShaderBlob
		, &pd3dErrorBlob)))
	{
		int size = pd3dErrorBlob->GetBufferSize();
		void *ack = pd3dErrorBlob->GetBufferPointer();

		if (ack)
		{
			char* str = new char[size];
			sprintf(str, (const char*)ack, size);
			OutputDebugStringA(str);
			delete[] str;
		}
		MessageBox(
			nullptr
			, TEXT("D3DCompileFromFile() Failed.")
			, TEXT("Error!"), MB_OK);
	}
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}
#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif
D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(
	  WCHAR *pszFileName
	, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;
	return(d3dInputLayoutDesc);
}
//래스터라이저 상태를 설정하기 위한 구조체를 반환한다.
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; // D3D12_FILL_MODE_WIREFRAME
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; // D3D12_CULL_MODE_NONE ||| D3D12_CULL_MODE_FRONT
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}
//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}
//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateGeometryShader(ID3DBlob ** ppd3dShaderBlob)
{
	return D3D12_SHADER_BYTECODE();
}

//그래픽스 파이프라인 상태 객체를 생성한다.
void CShader::CreateShader(
	  CD3DDeviceIndRes *pd3dDeviceIndRes
	, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	ComPtr<ID3DBlob> pd3dVertexShaderBlob = NULL;
	ComPtr<ID3DBlob> pd3dPixelShaderBlob = NULL;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDeviceIndRes->CreateGraphicsPipelineState(
		  &d3dPipelineStateDesc
		, &m_ppd3dPipelineStates[0]);
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) 
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateCbvAndSrvDescriptorHeaps(CD3DDeviceIndRes * pd3dDeviceIndRes, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	pd3dDeviceIndRes->CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		, nConstantBufferViews + nShaderResourceViews //CBVs + SRVs 
		, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		, 0
		, &m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (pd3dDeviceIndRes->nCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (pd3dDeviceIndRes->nCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

void CShader::CreateConstantBufferViews(CD3DDeviceIndRes * pd3dDeviceIndRes, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (pd3dDeviceIndRes->nCbvSrvDescriptorIncrementSize * j);
		pd3dDeviceIndRes->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void CShader::CreateShaderResourceViews(CD3DDeviceIndRes * pd3dDeviceIndRes, ID3D12GraphicsCommandList * pd3dCommandList, CTexture * pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
	int nTextures = pTexture->GetTextureCount();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pd3dDeviceIndRes->GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
		pd3dDeviceIndRes->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
		d3dSrvCPUDescriptorHandle.ptr += pd3dDeviceIndRes->nCbvSrvDescriptorIncrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, d3dSrvGPUDescriptorHandle);
		d3dSrvGPUDescriptorHandle.ptr += pd3dDeviceIndRes->nCbvSrvDescriptorIncrementSize;
	}
}

void CShader::CreateShaderVariables(
	CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CShader::ReleaseShaderVariables()
{
}
void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CShader::UpdateShaderVariable(
	ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
}

void CShader::ReleaseUploadBuffers()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0].Get());
	if (m_pd3dCbvSrvDescriptorHeap != NULL)
	{
		ID3D12DescriptorHeap* ptr = m_pd3dCbvSrvDescriptorHeap.Get();
		pd3dCommandList->SetDescriptorHeaps(1, &ptr);
	}
	
	
}
void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

////////////////////////////////////////////////////////////////////////
CDiffusedShader::CDiffusedShader()
{
}
CDiffusedShader::~CDiffusedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CDiffusedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CDiffusedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"VertexShader.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CDiffusedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
}

void CDiffusedShader::CreateShader(
	CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ComPtr<ID3D12PipelineState>[m_nPipelineStates];
	CShader::CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
}
////////////////////////////////////////////////////////////////////////
CPlayerShader::CPlayerShader()
{
}
CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"VertexShader.hlsl", "VSPlayer", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PSPlayer", "ps_5_1", ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ComPtr<ID3D12PipelineState>[m_nPipelineStates];

	CShader::CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
}

void CPlayerShader::CreateShaderVariables(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbPlayer = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, NULL
		, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, NULL);

	m_pd3dcbPlayer->Map(0, NULL, (void **)&m_pcbMappedPlayer);
}

void CPlayerShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayerShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	::memcpy(&m_pcbMappedPlayer->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
}

void CPlayerShader::ReleaseShaderVariables()
{
	if (m_pd3dcbPlayer)
	{
		m_pd3dcbPlayer->Unmap(0, NULL);
		m_pd3dcbPlayer->Release();
	}
}

void CPlayerShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

/////////////////////////////////////////////////////////////////////////////////
CObjectsShader::CObjectsShader()
{
}
CObjectsShader::~CObjectsShader()
{
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = 
		new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { 
		"POSITION", 0
		, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0
		, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"VertexShader.hlsl", "VSLighting", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PSLighting", "ps_5_1",
		ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(
	CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ComPtr<ID3D12PipelineState>[m_nPipelineStates];
	CShader::CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
}
void CObjectsShader::CreateShaderVariables(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, NULL
		, ncbGameObjectBytes * m_nObjects
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_GENERIC_READ
		, NULL);
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		XMStoreFloat4x4(
			&m_pcbMappedGameObjects[j].m_xmf4x4World,
			XMMatrixTranspose(
				XMLoadFloat4x4(&m_ppObjects[j]->GetWorldMtx())));
		m_pcbMappedGameObjects[j].m_nMaterial = m_ppObjects[j]->GetMaterial()->m_nReflection;;
	}
	pd3dCommandList->SetGraphicsRootShaderResourceView(
		2, m_pd3dcbGameObjects->GetGPUVirtualAddress());
}

void CObjectsShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
	}
}

void CObjectsShader::BuildObjects(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList
	*pd3dCommandList, CGameObject **ppObjects, int nObjects)
{
	m_nObjects = nObjects;
	m_ppObjects = ppObjects;

	CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
}

void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	UpdateShaderVariables(pd3dCommandList);
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects);
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) 
			m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

CGameObject *CObjectsShader::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition,
	XMFLOAT4X4& xmf4x4View, float *pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = NULL;
	for (int j = 0; j < m_nObjects; j++)
	{
		nIntersected = m_ppObjects[j]->PickObjectByRayIntersection(xmf3PickPosition,
			xmf4x4View, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = m_ppObjects[j];
		}
	}
	return(pSelectedObject);
}
/////////////////////////////////////////////////////////////////////////////////
CInstancingShader::CInstancingShader()
{
}
CInstancingShader::~CInstancingShader()
{
}

D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = 
		new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = {
		"POSITION", 0
		, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0
		, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"VertexShader.hlsl", "VSLighting", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PSLighting", "ps_5_1",
		ppd3dShaderBlob));
}
void CInstancingShader::CreateShader(
	CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ComPtr<ID3D12PipelineState>[m_nPipelineStates];
	CShader::CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShaderVariables(CD3DDeviceIndRes *pd3dDeviceIndRes,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dcbGameObjects = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, NULL
		, sizeof(CB_GAMEOBJECT_INFO) * m_nObjects
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_GENERIC_READ
		, NULL);
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}
void CInstancingShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
}
//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다.
void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		XMStoreFloat4x4(
			&m_pcbMappedGameObjects[j].m_xmf4x4World,
			XMMatrixTranspose(
				XMLoadFloat4x4(&m_ppObjects[j]->GetWorldMtx())));
		m_pcbMappedGameObjects[j].m_nMaterial = m_ppObjects[j]->GetMaterial()->m_nReflection;;
	}
	pd3dCommandList->SetGraphicsRootShaderResourceView(
		2, m_pd3dcbGameObjects->GetGPUVirtualAddress());
}

void CInstancingShader::BuildObjects(
	CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList,
	CGameObject **ppObjects, int nObjects)
{
	m_nObjects = nObjects;
	m_ppObjects = ppObjects;
	//인스턴싱을 위한 정점 버퍼와 뷰를 생성한다.
	CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
}

void CInstancingShader::ReleaseObjects()
{
}

void CInstancingShader::Render(
	ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다.
	UpdateShaderVariables(pd3dCommandList);
	//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다.
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects);
}
/////////////////////////////////////////////////////////////////////////////////
CTerrainShader::CTerrainShader()
{
}
CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = {
		"POSITION", 0
		, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { 
		"NORMAL", 0
		, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"VertexShader.hlsl", "VSLighting", "vs_5_1",
		ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PSLighting", "ps_5_1",
		ppd3dShaderBlob));
}
void CTerrainShader::SetTerrain(CGameObject * terrain)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[1];
	m_ppObjects[0] = terrain;
}
////////////////////////////////////////////////////////////////////////
CTexturedShader::CTexturedShader()
{
}
CTexturedShader::~CTexturedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTexturedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTexturedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"VertexShader.hlsl", "VSTextured", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PSTextured", "ps_5_1", ppd3dShaderBlob));
}

void CTexturedShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	UpdateShaderVariables(pd3dCommandList);
}

///////////////////////////////////////////////////////////////////

CTexturedTerrainShader::CTexturedTerrainShader()
{
}
CTexturedTerrainShader::~CTexturedTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTexturedTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTexturedTerrainShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		  L"VertexShader.hlsl"
		, "VSTerrain"
		, "vs_5_1"
		, ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedTerrainShader::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		  L"PixelShader.hlsl"
		, "PSTerrain"
		, "ps_5_1"
		, ppd3dShaderBlob));
}

void CTexturedTerrainShader::SetTerrain(CGameObject * terrain)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[1];
	m_ppObjects[0] = terrain;
}

///////////////////////////////////////////////////////////////////
CSkyBoxShader::CSkyBoxShader()
{
}
CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"PixelShader.hlsl", "PSSkyBox", "ps_5_1", ppd3dShaderBlob));
}

void CSkyBoxShader::SetSkyBox(CGameObject* skybox)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[1];
	m_ppObjects[0] = skybox;
}

CBillBoardShader::CBillBoardShader()
	: m_pd3dVertexBuffer(NULL)
	, m_pd3dVertexUploadBuffer(NULL)
	, m_d3dPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST)
	, m_pBillBoards(NULL)
	, m_nBillBoards(0)
	, m_nSlot(0)
	, m_nStride(0)
	, m_nOffset(0)
{
	memset(&m_d3dVertexBufferView, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
}
CBillBoardShader::~CBillBoardShader()
{
	if (m_pBillBoards) delete[] m_pBillBoards;
	if (m_pTexture) delete m_pTexture;
}
void CBillBoardShader::CreateShader(CD3DDeviceIndRes * pd3dDeviceIndRes, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ComPtr<ID3D12PipelineState>[m_nPipelineStates];

	ComPtr<ID3DBlob> pd3dVertexShaderBlob = NULL;
	ComPtr<ID3DBlob> pd3dPixelShaderBlob = NULL;
	ComPtr<ID3DBlob> pd3dGeometryShaderBlob = NULL;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.GS = CreateGeometryShader(&pd3dGeometryShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDeviceIndRes->CreateGraphicsPipelineState(
		&d3dPipelineStateDesc
		, &m_ppd3dPipelineStates[0]);
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CBillBoardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "SIZE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CBillBoardShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; // D3D12_FILL_MODE_WIREFRAME
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE; // D3D12_CULL_MODE_BACK ||| D3D12_CULL_MODE_FRONT
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

D3D12_SHADER_BYTECODE CBillBoardShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"VertexShader.hlsl", "VSBillBoard", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CBillBoardShader::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"PixelShader.hlsl", "PSBillBoard", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CBillBoardShader::CreateGeometryShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"GeometryShader.hlsl", "GSBillBoard", "gs_5_1", ppd3dShaderBlob));
}

void CBillBoardShader::CreateShaderVariables(CD3DDeviceIndRes * pd3dDeviceIndRes, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pd3dVertexBuffer = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, m_pBillBoards
		, m_nStride * m_nBillBoards
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nBillBoards;
}

void CBillBoardShader::BuildObjects(
	CD3DDeviceIndRes * pd3dDeviceIndRes
	, ID3D12GraphicsCommandList * pd3dCommandList
	, CBillBoardVertex *pBillBoards
	, int nBillBoards)
{
	m_nBillBoards = nBillBoards;
	m_pBillBoards = pBillBoards;
	m_nStride = sizeof(CBillBoardVertex);

	
}

void CBillBoardShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	
	pd3dCommandList->DrawInstanced(m_nBillBoards, 1, m_nOffset, 0);
}

