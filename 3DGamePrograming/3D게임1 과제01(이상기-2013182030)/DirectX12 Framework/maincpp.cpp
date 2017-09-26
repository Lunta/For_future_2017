//  유니코드 기반입니다.
#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>

#include "Framework.h"

//	LoadString : 타이틀 이름과 윈도우 클래스 이름 길이.
#define MAX_LOADSTRING 100

#define WINTITLE		TITLESTRING				//	클라이언트 타이틀 문자열
#define WINCLASSNAME	TEXT("winMain")			//	메인 윈도우 클래스명

// 전역 변수:
HINSTANCE hInst;							// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];				// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];		// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
//LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

CFramework myFramework;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	MSG msg;

	wsprintf(szTitle, WINTITLE);
	wsprintf(szWindowClass, WINCLASSNAME);

	// 전역 문자열을 초기화합니다.
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// 기본 메시지 루프입니다.
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)	break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			myFramework.FrameAdvance();
		}
	}
	myFramework.OnDestroy();
	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style =
		  CS_HREDRAW 	// 클라이언트의 너비를 변경하면, 전체 윈도우를 다시 그리게 한다.
		| CS_VREDRAW	// 클라이언트의 높이를 변경하면, 전체 윈도우를 다시 그리게 한다.
	//	| CS_DBLCLKS	// 해당 윈도우에서 더블클릭을 사용해야 한다면 추가해야 한다.
		;
	wcex.lpfnWndProc = CFramework::WndProc;
//	wcex.lpfnWndProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> HRESULT
//	{return myFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);};
//	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	//	해당 프로그램의 인스턴스 핸들.

//	MAKEINTERSOURCE : Make Inter Source. 프로그램 내부에 있는 리소스의 인덱스를 반환하는 매크로.
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//	메인 윈도우 핸들
	HWND hWnd;

	//	윈도우 스타일
	DWORD dwStyle =
		WS_OVERLAPPED 	// 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
		| WS_CAPTION 		// 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
		| WS_SYSMENU		// 시스템 메뉴를 가진 윈도우를 만든다.
		| WS_MINIMIZEBOX	// 최소화 버튼을 만든다.
		| WS_BORDER			// 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
//		| WS_THICKFRAME		// 크기 조정이 가능한 두꺼운 경계선을 가진다. WS_BORDER와 같이 사용할 수 없다.
		;					// 추가로 필요한 윈도우 스타일은 http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm 참고.

	//	인스턴스 핸들을 전역 변수에 저장
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	//	데스크톱 윈도우 사이즈
	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	//	클라이언트 사이즈
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	//	윈도우 사이즈에 실제로 추가되는(캡션, 외곽선 등) 크기를 보정.
	AdjustWindowRect(&rc, dwStyle, FALSE);

	//	클라이언트 절대좌표(left, top)
	//	데스크톱의 중앙에 클라이언트가 위치하도록 설정
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

	//	윈도우 생성
	hWnd = CreateWindow(
		szWindowClass			//	윈도우 클래스 명
		, szTitle				//	캡션 표시 문자열
		, dwStyle				//	윈도우 스타일
		, ptClientWorld.x		//	부모 윈도우 기반 윈도우 시작좌표 : x
		, ptClientWorld.y		//	부모 윈도우 기반 윈도우 시작좌표 : y
		, rc.right - rc.left	//	윈도우 사이즈 : width
		, rc.bottom - rc.top	//	윈도우 사이즈 : height
		, NULL					//	부모 윈도우.
		, NULL					//	메뉴 핸들
		, hInstance				//	인스턴스 핸들
		, NULL					//	추가 파라메터 : NULL
		);

	//	생성 실패시 프로그램 종료
	//	확인 : WndProc의 default msg handler가 DefWindowProc 함수를 반환하는가?
	if (!hWnd)	return FALSE;
	if (!myFramework.OnCreate(hWnd, rc)) return FALSE;
	//	윈도우 표시
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//	성공 반환
	return TRUE;
}
