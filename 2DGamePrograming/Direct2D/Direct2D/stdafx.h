#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <tchar.h>
#include <time.h>

// C++ 런타임 헤더 파일입니다.
#include <iostream>
#include <chrono>
#include <list>
#include <random>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
using namespace std;

// ComPtr
#include <wrl/client.h>
using namespace Microsoft::WRL;

// Direct2D
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
using namespace D2D1;

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

// 사용자 정의
#define CLIENT_WIDTH 1280
#define CLIENT_HEIGHT 720

#define TITLE_MX_LENGTH 64

#define TITLESTRING	TEXT("Direct2D Study")

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

#include "Vec2f.h"
#include "WarpList.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

