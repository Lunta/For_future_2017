// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment (linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

// Windows ��� ����
#include <windows.h>

// C�� ��Ÿ�� ��� ����
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include <wrl.h>
#include <shellapi.h>
#include <timeapi.h>
#include <Mmsystem.h>

// C++�� ��Ÿ�� ��� ����
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

// Direct ��� ����
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
/*#pragma comment(lib, "DirectXTex.lib") */

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

#define FRAME_BUFFER_WIDTH		640
#define FRAME_BUFFER_HEIGHT		480

#define MAX_LIGHTS				30 
#define MAX_MATERIALS			8 

#define POINT_LIGHT				1
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3

//#define _WITH_CB_GAMEOBJECT_32BIT_CONSTANTS
//#define _WITH_CB_GAMEOBJECT_ROOT_DESCRIPTOR
#define _WITH_CB_WORLD_MATRIX_DESCRIPTOR_TABLE

// ����� ����
#include "D3DDeviceIndRes.h"
#include "XMathHelper.h"

#define MAZE_SIZE 15
struct Maze
{
	typedef enum {
		Path = 0,
		Wall,
		Entrance,
		Exit
	}Structure;

	const int m_Width = MAZE_SIZE;
	const int m_Length = MAZE_SIZE;

	unsigned char m_ppMap[MAZE_SIZE][MAZE_SIZE] = {
		{ 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1 },
		{ 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1 },
		{ 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1 },
		{ 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1 },
		{ 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1 },
		{ 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1 },
		{ 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1 },
		{ 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1 },
		{ 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1 },
		{ 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1 },
		{ 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
		
	};

	XMFLOAT3 m_xmf3EntrancePos;
	XMFLOAT3 m_xmf3ExitPos;

	UINT	 m_nPathesPos;
	XMFLOAT3 *m_pxmf3PathesPos;
};