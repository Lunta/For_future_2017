#pragma once

class CIndRes
{
public:
	CIndRes();
	~CIndRes();

	bool Initialize();
	bool CreateHwndRenderTarget(HWND hWnd, ID2D1HwndRenderTarget** m_pd2dRenderTarget, D2D1_RENDER_TARGET_PROPERTIES prop = D2D1::RenderTargetProperties(), D2D1_PRESENT_OPTIONS presentOpt = D2D1_PRESENT_OPTIONS_IMMEDIATELY);

	ID2D1Factory3*		d2dFactory()	const { return m_pd2dFactory.Get(); }
	IDWriteFactory3*	dwFactory()		const { return m_pdwFactory.Get(); }
	IWICImagingFactory*	wicFactory()	const { return m_wicFactory.Get(); }

private:
	ComPtr<ID2D1Factory3>		m_pd2dFactory;
	ComPtr<IDWriteFactory3>		m_pdwFactory;
	ComPtr<IWICImagingFactory>	m_wicFactory;
};
