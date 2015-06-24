
class RenderTarget
{
public:
    RenderTarget()
    {
        _texture = 0;
        _frameBuffer = 0;
        _SDLTexture = nullptr;
        _renderer = nullptr;
    }

    ~RenderTarget()
    {
        releaseMemory();
    }

    void releaseMemory()
    {
        if (_texture != 0) glDeleteTextures(1, &_texture);
        if (_frameBuffer != 0) glDeleteFramebuffers(1, &_frameBuffer);
        if (_SDLTexture) SDL_DestroyTexture(_SDLTexture);
        _dimensions = vec2i(0, 0);
    }

    void init(Renderer &renderer, const vec2i &dimensions);

    void clear(const vec4f &color);

    void bindAsRenderTarget();
    void unbindRenderTarget();

    Bitmap getImage();

    void bindAsTexture();

    vec2i dimensions() const
    {
        return _dimensions;
    }

private:
    Renderer *_renderer;

    vec2i _dimensions;
    GLuint _texture;
    GLuint _frameBuffer;

    SDL_Texture *_SDLTexture;
};
