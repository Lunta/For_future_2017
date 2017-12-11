#pragma once

#include "GameObject.h"
#include "Camera.h"

class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void CreateCbvAndSrvDescriptorHeaps(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	void CreateConstantBufferViews(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	void CreateShaderResourceViews(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual void CreateShaderVariables(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);

	virtual void BuildObjects(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

	virtual void ReleaseUploadBuffers();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

protected:
	ComPtr<ID3D12PipelineState>*	m_ppd3dPipelineStates = NULL;
	int								m_nPipelineStates = 0;

	ComPtr<ID3D12DescriptorHeap>	m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CIlluminatedShader : public CShader
{
public:
	CIlluminatedShader();
	virtual ~CIlluminatedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CIlluminatedTexturedShader : public CTexturedShader
{
public:
	CIlluminatedTexturedShader();
	virtual ~CIlluminatedTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define _WITH_BATCH_MATERIAL

class CObjectsShader : public CIlluminatedTexturedShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void CreateShaderVariables(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	CGameObject * *m_ppObjects = 0;
	int								m_nObjects = 0;

	ID3D12Resource					*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO				*m_pcbMappedGameObjects = NULL;

#ifdef _WITH_BATCH_MATERIAL
	CMaterial						*m_pMaterial = NULL;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTerrainShader : public CTexturedShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxShader : public CTexturedShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);
};


class CBillBoardShader : public CShader
{
public:
	CBillBoardShader();
	virtual ~CBillBoardShader();

	virtual void CreateShader(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShaderVariables(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void BuildObjects(
		  CD3DDeviceIndRes*				pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*	pd3dCommandList
		, CBillBoardVertex*				pBillBoards = NULL
		, int							nBillBoards = 0);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void SetTexture(CTexture* pTexture) { m_pTexture = pTexture; }

protected:
	CTexture						*m_pTexture = NULL;
	CBillBoardVertex				*m_pBillBoards = NULL;
	UINT							m_nBillBoards = 0;
	UINT							m_nStride;
	UINT							m_nSlot;
	UINT							m_nOffset;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;

	ComPtr<ID3D12Resource>			m_pd3dVertexBuffer;
	ComPtr<ID3D12Resource>			m_pd3dVertexUploadBuffer;
};