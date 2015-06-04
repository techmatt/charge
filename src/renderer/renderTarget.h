
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
        _dimensions = vec2i(0, 0);
    }

    void init(Renderer &renderer, const vec2i &dimensions);

    void clear(const vec4f &color)
    {
        bindAsRenderTarget();
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        unbindRenderTarget();
    }

    void bindAsRenderTarget();
    void unbindRenderTarget();

    Bitmap getImage();

    void bindAsTexture();

    vec2i dimensions() const
    {
        return _dimensions;
    }

private:
    
    vec2i _dimensions;
    GLuint _texture;
    GLuint _frameBuffer;
};
