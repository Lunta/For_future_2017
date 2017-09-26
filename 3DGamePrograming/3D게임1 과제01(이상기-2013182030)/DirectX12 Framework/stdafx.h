// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//
#pragma once

// 아래 지정된 플랫폼에 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows XP 이상에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0501		// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 이상에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINDOWS 0x0410 // Windows Me 이상에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_IE			// IE 6.0 이상에서만 기능을 사용할 수 있습니다.
#define _WIN32_IE 0x0600	// 다른 버전의 IE에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

// Windows 헤더 파일
#include <windows.h>

// C의 런타임 헤더 파일
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include <d3d12.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>

#include <Mmsystem.h>

// C++ 런타임 헤더 파일
#include <chrono>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.


// 사용자 정의
using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#if _DEBUG
#pragma comment (linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#define TITLE_MX_LENGTH 64

#define TITLESTRING	TEXT("DirectX12 Project | 2013182030 이상기")

#define CLIENT_WIDTH 1200		// 클라이언트 너비
#define CLIENT_HEIGHT 720		// 클라이언트 높이

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

#define KEY_BUFFER_SIZE		256

#define DIR_FORWARD				0x01 // 0b00000001
#define DIR_BACKWARD			0x02 // 0b00000010
#define DIR_LEFT				0x04 // 0b00000100
#define DIR_RIGHT				0x08 // 0b00001000
#define DIR_UP					0x10 // 0b00010000
#define DIR_DOWN				0x20 // 0b00100000

#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))
#define DegreeToRadian(x)		((x)*0.0174532925194444444)
#define RANDOM_NUM(MIN, MAX)	((rand()%((MAX) - (MIN) + 1)) + (MIN))

#define WALL_SIZE 40.0f
#define WALL_HALFSIZE WALL_SIZE / 2.0f

namespace Vector3
{
	inline XMFLOAT3A XMVectorToFloat3A(XMVECTOR& xmvVector)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	inline XMFLOAT3A ScalarProduct(XMFLOAT3A& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3A xmf3Result;
		if (bNormalize)
			XMStoreFloat3A(&xmf3Result, XMVector3Normalize(XMLoadFloat3A(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3A(&xmf3Result, XMLoadFloat3A(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}

	inline XMFLOAT3A Add(const XMFLOAT3A& xmf3Vector1, const XMFLOAT3A& xmf3Vector2)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, XMLoadFloat3A(&xmf3Vector1) + XMLoadFloat3A(&xmf3Vector2));
		return(xmf3Result);
	}

	inline XMFLOAT3A Add(XMFLOAT3A& xmf3Vector1, XMFLOAT3A& xmf3Vector2, float fScalar)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, XMLoadFloat3A(&xmf3Vector1) + (XMLoadFloat3A(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}

	inline XMFLOAT3A Subtract(XMFLOAT3A& xmf3Vector1, XMFLOAT3A& xmf3Vector2)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, XMLoadFloat3A(&xmf3Vector1) - XMLoadFloat3A(&xmf3Vector2));
		return(xmf3Result);
	}

	inline float DotProduct(XMFLOAT3A& xmf3Vector1, XMFLOAT3A& xmf3Vector2)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, XMVector3Dot(XMLoadFloat3A(&xmf3Vector1), XMLoadFloat3A(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline XMFLOAT3A CrossProduct(XMFLOAT3A& xmf3Vector1, XMFLOAT3A& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3A xmf3Result;
		if (bNormalize)
			XMStoreFloat3A(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3A(&xmf3Vector1), XMLoadFloat3A(&xmf3Vector2))));
		else
			XMStoreFloat3A(&xmf3Result, XMVector3Cross(XMLoadFloat3A(&xmf3Vector1), XMLoadFloat3A(&xmf3Vector2)));
		return(xmf3Result);
	}

	inline XMFLOAT3A Normalize(XMFLOAT3A& xmf3Vector)
	{
		XMFLOAT3A m_xmf3Normal;
		XMStoreFloat3A(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3A(&xmf3Vector)));
		return(m_xmf3Normal);
	}

	inline float Length(XMFLOAT3A& xmf3Vector)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, XMVector3Length(XMLoadFloat3A(&xmf3Vector)));
		return(xmf3Result.x);
	}

	inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}

	inline float Angle(XMFLOAT3A& xmf3Vector1, XMFLOAT3A& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3A(&xmf3Vector1), XMLoadFloat3A(&xmf3Vector2)));
	}

	inline XMFLOAT3A TransformNormal(XMFLOAT3A& xmf3Vector, XMMATRIX& xmxm4x4Transform)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3A(&xmf3Vector), xmxm4x4Transform));
		return(xmf3Result);
	}

	inline XMFLOAT3A TransformNormal(XMFLOAT3A& xmf3Vector, XMFLOAT4X4A& xmmtx4x4Matrix)
	{
		return(TransformNormal(xmf3Vector, XMLoadFloat4x4A(&xmmtx4x4Matrix)));
	}

	inline XMFLOAT3A TransformCoord(XMFLOAT3A& xmf3Vector, XMMATRIX& xmxm4x4Transform)
	{
		XMFLOAT3A xmf3Result;
		XMStoreFloat3A(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3A(&xmf3Vector), xmxm4x4Transform));
		return(xmf3Result);
	}

	inline XMFLOAT3A TransformCoord(XMFLOAT3A& xmf3Vector, XMFLOAT4X4A& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4A(&xmmtx4x4Matrix)));
	}
}

namespace Vector4
{
	inline XMFLOAT4A Add(XMFLOAT4A& xmf4Vector1, XMFLOAT4A& xmf4Vector2)
	{
		XMFLOAT4A xmf4Result;
		XMStoreFloat4A(&xmf4Result, XMLoadFloat4A(&xmf4Vector1) + XMLoadFloat4A(&xmf4Vector2));
		return(xmf4Result);
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4A Identity()
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A Multiply(XMFLOAT4X4A& xmmtx4x4Matrix1, XMFLOAT4X4A& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMLoadFloat4x4A(&xmmtx4x4Matrix1) * XMLoadFloat4x4A(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A Multiply(XMFLOAT4X4A& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMLoadFloat4x4A(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4A& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4A(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A RotationYawPitchRoll(float fYaw, float fPitch, float fRoll)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMMatrixRotationRollPitchYaw(XMConvertToRadians(fRoll), XMConvertToRadians(fPitch), XMConvertToRadians(fYaw)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A RotationAxis(XMFLOAT3A& xmf3Axis, float fAngle)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMMatrixRotationAxis(XMLoadFloat3A(&xmf3Axis), XMConvertToRadians(fAngle)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A Inverse(XMFLOAT4X4A& xmmtx4x4Matrix)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4A(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A Transpose(XMFLOAT4X4A& xmmtx4x4Matrix)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4A(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A PerspectiveFovLH(float fFovAngleY, float fAspectRatio, float fNearZ, float fFarZ)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngleY), fAspectRatio, fNearZ, fFarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4A LookAtLH(XMFLOAT3A& xmf3EyePosition, XMFLOAT3A& xmf3LookAtPosition, XMFLOAT3A& xmf3UpDirection)
	{
		XMFLOAT4X4A xmmtx4x4Result;
		XMStoreFloat4x4A(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3A(&xmf3EyePosition), XMLoadFloat3A(&xmf3LookAtPosition), XMLoadFloat3A(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}

namespace Triangle
{
	inline bool Intersect(XMFLOAT3A& xmf3RayPosition, XMFLOAT3A& xmf3RayDirection, XMFLOAT3A& v0, XMFLOAT3A& v1, XMFLOAT3A& v2, float& fHitDistance)
	{
		return(TriangleTests::Intersects(XMLoadFloat3A(&xmf3RayPosition), XMLoadFloat3A(&xmf3RayDirection), XMLoadFloat3A(&v0), XMLoadFloat3A(&v1), XMLoadFloat3A(&v2), fHitDistance));
	}
}

namespace Plane
{
	inline XMFLOAT4A Normalize(XMFLOAT4A& xmf4Plane)
	{
		XMFLOAT4A xmf4Result;
		XMStoreFloat4A(&xmf4Result, XMPlaneNormalize(XMLoadFloat4A(&xmf4Plane)));
		return(xmf4Result);
	}
}
