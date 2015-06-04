
//http://lazyfoo.net/tutorials/SDL/21_sound_effects_and_music/index.php
//https://www.freesound.org/people/grunz/sounds/109662/

class AudioDevice
{
public:
    AudioDevice()
    {
        music = nullptr;
    }

    void init()
    {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        {
            cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
            return;
        }

        const string musicFilename = params().assetDir + "sounds/Music.wav";
        music = Mix_LoadMUS(musicFilename.c_str());
        if (music == nullptr)
        {
            cout << "Failed to load music file: " << musicFilename << endl;
            return;
        }

        Mix_PlayMusic(music, -1);

        playSoundEffects = true;
        playMusic = false;

        setMusic(playMusic);
    }

    void playEffect(const string &name, bool soft)
    {
        if (soft)
            playEffect(name + "Soft");
        else
            playEffect(name);
    }

    void playEffect(const string &name)
    {
        if (!playSoundEffects)
            return;
        if (audioEffects.count(name) == 0)
        {
            const string filename = params().assetDir + "sounds/" + name + ".wav";
            Mix_Chunk *newEffect = Mix_LoadWAV(filename.c_str());
            if (newEffect == NULL)
            {
                cout << "Failed to load sound file: " << name << endl;
                return;
            }
            audioEffects[name] = newEffect;
        }
        
        Mix_Chunk *effect = audioEffects[name];
        Mix_PlayChannel(-1, effect, 0);
    }

    void setMusic(bool newPlayState)
    {
        if (newPlayState)
            Mix_ResumeMusic();
        else
            Mix_PauseMusic();
        playMusic = newPlayState;
    }

    bool playSoundEffects;
    bool playMusic;

private:
    map<string, Mix_Chunk *> audioEffects;
    Mix_Music *music;
};
