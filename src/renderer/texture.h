
class Texture
{
public:
    Texture()
    {
        initInternal();
    }
    Texture(RendererSDL &renderer, const string &filename)
    {
        load(renderer, filename);
    }
    ~Texture()
    {
        releaseSDLMemrory();
    }

    void releaseSDLMemrory()
    {
        if (_SDLTexture != nullptr)
        {
            SDL_DestroyTexture(_SDLTexture);
            _SDLTexture = nullptr;
        }
        if (_SDLSurface != nullptr)
        {
            SDL_FreeSurface(_SDLSurface);
            _SDLSurface = nullptr;
        }
    }

    void load(RendererSDL &renderer, const string &filename);

    void loadRenderTarget(int width, int height);

    const Bitmap& bmp() const
    {
        return _bmp;
    }

    SDL_Texture* SDL()
    {
        return _SDLTexture;
    }

private:
    void initInternal()
    {
        _SDLTexture = nullptr;
        _SDLSurface = nullptr;
    }
    void initSDL(RendererSDL &renderer);

    Bitmap _bmp;
    SDL_Texture* _SDLTexture;
    SDL_Surface* _SDLSurface;
    bool _renderTarget;
};
