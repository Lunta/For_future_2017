#pragma once
class CIndRes
{
public:
	CIndRes();
	~CIndRes();

	bool Initialize();
	bool CreateHwndRenderTarget(
		HWND						hWnd
		, UINT						width
		, UINT						height
		, ID2D1HwndRenderTarget**	rendertarget);
	
	bool LoadImageFromFile(
		  ID2D1HwndRenderTarget*	pd2dRenderTarget
		, LPCTSTR					pszstrFileName
		, ID2D1Bitmap1**			ppd2dBitmap
		, D2D_RECT_U*				pd2drcImage = nullptr
		, UINT						nWidth = 0
		, UINT						nHeight = 0
		, WICBitmapTransformOptions	nFlipRotation = WICBitmapTransformRotate0);

	ID2D1Factory3*		Getd2dFactory() const { return m_pd2dFactory.Get(); }
	IDWriteFactory3*	GetdwFactory()	const { return m_pdwFactory.Get(); }
	IWICImagingFactory* GetwicFactory() const { return m_pwicFactory.Get(); }

private:
	ComPtr<ID2D1Factory3>		m_pd2dFactory;
	ComPtr<IDWriteFactory3>		m_pdwFactory;
	ComPtr<IWICImagingFactory>	m_pwicFactory;
};

