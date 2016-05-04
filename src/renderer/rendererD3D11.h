
class RendererD3D11 : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type() override
    {
        return RendererType::D3D11;
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
    
    ID3D11Device *m_device;
    ID3D11DeviceContext *m_context;
    ID3D11RenderTargetView *m_renderTargetView;
    ID3D11Debug *m_debug;

    IDXGISwapChain *m_swapChain;
    DXGI_SWAP_CHAIN_DESC m_swapChainDesc;	//settings


    ID3D11RasterizerState *m_rasterState;
    D3D11_RASTERIZER_DESC m_rasterDesc;

    ID3D11Texture2D *m_depthBuffer;
    ID3D11DepthStencilState *m_depthState;
    ID3D11DepthStencilView *m_depthStencilView;

    ID3D11SamplerState *m_samplerState;

    ID3D11Texture2D *m_captureBuffer;

    //D3D11ShaderManager m_shaderManager;

    D3D_FEATURE_LEVEL m_featureLevel;

    vec2f _windowSize;
    mat4f _windowToNDC;
    mat4f _quadToNDC;

    float _motionBlurMinAlpha;
    int _motionBlurFramesLeft;
    bool _firstMotionBlurFrame;
};
