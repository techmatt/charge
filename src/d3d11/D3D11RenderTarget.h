
class D3D11RenderTarget
{
public:
	D3D11RenderTarget()
	{
		_graphics = nullptr;
		_width = 0;
		_height = 0;

		_targets = nullptr;
		_targetsRTV = nullptr;
		_targetsSRV = nullptr;

		_depthStencil = nullptr;
		_depthStencilDSV = nullptr;
		_depthStensilSRV = nullptr;

		_captureTextures = nullptr;
		_captureDepth = nullptr;

		_bHasSRVs = false;
	}

	D3D11RenderTarget(RendererD3D11 &g, unsigned int width, unsigned int height, const vector<DXGI_FORMAT>& formats = vector < DXGI_FORMAT > {DXGI_FORMAT_R8G8B8A8_UNORM}, bool createSRVs = false)
    {
		_graphics = nullptr;
		_width = 0;
		_height = 0;

		_targets = nullptr;
		_targetsRTV = nullptr;
		_targetsSRV = nullptr;

		_depthStencil = nullptr;
		_depthStencilDSV = nullptr;
		_depthStensilSRV = nullptr;

		_captureTextures = nullptr;
		_captureDepth = nullptr;

		_bHasSRVs = false;

        load(g, width, height, formats, createSRVs);
    }

	~D3D11RenderTarget()
	{
		release();
	}

    // create a new render target with given width and height. Also creates an equal-sized depth buffer.
	void load(RendererD3D11 &g, unsigned int width, unsigned int height, const vector<DXGI_FORMAT>& formats = vector < DXGI_FORMAT > {DXGI_FORMAT_R8G8B8A8_UNORM}, bool createSRVs = false) 
	{
		_graphics = &g;
		_width = width;
		_height = height;

		_textureFormats = formats;
		_bHasSRVs = createSRVs;

		reset();
	}

	void release();
	void reset();

    // sets the render and depth buffers as the render target for the current device.
    // to return to the original graphics device render target, call bindRenderTarget() on the graphics device.
    void bind();

	//! restores the default render target
	void unbind() {
		_graphics->bindMainRenderTarget();
	}

    // clears the render and depth buffers
    void clear(const vec4f& clearColor = vec4f(0.0f), float clearDepth = 1.0f);
    void clearColor(const vec4f& clearColor = vec4f(0.0f));
	void clearDepth(float clearDepth = 1.0f);
	  
    unsigned int getWidth() { return _width; }
    unsigned int getHeight() { return _height; }

	unsigned int getNumTargets() const {
		return (unsigned int)_textureFormats.size();
	}
	bool hasSRVs() const {
		return _bHasSRVs;
	}
private:
	RendererD3D11 *_graphics;
	unsigned int _width;
	unsigned int _height;

	vector<DXGI_FORMAT>	_textureFormats;

    ID3D11Texture2D**			_targets;
	ID3D11RenderTargetView**	_targetsRTV;
	ID3D11ShaderResourceView**	_targetsSRV;

	ID3D11Texture2D*			_depthStencil;
	ID3D11DepthStencilView*		_depthStencilDSV;
	ID3D11ShaderResourceView*	_depthStensilSRV;

    ID3D11Texture2D **_captureTextures;    
    ID3D11Texture2D *_captureDepth;   
    
	bool _bHasSRVs;
};
