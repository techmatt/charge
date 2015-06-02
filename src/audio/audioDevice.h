
#if 0
struct AudioSample
{

    void load(const string &filename)
    {
        // the specs, length and buffer of our wav are filled
        if (SDL_LoadWAV(filename.c_str(), &spec, &buffer, &length) == NULL)
        {
            cout << "Failed to load " << filename << endl;
            return;
        }

        // set the callback function
        spec.callback = audioCallback;
        spec.userdata = this;
        // set our global static variables
        audio_pos = wav_buffer; // copy sound buffer
        audio_len = wav_length; // copy file length

        /* Open the audio device */
        if (SDL_OpenAudio(&wav_spec, NULL) < 0){
            fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
            exit(-1);
        }

        /* Start playing */
        SDL_PauseAudio(0);

        // wait until we're don't playing
        while (audio_len > 0) {
            SDL_Delay(100);
        }

        // shut everything down
        SDL_CloseAudio();
        SDL_FreeWAV(wav_buffer);
    }

    Uint32 length;
    Uint8 *buffer;
    SDL_AudioSpec spec;
};

class AudioDevice
{
    static void audioCallback(void *userData, Uint8 *stream, int requestedLength)
    {
        AudioSample &sample = *(AudioSample *)userData;
        if (sample.length == 0)
            return;

        int samplesToProcess = (requestedLength > sample.length ? sample.length : requestedLength);
        SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

        audio_pos += len;
        audio_len -= len;
    }

    void init()
    {
        SDL_AudioSpec desiredSpec;
        SDL_AudioDeviceID dev;

        SDL_zero(desiredSpec);
        desiredSpec.freq = 44100;
        desiredSpec.format = AUDIO_F32;
        desiredSpec.channels = 2;
        desiredSpec.samples = 4096;
        desiredSpec.callback = audioCallback;

        dev = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
        if (dev == 0)
        {
            cout << "Failed to open audio: " << SDL_GetError() << endl;
            return;
        }

        SDL_PauseAudioDevice(dev, 0);  // start audio playing.
        //SDL_Delay(5000);  // let the audio callback play some sound for 5 seconds.
        //SDL_CloseAudioDevice(dev);
    }

    RunningAudioSample channels

    SDL_AudioSpec spec;
    SDL_AudioDeviceID device; 
};

#endif