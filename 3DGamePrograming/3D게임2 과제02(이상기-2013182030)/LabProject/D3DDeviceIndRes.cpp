#include "stdafx.h"
#include "D3DDeviceIndRes.h"

#include "DDSTextureLoader12.h"

CD3DDeviceIndRes::CD3DDeviceIndRes() :
	// MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	m_bMsaa4xEnable(false),
	m_nMsaa4xQualityLevels(0)
{
}
CD3DDeviceIndRes::~CD3DDeviceIndRes()
{
}

bool CD3DDeviceIndRes::Initialize()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(IID_PPV_ARGS(&m_pd3dDebugController));
	m_pd3dDebugController->EnableDebugLayer();
#endif
	//DXGI 팩토리를 생성한다.
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_pdxgiFactory))))
	{
		MessageBox(
			  nullptr
			, TEXT("DXGIFactory 생성에 실패했습니다.")
			, TEXT("Error!"), MB_OK);
		return false;
	}
	if(!CreateDirect3DDevice())
	{
		MessageBox(
			nullptr
			, TEXT("D3DDevice 생성에 실패했습니다.")
			, TEXT("Error!"), MB_OK);
		return false;
	}
	if (!CheckFeatureSupport())
	{
		MessageBox(
			nullptr
			, TEXT("FeatureSupport 확인에 실패했습니다.")
			, TEXT("Error!"), MB_OK);
		return false;
	}
	return true;
}

bool CD3DDeviceIndRes::CreateDirect3DDevice()
{
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	ComPtr<IDXGIAdapter1> pd3dAdapter = NULL;
	for (UINT i = 0;
		m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter) !=
		DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);

		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		return SUCCEEDED(D3D12CreateDevice(
			  pd3dAdapter.Get()
			, D3D_FEATURE_LEVEL_12_1
			, IID_PPV_ARGS(&m_pd3dDevice)));
	}

	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	if (!pd3dAdapter.Get())
	{
		m_pdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pd3dAdapter));
		return SUCCEEDED(D3D12CreateDevice(
			  pd3dAdapter.Get()
			, D3D_FEATURE_LEVEL_11_0
			, IID_PPV_ARGS(&m_pd3dDevice)));
	}
	return false;
}

bool CD3DDeviceIndRes::CreateSwapChain(
	HWND					hWnd
	, UINT					BufferCount
	, UINT					BufferWidth
	, UINT					BufferHeight
	, ID3D12CommandQueue*	pd3dCommandQueue
	, IDXGISwapChain**		ppdxgiSwapChain)
{
	//스왑체인을 생성한다.
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.OutputWindow = hWnd;
	dxgiSwapChainDesc.BufferCount = BufferCount;
	dxgiSwapChainDesc.BufferDesc.Width = BufferWidth;
	dxgiSwapChainDesc.BufferDesc.Height = BufferHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality =
		(m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	//전체화면 모드에서 바탕화면의 해상도를 바꾸지 않고 후면버퍼의 크기를 바탕화면 크기로 변경한다.
	dxgiSwapChainDesc.Flags = 0;
#else
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif
	return SUCCEEDED(
		m_pdxgiFactory->CreateSwapChain(
			pd3dCommandQueue
			, &dxgiSwapChainDesc
			, ppdxgiSwapChain));
}

bool CD3DDeviceIndRes::CreateCommandQueue(ID3D12CommandQueue ** ppd3dCommandQueue)
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	return SUCCEEDED(
		m_pd3dDevice->CreateCommandQueue(
			  &d3dCommandQueueDesc
			, IID_PPV_ARGS(ppd3dCommandQueue)));
}
bool CD3DDeviceIndRes::CreateCommandQueue(
	  D3D12_COMMAND_QUEUE_DESC* pd3dCommandQueueDesc
	, ID3D12CommandQueue**		ppd3dCommandQueue)
{
	return SUCCEEDED(
		m_pd3dDevice->CreateCommandQueue(
			  pd3dCommandQueueDesc
			, IID_PPV_ARGS(ppd3dCommandQueue)));
}
bool CD3DDeviceIndRes::CreateCommandAllocator(
 	  D3D12_COMMAND_LIST_TYPE	d3dCommandListType
	, ID3D12CommandAllocator**	ppd3dCommandAllocator)
{
	return SUCCEEDED(
		m_pd3dDevice->CreateCommandAllocator(
			  d3dCommandListType
			, IID_PPV_ARGS(ppd3dCommandAllocator)));
}
bool CD3DDeviceIndRes::CreateCommandList(
	  UINT nodeMask
	, D3D12_COMMAND_LIST_TYPE		d3dCommandListType
	, ID3D12CommandAllocator*		pd3dCommandAllocator
	, ID3D12PipelineState*			pd3dPipelineState
	, ID3D12GraphicsCommandList**	pd3dCommandList)
{
	return SUCCEEDED(
		m_pd3dDevice->CreateCommandList(
			  nodeMask
			, d3dCommandListType
			, pd3dCommandAllocator
			, pd3dPipelineState
			, IID_PPV_ARGS(pd3dCommandList)));
}

bool CD3DDeviceIndRes::CreateFence(
	  UINT64			InitialValue
	, D3D12_FENCE_FLAGS Flags
	, ID3D12Fence**		ppFence)
{
	return SUCCEEDED(
		m_pd3dDevice->CreateFence(
			  InitialValue
			, Flags
			, IID_PPV_ARGS(ppFence)));
}
bool CD3DDeviceIndRes::CreateCommittedResource(
	D3D12_HEAP_PROPERTIES*	pHeapProperties
	, D3D12_HEAP_FLAGS		HeapFlags
	, D3D12_RESOURCE_DESC*	pDesc
	, D3D12_RESOURCE_STATES InitialResourceState
	, D3D12_CLEAR_VALUE*	pOptimizedClearValue
	, ID3D12Resource**		ppvResource)
{
	return SUCCEEDED(
		m_pd3dDevice->CreateCommittedResource(
			  pHeapProperties
			, HeapFlags
			, pDesc
			, InitialResourceState
			, pOptimizedClearValue
			, IID_PPV_ARGS(ppvResource)));
}
ID3D12Resource* CD3DDeviceIndRes::CreateBufferResource(
	  ID3D12GraphicsCommandList*	pd3dCommandList
	, void*							pData
	, UINT							nBytes
	, D3D12_HEAP_TYPE				d3dHeapType
	, D3D12_RESOURCE_STATES			d3dResourceStates
	, ID3D12Resource**				ppd3dUploadBuffer)
{
	ID3D12Resource *pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD)
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;

	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK)
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	CreateCommittedResource(
		  &d3dHeapPropertiesDesc
		, D3D12_HEAP_FLAG_NONE
		, &d3dResourceDesc
		, d3dResourceInitialStates
		, NULL
		, &pd3dBuffer);

	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				//업로드 버퍼를 생성한다.
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				CreateCommittedResource(
					  &d3dHeapPropertiesDesc
					, D3D12_HEAP_FLAG_NONE
					, &d3dResourceDesc
					, D3D12_RESOURCE_STATE_GENERIC_READ
					, NULL
					, ppd3dUploadBuffer);

				//업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다.
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);

				//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8 *pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}
ID3D12Resource * CD3DDeviceIndRes::CreateTextureResourceFromFile(
	ID3D12GraphicsCommandList*	pd3dCommandList
	, wchar_t*					pszFileName
	, ID3D12Resource**			ppd3dUploadBuffer
	, D3D12_RESOURCE_STATES		d3dResourceStates)
{
	ID3D12Resource *pd3dTexture = NULL;
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
	bool bIsCubeMap = false;

	if (FAILED(DirectX::LoadDDSTextureFromFileEx(
		m_pd3dDevice.Get()
		, pszFileName
		, 0
		, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT
		, &pd3dTexture
		, ddsData
		, vSubresources
		, &ddsAlphaMode
		, &bIsCubeMap)))
	{
		MessageBox(
			nullptr
			, TEXT("LoadDDSTextureFromFileEx() failed.")
			, TEXT("Error!"), MB_OK);
		return false;
	}
	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	//	D3D12_RESOURCE_DESC d3dResourceDesc = pd3dTexture->GetDesc();
	//	UINT nSubResources = d3dResourceDesc.DepthOrArraySize * d3dResourceDesc.MipLevels;
	UINT nSubResources = (UINT)vSubresources.size();
	//	UINT64 nBytes = 0;
	//	pd3dDevice->GetCopyableFootprints(&d3dResourceDesc, 0, nSubResources, 0, NULL, NULL, NULL, &nBytes);
	UINT64 nBytes = GetRequiredIntermediateSize(pd3dTexture, 0, nSubResources);

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //Upload Heap에는 텍스쳐를 생성할 수 없음
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	CreateCommittedResource(
		  &d3dHeapPropertiesDesc
		, D3D12_HEAP_FLAG_NONE
		, &d3dResourceDesc
		, D3D12_RESOURCE_STATE_GENERIC_READ
		, NULL
		, ppd3dUploadBuffer);

	//UINT nSubResources = (UINT)vSubresources.size();
	//D3D12_SUBRESOURCE_DATA *pd3dSubResourceData = new D3D12_SUBRESOURCE_DATA[nSubResources];
	//for (UINT i = 0; i < nSubResources; i++) pd3dSubResourceData[i] = vSubresources.at(i);

	//	std::vector<D3D12_SUBRESOURCE_DATA>::pointer ptr = &vSubresources[0];
	::UpdateSubresources(pd3dCommandList, pd3dTexture, *ppd3dUploadBuffer, 0, 0, nSubResources, &vSubresources[0]);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pd3dTexture;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	//	delete[] pd3dSubResourceData;

	return(pd3dTexture);
}

bool CD3DDeviceIndRes::CreateDepthStencilBuffer(
	  UINT64			BufferWidth
	, UINT64			BufferHeight
	, ID3D12Resource**	ppvResource)
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = BufferWidth;
	d3dResourceDesc.Height = BufferHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = 
		(m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	//깊이-스텐실 버퍼를 생성한다.
	return SUCCEEDED(m_pd3dDevice->CreateCommittedResource(
		  &d3dHeapProperties
		, D3D12_HEAP_FLAG_NONE
		, &d3dResourceDesc
		, D3D12_RESOURCE_STATE_DEPTH_WRITE
		, &d3dClearValue
		, IID_PPV_ARGS(ppvResource)));
}
void CD3DDeviceIndRes::CreateRenderTargetView(
	  ID3D12Resource				*pResource
	, D3D12_RENDER_TARGET_VIEW_DESC	*pDesc
	, D3D12_CPU_DESCRIPTOR_HANDLE	DestDescriptor)
{
	m_pd3dDevice->CreateRenderTargetView(
		  pResource
		, pDesc
		, DestDescriptor);
}
void CD3DDeviceIndRes::CreateDepthStencilView(
	  ID3D12Resource*					pResource
	, D3D12_DEPTH_STENCIL_VIEW_DESC*	pDesc
	, D3D12_CPU_DESCRIPTOR_HANDLE		DestDescriptor)
{
	m_pd3dDevice->CreateDepthStencilView(
		  pResource
		, pDesc
		, DestDescriptor);
}
void CD3DDeviceIndRes::CreateConstantBufferView(
	  D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc
	, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
{
	m_pd3dDevice->CreateConstantBufferView(pDesc, DestDescriptor);
}
bool CD3DDeviceIndRes::CreateDescriptorHeap(
	  D3D12_DESCRIPTOR_HEAP_TYPE	Type
	, UINT							NumDescriptors
	, D3D12_DESCRIPTOR_HEAP_FLAGS	Flags
	, UINT							NodeMask
	, ID3D12DescriptorHeap**		ppd3dDescriptorHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.Type = Type;
	d3dDescriptorHeapDesc.NumDescriptors = NumDescriptors;
	d3dDescriptorHeapDesc.Flags = Flags;
	d3dDescriptorHeapDesc.NodeMask = NodeMask;
	return SUCCEEDED(
		m_pd3dDevice->CreateDescriptorHeap(
			  &d3dDescriptorHeapDesc
			, IID_PPV_ARGS(ppd3dDescriptorHeap)));
}

UINT CD3DDeviceIndRes::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType)
{
	return m_pd3dDevice->GetDescriptorHandleIncrementSize(DescriptorHeapType);
}

bool CD3DDeviceIndRes::CreateRootSignature(
	  UINT					nodeMask
	, void*					pBlobWithRootSignature
	, SIZE_T				blobLengthInBytes
	, ID3D12RootSignature** ppd3dRootSignature)
{
	return SUCCEEDED(
		m_pd3dDevice->CreateRootSignature(
			  nodeMask
			, pBlobWithRootSignature
			, blobLengthInBytes
			, IID_PPV_ARGS(ppd3dRootSignature)));
}
bool CD3DDeviceIndRes::CreateGraphicsPipelineState(
	  D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc
	, ID3D12PipelineState** ppPipelineState)
{
	return SUCCEEDED(
		m_pd3dDevice->CreateGraphicsPipelineState(
			pDesc, IID_PPV_ARGS(ppPipelineState)));
}

bool CD3DDeviceIndRes::CheckFeatureSupport()
{
	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	bool bSucceeded = SUCCEEDED(
		m_pd3dDevice->CheckFeatureSupport(
		  D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS
		, &d3dMsaaQualityLevels
		, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));

	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	return bSucceeded;
}

bool CD3DDeviceIndRes::CheckFeatureSupport(
	  D3D12_FEATURE feature
	, void*			pFeatureSupportData
	, UINT			FeatureSupportDataSize)
{
	return SUCCEEDED(
		m_pd3dDevice->CheckFeatureSupport(
			  feature
			, pFeatureSupportData
			, FeatureSupportDataSize));
}

bool CD3DDeviceIndRes::MakeWindowAssociation(HWND hWnd, UINT flags)
{
	return SUCCEEDED(m_pdxgiFactory->MakeWindowAssociation(hWnd, flags));
}