
struct BasicConstantBuffer
{
	mat4f worldViewProj;
	vec4f modelColor;
};

class RendererD3D11 : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type() override
    {
        return RendererType::D3D11;
    }
	RendererD3D11& castD3D11() override
	{
		return *this;
	}

	ID3D11Device& device()
	{
		return *_device;
	}
	ID3D11DeviceContext& context()
	{
		return *_context;
	}

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color) override;
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color) override;

    void renderSplashA(const vec3f &focusColorA, const vec3f &focusColorB, const vec2f &pulse);
    void renderSplashB(const vec2f &kernelOffset);

    void renderFullScreen(const vec4f &color);
    void renderFullScreen(Texture &tex, const vec4f &color);

    void renderGaussian(const vec2f &kernelOffset);

    void renderMotionBlur(const vec4f &color);

    void createViews();

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

    ID3D11Device *_device;
    ID3D11DeviceContext *_context;
    ID3D11RenderTargetView *_renderTargetView;
    ID3D11Debug *_debug;

    IDXGISwapChain *_swapChain;
    DXGI_SWAP_CHAIN_DESC _swapChainDesc;	//settings


    ID3D11RasterizerState *_rasterState;
    D3D11_RASTERIZER_DESC _rasterDesc;

    ID3D11Texture2D *_depthBuffer;
    ID3D11DepthStencilState *_depthState;
    ID3D11DepthStencilView *_depthStencilView;

    ID3D11SamplerState *_samplerState;

    ID3D11Texture2D *_captureBuffer;

    D3D11ShaderManager _shaderManager;
	D3D11ShaderPair *_quadShader;
	D3D11TriMesh _quadMesh;

	D3D11ConstantBuffer<BasicConstantBuffer> _constantBuffer;

    D3D_FEATURE_LEVEL _featureLevel;

    int _width, _height;

    vec2f _windowSize;
    mat4f _windowToNDC;
    mat4f _quadToNDC;

    float _motionBlurMinAlpha;
    int _motionBlurFramesLeft;
    bool _firstMotionBlurFrame;
};
