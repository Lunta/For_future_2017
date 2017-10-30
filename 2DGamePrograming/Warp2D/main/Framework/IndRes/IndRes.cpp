#include "stdafx.h"
#include "IndRes.h"


CIndRes::CIndRes()
{
}


CIndRes::~CIndRes()
{
}

bool CIndRes::Initialize()
{
	D2D1_FACTORY_OPTIONS options{};
#if defined(_DEBUG)
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	if (FAILED(D2D1CreateFactory<ID2D1Factory3>(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &m_pd2dFactory)))
	{
		MessageBox(nullptr, TEXT("D2D Factory 생성에 실패했습니다."), TEXT("Error!"), MB_OK);
		return false;
	}

	if (FAILED(DWriteCreateFactory(
		  DWRITE_FACTORY_TYPE_SHARED
		, __uuidof(decltype(m_pdwFactory)::InterfaceType)
		, &m_pdwFactory)))
	{
		MessageBox(nullptr, TEXT("DWrtie Factory 생성에 실패했습니다."), TEXT("Error!"), MB_OK);
		return false;
	}

	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		MessageBox(nullptr, TEXT("COM 객체 초기화에 실패했습니다."), TEXT("Error!"), MB_OK);
		return false;
	}
	
	if (FAILED(CoCreateInstance(
		  CLSID_WICImagingFactory
		, nullptr
		, CLSCTX_INPROC_SERVER
		, IID_PPV_ARGS(&m_wicFactory)
	)))
	{
		MessageBox(nullptr, TEXT("WIC 객체 생성에 실패했습니다."), TEXT("Error!"), MB_OK);
		return false;
	}

	return true;
}

bool CIndRes::CreateHwndRenderTarget(HWND hWnd, ID2D1HwndRenderTarget ** m_pd2dRenderTarget, D2D1_RENDER_TARGET_PROPERTIES prop, D2D1_PRESENT_OPTIONS presentOpt)
{
	RECT rc;
	::GetClientRect(hWnd, &rc);
	auto width = rc.right - rc.left;
	auto height = rc.bottom - rc.top;

	return SUCCEEDED(m_pd2dFactory->CreateHwndRenderTarget(
		  prop
		, HwndRenderTargetProperties(hWnd, SizeU(width, height), presentOpt)
		, m_pd2dRenderTarget));
}
