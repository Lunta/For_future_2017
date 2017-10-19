#pragma once
class CD3DDeviceIndRes
{
private:
#if defined(_DEBUG)
	// 디버그 컨트롤러
	ComPtr<ID3D12Debug>					m_pd3dDebugController;
#endif

	// DXGI 팩토리 인터페이스에 대한 포인터이다.
	ComPtr<IDXGIFactory4>				m_pdxgiFactory;
	// Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ComPtr<ID3D12Device>				m_pd3dDevice;

	// MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	bool								m_bMsaa4xEnable;
	UINT								m_nMsaa4xQualityLevels;
public:
	CD3DDeviceIndRes();
	~CD3DDeviceIndRes();

	bool Initialize();
	bool CreateDirect3DDevice();

	bool CreateCommandQueue(ID3D12CommandQueue** ppd3dCommandQueue);
	bool CreateCommandQueue(
		  D3D12_COMMAND_QUEUE_DESC*		pd3dCommandQueueDesc
		, ID3D12CommandQueue**			ppd3dCommandQueue);
	bool CreateCommandAllocator(
		  D3D12_COMMAND_LIST_TYPE		d3dCommandListType
		, ID3D12CommandAllocator**		ppd3dCommandAllocator);
	bool CreateCommandList(
		  UINT nodeMask
		, D3D12_COMMAND_LIST_TYPE		d3dCommandListType
		, ID3D12CommandAllocator*		pd3dCommandAllocator
		, ID3D12PipelineState*			pd3dPipelineState
		, ID3D12GraphicsCommandList**	pd3dCommandList);

	bool CreateSwapChain(
		  HWND							hWnd
		, UINT							BufferCount
		, UINT							BufferWidth
		, UINT							BufferHeight
		, ID3D12CommandQueue*			pd3dCommandQueue
		, IDXGISwapChain**				ppdxgiSwapChain);
	bool CreateFence(
		  UINT64						InitialValue
		, D3D12_FENCE_FLAGS				Flags
		, ID3D12Fence**					ppFence);

	bool CreateCommittedResource(
		  D3D12_HEAP_PROPERTIES*		pHeapProperties
		, D3D12_HEAP_FLAGS				HeapFlags
		, D3D12_RESOURCE_DESC*			pDesc
		, D3D12_RESOURCE_STATES			InitialResourceState
		, D3D12_CLEAR_VALUE*			pOptimizedClearValue
		, ID3D12Resource**				ppvResource);

	ID3D12Resource* CreateBufferResource(
		  ID3D12GraphicsCommandList*	pd3dCommandList
		, void*							pData
		, UINT							nBytes
		, D3D12_HEAP_TYPE				d3dHeapType = D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATES			d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, ID3D12Resource**				ppd3dUploadBuffer = NULL);
	ID3D12Resource *CreateTextureResourceFromFile(
		  ID3D12GraphicsCommandList*	pd3dCommandList
		, wchar_t*						pszFileName
		, ID3D12Resource**				ppd3dUploadBuffer
		, D3D12_RESOURCE_STATES			d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	bool CreateDepthStencilBuffer(
		  UINT64						BufferWidth
		, UINT64						BufferHeight
		, ID3D12Resource**				ppvResource);
	void CreateRenderTargetView(
		  ID3D12Resource*				pResource
		, D3D12_RENDER_TARGET_VIEW_DESC*pDesc
		, D3D12_CPU_DESCRIPTOR_HANDLE	DestDescriptor);
	void CreateDepthStencilView(
		  ID3D12Resource*				pResource
		, D3D12_DEPTH_STENCIL_VIEW_DESC*pDesc
		, D3D12_CPU_DESCRIPTOR_HANDLE	DestDescriptor);

	bool CreateDescriptorHeap(
		  D3D12_DESCRIPTOR_HEAP_TYPE	Type
		, UINT							NumDescriptors
		, D3D12_DESCRIPTOR_HEAP_FLAGS	Flags
		, UINT							NodeMask
		, ID3D12DescriptorHeap**		ppd3dDescriptorHeap);
	UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType);

	bool CreateRootSignature(
		  UINT							nodeMask
		, void*							pBlobWithRootSignature
		, SIZE_T						blobLengthInBytes
		, ID3D12RootSignature**			ppd3dRootSignature);
	bool CreateGraphicsPipelineState(
		  D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc
		, ID3D12PipelineState**			ppPipelineState);

	bool CheckFeatureSupport();
	bool CheckFeatureSupport(
		  D3D12_FEATURE					feature
		, void*							pFeatureSupportData
		, UINT							FeatureSupportDataSize);
	bool MakeWindowAssociation(HWND hWnd, UINT flags);
};