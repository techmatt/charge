#include "Main.h"

void GameAudio::Init()
{
	Audio.Init();
	
	ChargeSound.Open(L"Sounds\\AmpCharge.wav", NULL, WAVEFILE_READ);
	DischargeSound.Open(L"Sounds\\AmpDischarge.wav", NULL, WAVEFILE_READ);
	SplitterSound.Open(L"Sounds\\Splitter.wav", NULL, WAVEFILE_READ);
	ChargeDeathSound.Open(L"Sounds\\ChargeDeath.wav", NULL, WAVEFILE_READ);
    MusicSound.Open(L"Sounds\\Music.wav", NULL, WAVEFILE_READ);
}
