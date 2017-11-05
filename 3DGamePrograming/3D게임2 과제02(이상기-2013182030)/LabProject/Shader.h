#pragma once

#include "GameObject.h"
#include "Camera.h"

class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int m_nReferences{ 0 };

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	int							m_nPipelineStates{ 0 };
	ComPtr<ID3D12PipelineState> *m_ppd3dPipelineStates{ NULL };

public:
	D3D12_SHADER_BYTECODE CompileShaderFromFile(
		WCHAR *pszFileName
		, LPCSTR pszShaderName
		, LPCSTR pszShaderProfile
		, ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(
		WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob = NULL);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(
		CD3DDeviceIndRes *pd3dDeviceIndRes
		, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(
		CD3DDeviceIndRes *pd3dDeviceIndRes
		, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(
		ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariable(
		ID3D12GraphicsCommandList *pd3dCommandList
		, XMFLOAT4X4 *pxmf4x4World);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};

class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();
	virtual ~CDiffusedShader();

public:
	virtual D3D12_INPUT_LAYOUT_DESC	CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(
		CD3DDeviceIndRes *pd3dDeviceIndRes
		, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

struct CB_PLAYER_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
};
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	ID3D12Resource					*m_pd3dcbPlayer = NULL;
	CB_PLAYER_INFO					*m_pcbMappedPlayer = NULL;
};

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nMaterial;
};
class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();
	
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(
		CD3DDeviceIndRes *pd3dDeviceIndRes
		, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(
		CD3DDeviceIndRes *pd3dDeviceIndRes
		, ID3D12GraphicsCommandList *pd3dCommandList
		, CGameObject **ppObjects = NULL
		, int nObjects = 0);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	virtual CGameObject *PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition,
		XMFLOAT4X4& xmf4x4View, float *pfNearHitDistance);

protected:
	CGameObject **m_ppObjects = NULL;
	int m_nObjects = 0;

	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap = NULL;

	ID3D12Resource				*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO			*m_pcbMappedGameObjects = NULL;
};

class CInstancingShader : public CObjectsShader
{
public:
	CInstancingShader();
	virtual ~CInstancingShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(
		CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(
		CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void BuildObjects(CD3DDeviceIndRes *pd3dDeviceIndRes, 
		ID3D12GraphicsCommandList *pd3dCommandList, 
		CGameObject **ppObjects, int nObjects) override;
	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};

class CTerrainShader : public CObjectsShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	void SetTerrain(CGameObject* terrain);
};