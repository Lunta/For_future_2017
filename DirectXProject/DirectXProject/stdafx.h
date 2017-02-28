// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN      // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
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


#define CLIENT_WIDTH 1200		// Ŭ���̾�Ʈ �ʺ�
#define CLIENT_HEIGHT 720		// Ŭ���̾�Ʈ ���� 

#define MAX_LOADSTRING 100		// Ÿ��Ʋ �̸��� ������ Ŭ���� �̸� ����.
#define TITLE_MX_LENGTH 64

#define WINCLASSNAME	TEXT("WinMain")			//	���� ������ Ŭ������
#define WINTITLE		TEXT("DirectX Project")	//	Ŭ���̾�Ʈ Ÿ��Ʋ ���ڿ�
#define TITLESTRING		WINTITLE

// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug������ �׻� ����
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// �ִ� FPS
#if _DEBUG
#define MAX_FPS 0.0
#else
#define MAX_FPS 1.0 / 60.0
#endif

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

#define PI 3.141592653589793

// a struct to define a single vertex
struct VERTEX { FLOAT X, Y, Z; D3DXCOLOR Color; };