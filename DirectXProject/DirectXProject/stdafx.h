// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN      // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows Header file:
#include <windows.h>
#include <windowsx.h>

// Direct3D Header file:
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx11d.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "d3dx10d.lib")

// Set Console
#pragma comment (linker, "/entry:wWinMainCRTStartup /subsystem:console")

// C runtime Header file
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ runtime Header file
#include <iostream>
#include <chrono>
#include <string>


#define CLIENT_WIDTH 1200		// 클라이언트 너비
#define CLIENT_HEIGHT 720		// 클라이언트 높이 

#define MAX_LOADSTRING 100		// 타이틀 이름과 윈도우 클래스 이름 길이.
#define TITLE_MX_LENGTH 64

#define WINCLASSNAME	TEXT("WinMain")			//	메인 윈도우 클래스명
#define WINTITLE		TEXT("DirectX Project")	//	클라이언트 타이틀 문자열
#define TITLESTRING		WINTITLE

// 캡션 FPS 출력 여부 -------------------
// 항상 캡션에 FPS를 출력		(0 : 비활성 | 1 : 활성)
#define USE_CAPTIONFPS_ALWAYS	 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug에서는 항상 실행
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// 최대 FPS
#if _DEBUG
#define MAX_FPS 0.0
#else
#define MAX_FPS 1.0 / 60.0
#endif

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

#define PI 3.141592653589793

// a struct to define a single vertex
struct VERTEX { FLOAT X, Y, Z; D3DXCOLOR Color; };