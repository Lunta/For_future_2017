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
	if (FAILED(D2D1CreateFactory<ID2D1Factory3>(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options, &m_pd2dFactory)))
	{
		MessageBox(nullptr, 
			TEXT("D2D Factory 생성에 실패했습니다."), 
			TEXT("Error!"), MB_OK);
		return false;
	}
	if (FAILED(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(decltype(m_pdwFactory)::InterfaceType),
		&m_pdwFactory)))
	{
		MessageBox(nullptr, 
			TEXT("DWrite Factory 생성에 실패했습니다."), 
			TEXT("Error!"), MB_OK);
		return false;
	}
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		MessageBox(nullptr,
			TEXT("COM 객체 초기화에 실패했습니다."),
			TEXT("Error!"), MB_OK);
		return false;
	}
	if (FAILED(CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(&m_pwicFactory))))
	{
		MessageBox(nullptr,
			TEXT("WIC 객체 생성에 실패했습니다."),
			TEXT("Error!"), MB_OK);
		return false;
	}
	return true;
}

bool CIndRes::CreateHwndRenderTarget(HWND hWnd, UINT width, UINT height, ID2D1HwndRenderTarget ** rendertarget)
{
	return SUCCEEDED(
		m_pd2dFactory->CreateHwndRenderTarget(
			  RenderTargetProperties()
			, HwndRenderTargetProperties(hWnd, SizeU(width, height))
			, rendertarget));
}

bool CIndRes::LoadImageFromFile(
	  ID2D1HwndRenderTarget*	pd2dRenderTarget
	, LPCTSTR					pszstrFileName
	, ID2D1Bitmap1**			ppd2dBitmap
	, D2D_RECT_U*				pd2drcImage
	, UINT						nWidth
	, UINT						nHeight
	, WICBitmapTransformOptions nFlipRotation)
{
	ComPtr<IWICBitmapDecoder> pwicBitmapDecoder;

	if (FAILED(
		m_pwicFactory->CreateDecoderFromFilename(
			pszstrFileName
			, NULL
			, GENERIC_READ
			, WICDecodeMetadataCacheOnDemand
			, &pwicBitmapDecoder)))
		return false;

	ComPtr<IWICBitmapFrameDecode> pwicBitmapFrameDecode;
	if (FAILED(
		pwicBitmapDecoder->GetFrame(
			0
			, &pwicBitmapFrameDecode)))
		return false;

	ComPtr<IWICBitmapSource> pwicSource{ pwicBitmapFrameDecode };

	UINT nImageWidth{ 0 };
	UINT nImageHeight{ 0 };

	if (FAILED(pwicSource->GetSize(&nImageWidth, &nImageHeight)))
		return false;

	ComPtr<IWICFormatConverter>		pwicFormatConverter;
	ComPtr<IWICBitmapScaler>		pwicScaler;
	ComPtr<IWICBitmapClipper>		pwicClipper;
	ComPtr<IWICBitmapFlipRotator>	pwicFlipRotator;

	if (pd2drcImage)
	{
		if (pd2drcImage->left < 0)
			pd2drcImage->left = 0;
		if (pd2drcImage->top < 0)
			pd2drcImage->top = 0;
		if (pd2drcImage->right > nImageWidth)
			pd2drcImage->right = nImageWidth;
		if (pd2drcImage->bottom > nImageHeight)
			pd2drcImage->bottom = nImageHeight;

		WICRect wicRect
		{
			static_cast<INT>(pd2drcImage->left)
			, static_cast<INT>(pd2drcImage->top)
			, static_cast<INT>(pd2drcImage->right - pd2drcImage->left)
			, static_cast<INT>(pd2drcImage->bottom - pd2drcImage->top)
		};

		if (FAILED(
			m_pwicFactory->CreateBitmapClipper(&pwicClipper)))
			return false;
		if (FAILED(
			pwicClipper->Initialize(
				pwicSource.Get()
				, &wicRect)))
			return false;

		pwicSource = pwicClipper;
	}

	if ((nWidth != 0) || (nHeight != 0))
	{
		if (nWidth == 0)
			nWidth = UINT(
				static_cast<float>(nHeight) /
				static_cast<float>(nImageHeight) *
				static_cast<float>(nImageWidth));
		if (nHeight == 0)
			nHeight = UINT(
				static_cast<float>(nWidth) /
				static_cast<float>(nImageWidth) *
				static_cast<float>(nImageHeight));

		if (FAILED(
			m_pwicFactory->CreateBitmapScaler(&pwicScaler)))
			return false;
		if (FAILED(
			pwicScaler->Initialize(
				pwicSource.Get()
				, nWidth
				, nHeight
				, WICBitmapInterpolationModeCubic)))
			return false;
		pwicSource = pwicScaler;
	}

	if (nFlipRotation != WICBitmapTransformRotate0)
	{
		if (FAILED(
			m_pwicFactory->CreateBitmapFlipRotator(
				&pwicFlipRotator)))
			return false;
		if (FAILED(
			pwicFlipRotator->Initialize(
				pwicSource.Get()
				, nFlipRotation)))
			return false;
		pwicSource = pwicFlipRotator;
	}

	if (FAILED(
		m_pwicFactory->CreateFormatConverter(
			&pwicFormatConverter)))
		return false;

	if (FAILED(
		pwicFormatConverter->Initialize(
			pwicSource.Get()
			, GUID_WICPixelFormat32bppPBGRA
			, WICBitmapDitherTypeNone
			, NULL
			, 0.f
			, WICBitmapPaletteTypeMedianCut)))
		return false;

	ComPtr<ID2D1Bitmap> bmp;
	if (FAILED(
		pd2dRenderTarget->CreateBitmapFromWicBitmap(
			pwicFormatConverter.Get()
			, NULL
			, &bmp)))
		return false;
	bmp->QueryInterface(ppd2dBitmap);

	return true;
}