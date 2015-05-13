
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

    void load(RendererSDL &renderer, const string &filename);

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
};
