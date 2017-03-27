#include "stdafx.h"

#include "Framework.h"

#include "LobbyScene.h"

using namespace std;

template <typename T>
T GetUserDataPtr(HWND hWnd) 
{
	return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}
void SetUserDataPtr(HWND hWnd, LPVOID ptr) 
{
	LONG_PTR result = ::SetWindowLongPtr(
		hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

CFramework::CFramework()
{
}
CFramework::~CFramework()
{
	OnDestroy();
}

bool CFramework::OnCreate(HWND hWnd, const RECT & rc)
{
	m_hWnd = hWnd;
	m_rcClient = rc;
	srand((unsigned int)time(NULL));

	// 클라이언트 좌표 초기화
	m_rcClient.right -= m_rcClient.left;
	m_rcClient.bottom -= m_rcClient.top;
	m_rcClient.left = 0;
	m_rcClient.top = 0;

	// Init DirectX
	InitD3D();

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	// 사운드 생성
	m_Sound.Initialize();

	// 현재 Scene은 없는걸로
	m_pCurrentScene = nullptr;
	// Scene 배열 초기화
	for (auto& p : m_arrScene) 
		p = nullptr;
	
	//// Scene 생성
	//BuildScene();
	ChangeScene(CScene::CurrentScene::Title);

	// 캡션 변경
	lstrcpy(m_CaptionTitle, TITLESTRING);

#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT(" ("));
#endif
	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);
	
	// 타이머 초기화
	m_LastUpdate_time = chrono::system_clock::now();
	m_current_time = chrono::system_clock::now();
	m_fps = 0;

	return (m_hWnd != NULL);
}

void CFramework::InitD3D()
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                   // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	scd.BufferDesc.Width = CLIENT_WIDTH;                   // set the back buffer width
	scd.BufferDesc.Height = CLIENT_HEIGHT;                 // set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
	scd.OutputWindow = m_hWnd;                             // the window to be used
	scd.SampleDesc.Count = 4;                              // how many multisamples
	scd.Windowed = TRUE;                                   // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

														   // create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_pSwapChain,
		&m_pD3D_Device,
		NULL,
		&m_pD3D_DC);


	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	m_pD3D_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	m_pD3D_DC->OMSetRenderTargets(1, &m_pBackbuffer, NULL);


	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = CLIENT_WIDTH;
	viewport.Height = CLIENT_HEIGHT;

	m_pD3D_DC->RSSetViewports(1, &viewport);

	InitGraphics();
}

void CFramework::InitGraphics()
{
	// create a triangle using the VERTEX struct
	VERTEX OurVertices[] =
	{
		{ 0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ -0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) }
	};


	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	m_pD3D_Device->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer


												   // copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	m_pD3D_DC->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	m_pD3D_DC->Unmap(pVBuffer, NULL);                                      // unmap the buffer
}

void CFramework::BuildScene()
{

	m_arrScene[GetSceneEnumInt(Lobby)] = new CLobbyScene();
	m_arrScene[GetSceneEnumInt(Lobby)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Lobby);


	m_pCurrentScene = m_arrScene[GetSceneEnumInt(Title)]; // 현재신은 타이틀로 초기화

}

void CFramework::ReleaseScene()
{
}

bool CFramework::OnDestroy()
{
	CleanD3D();
	return false;
}

void CFramework::CleanD3D()
{
	m_pSwapChain->SetFullscreenState(FALSE, NULL);  // switch to windowed mode

	// close and release all existing COM objects
	pVBuffer->Release();
	m_pSwapChain->Release();
	m_pBackbuffer->Release();
	m_pD3D_Device->Release();
	m_pD3D_DC->Release();
}

bool CFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrentScene)
	if (m_pCurrentScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam)) return false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		return true;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		return true;
	}
	return false;
}
bool CFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if(m_pCurrentScene)
	if (m_pCurrentScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam)) return false;
	switch (nMessageID)
	{
	case WM_LBUTTONUP:
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		break;
	}
	return false;
}
HRESULT CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	default:
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}

void CFramework::Update(float fTimeElapsed)
{
	if(m_pCurrentScene) m_pCurrentScene->Update(fTimeElapsed);
	m_Sound.Update(fTimeElapsed);
}

void CFramework::PreproccessingForRender()
{

	// clear the back buffer to a deep blue
	m_pD3D_DC->ClearRenderTargetView(m_pBackbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 0.5f));

	if(m_pCurrentScene) m_pCurrentScene->Render();

	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pD3D_DC->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	m_pD3D_DC->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	m_pD3D_DC->Draw(3, 0);

	// switch the back buffer and the front buffer
	m_pSwapChain->Present(0, 0);
}

void CFramework::ChangeScene(CScene::CurrentScene Tag, bool bDestroy)
{
	const int tag = GetEnumValueByType(Tag);
	//
	if (bDestroy) {
		int d_tag = GetEnumValueByType(m_pCurrentScene->GetMyTag());
		delete m_pCurrentScene;
		m_pCurrentScene = nullptr;
		m_arrScene[d_tag] = nullptr;
	}
	if (m_arrScene[tag]) { // 만약 nullptr가 아닐 경우
		m_pCurrentScene = m_arrScene[tag];

	}
	else {
		switch (Tag) {
		case CScene::CurrentScene::Lobby:
			m_arrScene[GetSceneEnumInt(Lobby)] = new CLobbyScene();
			m_arrScene[GetSceneEnumInt(Lobby)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Lobby);
			break;
		}
		m_pCurrentScene = m_arrScene[tag];
	}
}

void CFramework::FrameAdvance()
{
	// Get tick
	m_timeElapsed = chrono::system_clock::now() - m_current_time;
	if (m_timeElapsed.count() > MAX_FPS)
	{
		m_current_time = chrono::system_clock::now();
	
		if (m_timeElapsed.count() > 0.0)
			m_fps = 1.0 / m_timeElapsed.count();
	}
	// 최대 FPS 미만의 시간이 경과하면 진행 생략
	else return;

	Update(m_timeElapsed.count());
	PreproccessingForRender();

	InvalidateRect(m_hWnd, &m_rcClient, FALSE);

#if defined(SHOW_CAPTIONFPS)

	m_UpdateElapsed = chrono::system_clock::now() - m_LastUpdate_time;
	if (m_UpdateElapsed.count() > MAX_UPDATE_FPS)
		m_LastUpdate_time = chrono::system_clock::now();
	else return;

	_itow_s(m_fps + 0.1f, m_CaptionTitle + m_TitleLength, TITLE_MX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MX_LENGTH - m_TitleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_CaptionTitle);
#endif

}

LRESULT CFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CFramework* self = ::GetUserDataPtr<CFramework*>(hWnd);
	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:

	case WM_MOUSEMOVE:
		self->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		self->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_DESTROY:
		::SetUserDataPtr(hWnd, NULL);
		::PostQuitMessage(0);
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}
