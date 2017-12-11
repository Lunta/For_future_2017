#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(
	  CD3DDeviceIndRes* pd3dDeviceIndRes
	, ID3D12GraphicsCommandList * pd3dCommandList)
	: m_pd3dVertexBuffer(nullptr)
	, m_pd3dVertexUploadBuffer(nullptr)
	, m_pd3dIndexBuffer(nullptr)
	, m_pd3dIndexUploadBuffer(nullptr)
	, m_d3dPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_nSlot(0)
	, m_nVertices(0)
	, m_nStride(0)
	, m_nOffset(0)
	, m_nIndices(0)
	, m_nStartIndex(0)
	, m_nBaseVertex(0)
{
	memset(&m_d3dVertexBufferView, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	memset(&m_d3dIndexBufferView, 0, sizeof(D3D12_INDEX_BUFFER_VIEW));
}
CMesh::CMesh(
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, UINT							nVertices
	, CVertex*						pVertices
	, UINT							nIndices
	, UINT*							pIndices)
{
	m_nStride = sizeof(CVertex);
	m_nVertices = nVertices;

	CVertex* arrVertices = new CVertex[m_nVertices];
	memcpy(arrVertices, pVertices, sizeof(CVertex) * m_nVertices);

	m_pd3dVertexBuffer = pd3dDeviceIndRes->CreateBufferResource(
		  pd3dCommandList
		, arrVertices
		, m_nStride * m_nVertices
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	if (nIndices > 0)
	{
		m_nIndices = nIndices;
		m_pd3dIndexBuffer = pd3dDeviceIndRes->CreateBufferResource(
			  pd3dCommandList
			, pIndices
			, sizeof(UINT) * m_nIndices
			, D3D12_HEAP_TYPE_DEFAULT
			, D3D12_RESOURCE_STATE_INDEX_BUFFER
			, &m_pd3dIndexUploadBuffer);

		m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
		m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	}
}
CMesh::~CMesh()
{
}

void CMesh::ReleaseUploadBuffers()
{
	m_pd3dVertexUploadBuffer.Reset();
	m_pd3dIndexUploadBuffer.Reset();
};

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	Render(pd3dCommandList, 1);
}
void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

void CMesh::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMesh::CalculateTriangleListVertexNormals(XMFLOAT3 * pxmf3Normals, XMFLOAT3 * pxmf3Positions, UINT nVertices, UINT * pIndices, UINT nIndices)
{
	UINT nPrimitives = (pIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pIndices[i * 3 + 0];
			nIndex1 = pIndices[i * 3 + 1];
			nIndex2 = pIndices[i * 3 + 2];
			if (pIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMesh::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pIndices, UINT nIndices)
{
	UINT nPrimitives = (pIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pIndices) nIndex0 = pIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pIndices) nIndex1 = pIndices[nIndex1];
			nIndex2 = (pIndices) ? pIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMesh::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pIndices, UINT nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pIndices, nIndices);
		break;
	default:
		break;
	}
}

CRectMeshTextured::CRectMeshTextured(
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, float							fWidth
	, float							fHeight
	, float							fDepth
	, float							fxPosition
	, float							fyPosition
	, float							fzPosition)
	: CMesh(pd3dDeviceIndRes, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CSkyBoxVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CSkyBoxVertex pVertices[6];

	float fx = (fWidth * 0.5f) + fxPosition
		, fy = (fHeight * 0.5f) + fyPosition,
		fz = (fDepth * 0.5f) + fzPosition;

	if (fWidth == 0.0f)
	{
		if (fxPosition > 0.0f)
		{
			pVertices[0] = CSkyBoxVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CSkyBoxVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CSkyBoxVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CSkyBoxVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CSkyBoxVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CSkyBoxVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CSkyBoxVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CSkyBoxVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CSkyBoxVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CSkyBoxVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CSkyBoxVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CSkyBoxVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fHeight == 0.0f)
	{
		if (fyPosition > 0.0f)
		{
			pVertices[0] = CSkyBoxVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CSkyBoxVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CSkyBoxVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CSkyBoxVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CSkyBoxVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CSkyBoxVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CSkyBoxVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CSkyBoxVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CSkyBoxVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CSkyBoxVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CSkyBoxVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CSkyBoxVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fDepth == 0.0f)
	{
		if (fzPosition > 0.0f)
		{
			pVertices[0] = CSkyBoxVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CSkyBoxVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CSkyBoxVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CSkyBoxVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CSkyBoxVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CSkyBoxVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CSkyBoxVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CSkyBoxVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CSkyBoxVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CSkyBoxVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CSkyBoxVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CSkyBoxVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
	}

	m_pd3dVertexBuffer = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, pVertices
		, m_nStride * m_nVertices
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CRectMeshTextured::~CRectMeshTextured()
{
}

CHeightMapImage::CHeightMapImage(
	  LPCTSTR						pFileName
	, int							nWidth
	, int							nLength
	, XMFLOAT3						xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	BYTE *pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	//파일을 열고 읽는다. 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다.
	HANDLE hFile = ::CreateFile(
		  pFileName
		, GENERIC_READ
		, 0
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY
		, NULL);

	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	/*이미지의 y-축과 지형의 z-축이 방향이 반대이므로 이미지를 상하대칭 시켜 저장한다. 그러면 다음 그림과 같이 이
	미지의 좌표축과 지형의 좌표축의 방향이 일치하게 된다.*/
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = 
				pHeightMapPixels[x + (y*m_nWidth)];
		}
	}
	if (pHeightMapPixels) delete[] pHeightMapPixels;
}
CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	// x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터이다.
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// 높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한 
	// 픽셀 값을 사용하여 법선 벡터를 계산한다.
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	//(x, z), (x+1, z), (z, z+1)의 픽셀에서 지형의 높이를 구한다.
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;

	//xmf3Edge1은 (0, y3, m_xmf3Scale.z) - (0, y1, 0) 벡터이다.
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);

	//xmf3Edge2는 (m_xmf3Scale.x, y2, 0) - (0, y1, 0) 벡터이다.
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);

	//법선 벡터는 xmf3Edge1과 xmf3Edge2의 외적을 정규화하면 된다.
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);
	return(xmf3Normal);
}

XMFLOAT3 CHeightMapImage::GetHeightMapTangent(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;

	return XMFLOAT3(1.f, (y2 - y1) / m_xmf3Scale.x, 0.0f);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMapImage::GetHeight(float fx, float fz)
{
	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다. 높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 높이는
	0이다.*/
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;
	float fBottomLeft = (float)m_pHeightMapPixels[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1)*m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-좌표가 1, 3, 5, ...인 경우 인덱스가 오른쪽에서 왼쪽으로 나열된다.
	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
		/*지형의 삼각형들이 오른쪽에서 왼쪽 방향으로 나열되는 경우이다. 다음 그림의 오른쪽은 (fzPercent < fxPercent)
		인 경우이다. 이 경우 TopLeft의 픽셀 값은 (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))로 근사한다.
		다음 그림의 왼쪽은 (fzPercent ≥ fxPercent)인 경우이다. 이 경우 BottomRight의 픽셀 값은 (fBottomRight =
		fBottomLeft + (fTopRight - fTopLeft))로 근사한다.*/
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		/*지형의 삼각형들이 왼쪽에서 오른쪽 방향으로 나열되는 경우이다. 다음 그림의 왼쪽은 (fzPercent < (1.0f -
		fxPercent))인 경우이다. 이 경우 TopRight의 픽셀 값은 (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))로
		근사한다. 다음 그림의 오른쪽은 (fzPercent ≥ (1.0f - fxPercent))인 경우이다. 이 경우 BottomLeft의 픽셀 값은
		(fBottomLeft = fTopLeft + (fBottomRight - fTopRight))로 근사한다.*/
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);
}


CHeightMapGridMesh::CHeightMapGridMesh(
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, int xStart, int zStart, int nWidth, int nLength
	, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void *pContext) 
	: CMesh(pd3dDeviceIndRes, pd3dCommandList)
{
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(CVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	CVertex *pVertices = new CVertex[m_nVertices];

	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	int cxHeightMap = pHeightMapImage->GetHeightMapWidth();
	int czHeightMap = pHeightMapImage->GetHeightMapLength();

	float fHeight = 0.0f, fMinHeight = FLT_MAX, fMaxHeight = FLT_MIN;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			float fScaledX = static_cast<float>(x) * m_xmf3Scale.x;
			float fScaledZ = static_cast<float>(z) * m_xmf3Scale.z;

			fHeight = OnGetHeight(x, z, pContext);
			pVertices[i].m_xmf3Position		= XMFLOAT3(fScaledX, fHeight, fScaledZ);
			pVertices[i].m_xmf3Normal		= pHeightMapImage->GetHeightMapNormal(fScaledX, fScaledZ);
			pVertices[i].m_xmf3Tangent		= pHeightMapImage->GetHeightMapTangent(fScaledX, fScaledZ);

			pVertices[i].m_xmf2TexCoord0	= XMFLOAT2(
				  static_cast<float>(x) / static_cast<float>(cxHeightMap - 1)
				, static_cast<float>(czHeightMap - 1 - z) / static_cast<float>(czHeightMap - 1));

			pVertices[i].m_xmf2TexCoord1	= XMFLOAT2(
				  static_cast<float>(x * 2) / m_xmf3Scale.x
				, static_cast<float>(z * 2) / m_xmf3Scale.z);

			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer = pd3dDeviceIndRes->CreateBufferResource(
		  pd3dCommandList
		, pVertices
		, m_nStride * m_nVertices
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices;

	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	UINT *pIndices = new UINT[m_nIndices];

	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0)) pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1)) pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, pIndices
		, sizeof(UINT) * m_nIndices
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pIndices;
}
CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	BYTE *pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z*nWidth)] * xmf3Scale.y;
	return(fHeight);
}