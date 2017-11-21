#pragma once



/// <summary>
/// 현재 윈도우 프로시저와 연결된 클래스 인스턴스를 가져옵니다.
/// </summary>
///	<param name = "hWnd"> 윈도우 핸들입니다. 해당 윈도우 핸들과 연결된 클래스 인스턴스를 반환합니다. </param>
/// <returns> 연결된 클래스 인스턴스 포인터를 반환합니다. </returns>
template <typename FrameworkPtr>
FrameworkPtr GetUserDataPtr(HWND hWnd) noexcept
{
	return reinterpret_cast<FrameworkPtr>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

/// <summary>
/// 윈도우 프로시저와 클래스 인스턴스를 연결합니다.
/// </summary>
///	<param name = "hWnd"> 윈도우 핸들입니다. 이 핸들 값으로 데이터를 찾을 수 있게 됩니다. </param>
///	<param name = "ptr"> 윈도우 핸들과 연결될 데이터입니다. 대체로 클래스 인스턴스의 포인터를 사용합니다. </param>
/// <returns> LONG_PTR 형식으로 결과값을 반환합니다. </returns>
inline auto SetUserDataPtr(HWND hWnd, LPVOID ptr) noexcept
{
	return ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 가로 길이입니다.
/// </summary>
int GetMarginWidth();

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 세로 길이입니다.
/// </summary>
int GetMarginHeight();

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 세로 길이입니다.
/// </summary>
HINSTANCE GetHInstance();

/// <summary>
/// HRESULT가 S_FALSE 일 경우 입력된 메시지를 입력한 메시지 박스를 출력하며 true를 반환합니다.
/// </summary>
///	<param name = "hr"> 평가할 HRSULT 객체입니다. S_FALSE이면 메시지 박스를 출력합니다. </param>
///	<param name = "msg"> 메시지입니다. 생략 가능합니다. </param>
///	<param name = "captionMsg"> 캡션 문장입니다. 생략 가능합니다. </param>
/// <returns> HRESULT 값이 S_FALSE일 경우 true를 반환합니다. </returns>
inline bool FailureMessage(HRESULT hr, LPCTSTR msg = TEXT(""), LPCTSTR captionMsg = TEXT("Error!"))
{
	if (FAILED(hr))
	{
		MessageBox(NULL, msg, captionMsg, MB_OK);
		return true;
	}
	return false;
}

/// <summary>
/// 인자로 받은 두 값을 t 값만큼 선형 보간합니다.
/// </summary>
///	<param name = "start"> 시작 값입니다. </param>
///	<param name = "end"> 끝 값입니다. </param>
///	<param name = "t"> 보간 값입니다. 대체로 0부터 1 사이의 숫자입니다. </param>
template<typename Ty>
constexpr Ty Interpolation(Ty start, Ty end, float t)
{
	return static_cast<Ty>(start * (1.f - t) + end * t);
}




