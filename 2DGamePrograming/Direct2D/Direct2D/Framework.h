#pragma once

#include "Scene.h"
#include "IndRes.h"

class CUnit;

template<typename Enum>				// Enum class�� �������� �˷��־� ���ڿ� �����ϴ� ���� ��ȯ�ϴ� �Լ��Դϴ�.
inline constexpr auto GetEnumValueByType(Enum enumerator) noexcept	// enum class E : int { a,b,c }; �� ��,
{																	// auto data = GetEnumValueByType(E::a);
	return static_cast<std::underlying_type_t<Enum>>(enumerator);	// data�� ������ int �̰�, ���� 0 �Դϴ�.
}

#define GetSceneEnumInt(Enum) GetEnumValueByType(CScene::CurrentScene::Enum)

class CFramework
{
private:
	HWND							m_hWnd{ NULL };
	RECT							m_rcClient{ 0,0,0,0 };

	CIndRes							m_IndRes;
	ComPtr<ID2D1HwndRenderTarget>	m_pd2dRenderTarget{ nullptr };
	ComPtr<IDWriteTextFormat>		m_pdwTextFormat{ nullptr };
	ComPtr<IDWriteTextLayout>		m_pdwTextLayout{ nullptr };
	ComPtr<ID2D1Bitmap1>			m_pd2dbmpTest{ nullptr };
	float currImg = 0;

	CScene							*m_pCurrentScene;
	CScene							*m_arrScene[GetEnumValueByType(CScene::CurrentScene::count)];

	std::chrono::system_clock::time_point m_current_time;
	std::chrono::duration<double> m_timeElapsed; // �ð��� �󸶳� ������
	double m_fps;

	TCHAR m_CaptionTitle[TITLE_MX_LENGTH];
	int m_TitleLength;
	std::chrono::system_clock::time_point m_LastUpdate_time;
	std::chrono::duration<double> m_UpdateElapsed; // �ð��� �󸶳� ������

public:
	CFramework();
	~CFramework();

	bool OnCreate(HWND hWnd, const RECT& rc);
	bool OnDestroy();
	void BuildObjects();
	void BuildScene();
	
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	HRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void Update(float fTimeElapsed);
	void Render();

	void ChangeScene(CScene::CurrentScene tag, bool bDestroy = false);

	void FrameAdvance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

