#include "stdafx.h"
#include "SoundManager.h"

CSoundManager::CSoundManager()
{
}
CSoundManager::~CSoundManager()
{
}

void CSoundManager::Initialize()
{
	// 사운드 시스템을 생성하는 코드
	FMOD_System_Create(&g_System);
	// 채널수와 사운드 모드 설정
	FMOD_System_Init(g_System, Max_Channel, FMOD_INIT_NORMAL, NULL);             
	// 채널그룹 생성
	FMOD_System_CreateChannelGroup(g_System, "BGM", &g_BGM_ChannelGroup);
	FMOD_System_CreateChannelGroup(g_System, "UI", &g_UI_ChannelGroup);
	FMOD_System_CreateChannelGroup(g_System, "Effect", &g_Effect_ChannelGroup);
	// 채널그룹에 채널셋팅
	FMOD_Channel_SetChannelGroup(g_BGM_Channel, g_BGM_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_UI_Channel, g_UI_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_Enemy_Channel, g_Effect_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_Player_Channel, g_Effect_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_Boss_Channel, g_Effect_ChannelGroup);
	// 사운드 생성
	CreateSound_BGM();
	// 볼륨 크기조절
	FMOD_Channel_SetVolume(g_Enemy_Channel, 0.2);
}

void CSoundManager::CreateSound_BGM() {

	//FMOD_System_CreateSound(g_System, "Resource\\sound\\Logo.wav", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Logo_BGM]);
	//FMOD_System_CreateSound(g_System, "Resource\\sound\\Title_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Title_BGM]);
	//FMOD_System_CreateSound(g_System, "Resource\\sound\\Lobby_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Lobby_BGM]);
	//FMOD_System_CreateSound(g_System, "Resource\\sound\\Stage_0-1_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Stage_BGM]);
	//FMOD_System_CreateSound(g_System, "Resource\\sound\\BOSSStage_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Boss_Stage_BGM]);
	//FMOD_System_CreateSound(g_System, "Resource\\sound\\Game_Over.mp3", FMOD_DEFAULT, 0, &g_BGM_Sound[BGM_SOUND::GameOver_BGM]);
	//FMOD_System_CreateSound(g_System, "Resource\\sound\\Game_Clear.wav", FMOD_DEFAULT, 0, &g_BGM_Sound[BGM_SOUND::GameClear_BGM]);
}

void CSoundManager::Update(float fTimeElapsed)
{
	FMOD_System_Update(g_System);
}

void CSoundManager::PlaySnd_BGM(int set) {
	switch (set) {
	case BGM_SOUND::Logo_BGM:
		//FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::Logo_BGM], NULL, false, &g_BGM_Channel);
		break;
	
	case BGM_SOUND::Stop_BGM:
		//FMOD_Channel_Stop(g_BGM_Channel);
		break;
	}
}

void CSoundManager::Destroy() {
	//for (int i = 0; i < BGM_SOUND::Count_BGM; ++i) {
	//	FMOD_Sound_Release(g_BGM_Sound[i]);
	//}
	//for (int i = 0; i < UI_SOUND::Count_UI; ++i) {
	//	FMOD_Sound_Release(g_UI_Sound[i]);
	//}
	//for (int i = 0; i < PLAYER_SOUND::Count_P; ++i) {
	//	FMOD_Sound_Release(g_Player_Sound[i]);
	//}
	//for (int i = 0; i < ENEMY_SOUND::Enemy_Count; ++i) {
	//	FMOD_Sound_Release(g_Enemy_Sound[i]);
	//}
	//for (int i = 0; i < BOSS_SOUND::Boss_Count; ++i) {
	//	FMOD_Sound_Release(g_Boss_Sound[i]);
	//}
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}