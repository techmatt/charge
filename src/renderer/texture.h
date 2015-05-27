

//
// this *could* be astracted into TextureSDL and TextureGL, but for now it is unified.
//

class Texture
{
public:
    Texture()
    {
        initInternal();
    }
    Texture(TTF_Font *font, const char *text, RGBColor color)
    {
        initInternal();
        drawText(font, text, color);
    }
    Texture(Renderer &renderer, const string &filename)
    {
        initInternal();
        load(renderer, filename);
    }
    Texture(Renderer &renderer, const Bitmap &bmp)
    {
        initInternal();
        load(renderer, bmp);
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

    void load(Renderer &renderer, const string &filename);
    void load(Renderer &renderer, const Bitmap &bmp);

    void drawText(TTF_Font *font, const char *text, RGBColor color);

    void bindOpenGL();

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
    void initSDL(Renderer &renderer);
    void initOpenGL(bool useMipmaps);

    Bitmap _bmp;
    SDL_Texture* _SDLTexture;
    SDL_Surface* _SDLSurface;
    GLuint _OpenGLTexture;
    bool _renderTarget;
};
