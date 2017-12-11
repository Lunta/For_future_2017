//------------------------------------------------------- ----------------------
// File: Mesh.h
//-----------------------------------------------------------------------------

#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CVertex
{
public:
	XMFLOAT3						m_xmf3Position;
	XMFLOAT3						m_xmf3Normal;
	XMFLOAT3						m_xmf3Tangent;
	XMFLOAT2						m_xmf2TexCoord0;
	XMFLOAT2						m_xmf2TexCoord1;
	
public:
	CVertex() 
		: m_xmf3Position(XMFLOAT3(0.f, 0.f, 0.f))
		, m_xmf3Normal(XMFLOAT3(0.f, 0.f, 0.f))
		, m_xmf3Tangent(XMFLOAT3(0.f, 0.f, 0.f))
		, m_xmf2TexCoord0(XMFLOAT2(0.f, 0.f))
		, m_xmf2TexCoord1(XMFLOAT2(0.f, 0.f)) 
	{}
	CVertex(
		  const XMFLOAT3& xmf3Pos
		, const XMFLOAT3& xmf3Normal
		, const XMFLOAT3& xmf3Tangent
		, const XMFLOAT2& xmf2UV0
		, const XMFLOAT2& xmf2UV1)
		: m_xmf3Position(xmf3Pos)
		, m_xmf3Normal(xmf3Normal)
		, m_xmf3Tangent(xmf3Tangent)
		, m_xmf2TexCoord0(xmf2UV0)
		, m_xmf2TexCoord1(xmf2UV1) 
	{}
	~CVertex() {}
};

class CBillBoardVertex
{
public:
	XMFLOAT3						m_xmf4Pos;
	XMFLOAT2						m_xmf2Size;

public:
	CBillBoardVertex() 
		: m_xmf4Pos(XMFLOAT3(0.0f, 0.0f, 0.0f))
		, m_xmf2Size(XMFLOAT2(0.0f, 0.0f)) 
	{}
	CBillBoardVertex(
		  const XMFLOAT3& xmf3Pos
		, const XMFLOAT2& xmf2Size)
		: m_xmf4Pos(xmf3Pos)
		, m_xmf2Size(xmf2Size) 
	{}
	~CBillBoardVertex() {}
};

class CSkyBoxVertex
{
public:
	XMFLOAT3						m_xmf4Pos;
	XMFLOAT2						m_xmf2UV;

public:
	CSkyBoxVertex()
		: m_xmf4Pos(XMFLOAT3(0.0f, 0.0f, 0.0f))
		, m_xmf2UV(XMFLOAT2(0.0f, 0.0f))
	{}
	CSkyBoxVertex(
		const XMFLOAT3& xmf3Pos
		, const XMFLOAT2& xmf2Size)
		: m_xmf4Pos(xmf3Pos)
		, m_xmf2UV(xmf2Size)
	{}
	~CSkyBoxVertex() {}
};

class CMesh
{
public:
	CMesh(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	CMesh(
		  CD3DDeviceIndRes*					pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*		pd3dCommandList
		, UINT								nVertices
		, CVertex*							pVertices
		, UINT								nIndices
		, UINT*								pIndices);
	virtual ~CMesh();
	
private:
	int							m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();

protected:
	ComPtr<ID3D12Resource>					m_pd3dVertexBuffer;
	ComPtr<ID3D12Resource>					m_pd3dVertexUploadBuffer;

	ComPtr<ID3D12Resource>					m_pd3dIndexBuffer;
	ComPtr<ID3D12Resource>					m_pd3dIndexUploadBuffer;

	D3D12_VERTEX_BUFFER_VIEW				m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW					m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY				m_d3dPrimitiveTopology;
	UINT									m_nSlot;
	UINT									m_nVertices;
	UINT									m_nStride;
	UINT									m_nOffset;

	UINT									m_nIndices;
	UINT									m_nStartIndex;
	int										m_nBaseVertex;

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstances);
	
	void CalculateTriangleListVertexNormals(
		  XMFLOAT3*				pxmf3Normals
		, XMFLOAT3*				pxmf3Positions
		, UINT					nVertices);
	void CalculateTriangleListVertexNormals(
		  XMFLOAT3*				pxmf3Normals
		, XMFLOAT3*				pxmf3Positions
		, UINT					nVertices
		, UINT*					pIndices
		, UINT					nIndices);
	void CalculateTriangleStripVertexNormals(
		  XMFLOAT3*				pxmf3Normals
		, XMFLOAT3*				pxmf3Positions
		, UINT					nVertices
		, UINT*					pIndices
		, UINT					nIndices);
	void CalculateVertexNormals(
		  XMFLOAT3*				pxmf3Normals
		, XMFLOAT3*				pxmf3Positions
		, UINT					nVertices
		, UINT*					pIndices
		, UINT					nIndices);
};

class CRectMeshTextured : public CMesh
{
public:
	CRectMeshTextured(
		  CD3DDeviceIndRes*				pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*	pd3dCommandList
		, float							fWidth = 20.0f
		, float							fHeight = 20.0f
		, float							fDepth = 20.0f
		, float							fxPosition = 0.0f
		, float							fyPosition = 0.0f
		, float							fzPosition = 0.0f);
	virtual ~CRectMeshTextured();
};

class CHeightMapImage
{
private:
	BYTE*						m_pHeightMapPixels;
	int							m_nWidth;
	int							m_nLength;
	XMFLOAT3					m_xmf3Scale;

public:
	CHeightMapImage(
		  LPCTSTR				pFileName 
		, int					nWidth
		, int					nLength 
		, XMFLOAT3				xmf3Scale);
	~CHeightMapImage();

	float				GetHeight(float x, float z);
	XMFLOAT3			GetHeightMapNormal(int x, int z);
	XMFLOAT3			GetHeightMapTangent(int x, int z);
	XMFLOAT3			GetScale()							{ return(m_xmf3Scale); }
	BYTE*				GetHeightMapPixels()				{ return(m_pHeightMapPixels); }
	int					GetHeightMapWidth()					{ return(m_nWidth); }
	int					GetHeightMapLength()				{ return(m_nLength); }
};

class CHeightMapGridMesh : public CMesh
{
protected:
	int							m_nWidth;
	int							m_nLength;
	XMFLOAT3					m_xmf3Scale;

public:
	CHeightMapGridMesh(
		  CD3DDeviceIndRes*				pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*	pd3dCommandList
		, int xStart, int zStart, int nWidth, int nLength
		, XMFLOAT3						xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f)
		, XMFLOAT4						xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f)
		, void*							pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3			GetScale()					{ return(m_xmf3Scale); }
	int					GetWidth()					{ return(m_nWidth); }
	int					GetLength()					{ return(m_nLength); }

	virtual float		OnGetHeight(int x, int z, void *pContext);
};