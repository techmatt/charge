

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
    Texture(Renderer &renderer, TTF_Font *font, const string &text, RGBColor color, int wrapWidth, float glowStrength, RGBColor glowColor)
    {
        initInternal();
        drawText(renderer, font, text, color, wrapWidth, glowStrength, glowColor);
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
        releaseSDLMemory();
        releaseOpenGLMemory();
    }

    void releaseSDLMemory()
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

    void releaseOpenGLMemory()
    {
        if (_OpenGLTexture != 0) glDeleteTextures(1, &_OpenGLTexture);
    }

    void load(Renderer &renderer, const string &filename);
    void load(Renderer &renderer, const Bitmap &bmp);

    void drawText(Renderer &renderer, TTF_Font *font, const string &text, RGBColor color, int wrapWidth, float glowStrength, RGBColor glowColor);

    void bindOpenGL();

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
        _renderer = nullptr;
        _SDLTexture = nullptr;
        _SDLSurface = nullptr;
        _OpenGLTexture = 0;
    }
    void initSDL();
    void initOpenGL(bool useMipmaps);

    Renderer* _renderer;

    Bitmap _bmp;
    SDL_Texture* _SDLTexture;
    SDL_Surface* _SDLSurface;
    GLuint _OpenGLTexture;
};
