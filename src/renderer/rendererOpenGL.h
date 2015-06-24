
//
// This should be a renderer, but I don't feel like keeping the renderer interface up-to-date until we need to abstract.
//
class RendererOpenGL : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type() override
    {
        return RendererType::OpenGL;
    }

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color) override;
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color) override;

    void renderSplashA(const vec3f &focusColorA, const vec3f &focusColorB, const vec2f &pulse);
    void renderSplashB(const vec2f &kernelOffset);

    void renderFullScreen(const vec4f &color);
    void renderFullScreen(Texture &tex, const vec4f &color);

    void renderGaussian(const vec2f &kernelOffset);

    void renderMotionBlur(const vec4f &color);

    void clear() override;
    void present() override;

    void setRenderTarget(Texture &target) override;
    void setDefaultRenderTarget() override;
    
    vec2i getWindowSize() override;

	CoordinateFrame getWindowCoordinateFrame();	// this is the coordinate frame of the rendered area in the entire window.

    // binds the motion blur render target (if active) or the window's render target
    void bindMainRenderTarget() override;

    virtual bool motionBlurActive() override
    {
        return (_motionBlurFramesLeft > 0);
    }

    void initMotionBlur(float minAlpha, int frameCount)
    {
        _motionBlurMinAlpha = minAlpha;
        _firstMotionBlurFrame = true;
        _motionBlurFramesLeft = frameCount;
    }

private:
    void updateWindowSize();

    mat4f makeWindowTransform(const rect2f &rect, float depth);
    mat4f makeWindowTransform(const rect2f &rect, float depth, float rotation);

    SDL_Window *_window;
    SDL_GLContext _context;
    
    GLQuad _quad;
    GLProgram _quadProgram;
    GLProgram _motionProgram;
    GLProgram _gaussianProgram;
    GLProgram _splashAProgram;
    GLProgram _splashBProgram;

    RenderTarget _motionBlurRenderTargetA;
    RenderTarget _motionBlurRenderTargetB;

    vec2f _windowSize;
    mat4f _windowToNDC;
    mat4f _quadToNDC;

    float _motionBlurMinAlpha;
    int _motionBlurFramesLeft;
    bool _firstMotionBlurFrame;
};
