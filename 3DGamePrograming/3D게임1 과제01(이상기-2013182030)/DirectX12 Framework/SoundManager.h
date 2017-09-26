#pragma once

#ifdef _M_IX86
#pragma comment (lib, "fmod_vc.lib")
#elif _M_X64
#pragma comment (lib, "fmod64_vc.lib")
#endif

#include "inc\fmod.hpp"
using namespace FMOD;

// 동시실행가능한 사운드(채널) 갯수.
#define Max_Channel 3000

class CSoundManager
{
public:
	///////////////// BGM ////////////////////
	typedef enum 
	{
		Dummy_BGM = 0,
		Count_BGM,
		Stop_BGM
	}BGM_SOUND;
	///////////////// UI /////////////////////
	typedef enum
	{
		Dummy_UI = 0,
		Count_UI,
		Stop_UI
	}UI_SOUND;


private:
	FMOD_SYSTEM			*g_System{ nullptr };
						
	FMOD_SOUND			*g_BGM_Sound[BGM_SOUND::Count_BGM];
	FMOD_SOUND			*g_UI_Sound[UI_SOUND::Count_UI];
						
	FMOD_CHANNELGROUP	*g_BGM_ChannelGroup{ nullptr };
	FMOD_CHANNELGROUP	*g_UI_ChannelGroup{ nullptr };
						
	FMOD_CHANNEL		*g_BGM_Channel{ nullptr };
	FMOD_CHANNEL		*g_UI_Channel{ nullptr };
public:
	CSoundManager();
	~CSoundManager();

	void Initialize();
	
	void CreateSound_BGM();
	void CreateSound_UI();

	void Update(float fTimeElapsed);

	void PlaySnd_BGM(int);
	void PlaySnd_UI(int);

	void Destroy();
};

