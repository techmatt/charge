struct GameAudio
{
	void Init();

	AudioDevice Audio;
	WaveFile DischargeSound;
	WaveFile ChargeSound;
	WaveFile SplitterSound;
	//WaveFile SoundBuildingSounds[MaxChargeLevel];
	WaveFile ChargeDeathSound;
    WaveFile MusicSound;
    bool PlaySounds;
};