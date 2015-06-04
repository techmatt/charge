
//
// This only supports OpenGL
//
class RenderTarget
{
public:
    RenderTarget()
    {
        _texture = 0;
        _frameBuffer = 0;
    }

    ~RenderTarget()
    {
        releaseMemory();
    }

    void releaseMemory()
    {
        if (_texture != 0) glDeleteTextures(1, &_texture);
        if (_frameBuffer != 0) glDeleteFramebuffers(1, &_frameBuffer);
    }

    void init(Renderer &renderer, const vec2i &dimensions);

    void bindAsRenderTarget();
    void unbindRenderTarget();

    Bitmap getImage();

    void bindAsTexture();

    vec2i dimensions() const
    {
        return _dimensions;
    }

private:
    void initInternal()
    {
        _texture = 0;
    }
    void initSDL(Renderer &renderer);
    void initOpenGL(bool useMipmaps);

    vec2i _dimensions;
    GLuint _texture;
    GLuint _frameBuffer;
};
