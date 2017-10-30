#include "stdafx.h"
#include "Framework.h"
#include "MainScene.h"

using namespace std;

template <typename T>
constexpr T GetUserDataPtr(HWND hWnd) noexcept
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
	srand((unsigned int)time(NULL));

	m_hWnd = hWnd;
	m_rcClient = rc;

	// 클라이언트 좌표 초기화
	m_rcClient.right -= m_rcClient.left;
	m_rcClient.bottom -= m_rcClient.top;
	m_rcClient.left = 0;
	m_rcClient.top = 0;

	m_IndRes.Initialize();
	m_IndRes.CreateHwndRenderTarget(
		m_hWnd,
		m_rcClient.right,
		m_rcClient.bottom,
		&m_pd2dRenderTarget);

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	// 현재 Scene은 없는걸로
	m_pCurrentScene = nullptr;
	// Scene 배열 초기화
	for (auto& p : m_arrScene) 
		p = nullptr;

	// Object 생성
	BuildObjects();
	// Scene 생성
	BuildScene();

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

bool CFramework::OnDestroy()
{
	return false;
}

void CFramework::BuildObjects()
{
	auto dwFactory = m_IndRes.GetdwFactory();
	dwFactory->CreateTextFormat(
		L"Arial",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		30,
		L"ko-kr",
		&m_pdwTextFormat
	);
	m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	dwFactory->CreateTextLayout(
		L"Hello, World!", 
		lstrlenW(L"Hello, World!"), 
		m_pdwTextFormat.Get(), 
		300,
		100,
		&m_pdwTextLayout
	);

	m_IndRes.LoadImageFromFile(
		  m_pd2dRenderTarget.Get()
		, L"Effect.png"
		, &m_pd2dbmpTest);
}

void CFramework::BuildScene()
{
	m_arrScene[GetSceneEnumInt(Main)] = new CMainScene();
	m_arrScene[GetSceneEnumInt(Main)]->BuildObjects(this, m_hWnd, CScene::CurrentScene::Main);

	m_pCurrentScene = m_arrScene[GetSceneEnumInt(Main)]; // 현재신은 타이틀로 초기화

}

bool CFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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
	m_pCurrentScene->Update(fTimeElapsed);
	if ((currImg += fTimeElapsed*20) >= 10.f)
		currImg -= 10.f;
}

void CFramework::Render()
{
	if (m_pd2dRenderTarget.Get())
	{
		m_pd2dRenderTarget->BeginDraw();
		m_pd2dRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pd2dRenderTarget->Clear(ColorF{ ColorF::LightGray });
		ComPtr<ID2D1SolidColorBrush> brush;
		m_pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::PaleGreen }, &brush);
		m_pd2dRenderTarget->DrawRectangle(RectF(100, 100, 200, 200), brush.Get());

		m_pd2dRenderTarget->DrawTextLayout(Point2F(0, 0), m_pdwTextLayout.Get(), brush.Get());
		m_pd2dRenderTarget->DrawText(L"Text", 4, m_pdwTextFormat.Get(), RectF(100, 100, 200, 200), brush.Get());

		m_pCurrentScene->Render(m_pd2dRenderTarget.Get());

		auto bmpSize = m_pd2dbmpTest->GetSize();
		m_pd2dRenderTarget->DrawBitmap(
			m_pd2dbmpTest.Get()
			, RectF(100, 100, 100 + bmpSize.width * 0.1f, 100 + bmpSize.height)
			, min(1.f, currImg * 0.5f)
			, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
			, RectF(
				  bmpSize.width *0.1f * (int)currImg
				, 0
				, bmpSize.width *0.1f * (int)(currImg+1)
				, bmpSize.height)
		);

		m_pd2dRenderTarget->EndDraw();
	}
}

void CFramework::ChangeScene(CScene::CurrentScene Tag, bool bDestroy)
{
	const int tag = GetEnumValueByType(Tag);
	if (bDestroy)
	{
		int d_tag = GetEnumValueByType(m_pCurrentScene->GetMyTag());
		delete m_pCurrentScene;
		m_pCurrentScene = nullptr;
		m_arrScene[d_tag] = nullptr;
	}
	// 만약 nullptr가 아닐 경우
	if (m_arrScene[tag])
	{ 
		m_pCurrentScene = m_arrScene[tag];
	}
	else
	{
		switch (Tag) 
		{
		case CScene::CurrentScene::Main:
			m_arrScene[GetSceneEnumInt(Main)] = new CMainScene();
			m_arrScene[GetSceneEnumInt(Main)]->BuildObjects(this, m_hWnd, CScene::CurrentScene::Main);
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

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		self->Render();
		EndPaint(hWnd, &ps);
	}
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
