
#include "main.h"

#ifdef INCLUDE_D3D

const bool debuggingD3D11 = false;

void RendererD3D11::init(SDL_Window *window)
{
    _window = window;

    _motionBlurFramesLeft = 0;
    _motionBlurMinAlpha = 1.0f;

    SDL_SysWMinfo windowInfo;
	SDL_VERSION(&windowInfo.version);
	if (!SDL_GetWindowWMInfo(window, &windowInfo))
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't get window information: %s", SDL_GetError());
	}

    SDL_GetWindowSize(_window, &_width, &_height);

	ZeroMemory(&_swapChainDesc, sizeof(_swapChainDesc));
	_swapChainDesc.BufferCount = 1;
	_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_swapChainDesc.SampleDesc.Count = 1;
	_swapChainDesc.SampleDesc.Quality = 0;
	_swapChainDesc.Windowed = TRUE;
	_swapChainDesc.OutputWindow = windowInfo.info.win.window;
    _swapChainDesc.BufferDesc.Width = _width;
    _swapChainDesc.BufferDesc.Height = _height;
	
    UINT createDeviceFlags = 0;
    if(debuggingD3D11)
    	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    
    D3D_FEATURE_LEVEL featureLevels[] =
    {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    D3D_VALIDATE(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
        D3D11_SDK_VERSION, &_swapChainDesc, &_swapChain, &_device, &_featureLevel, &_context));

    createViews();

    //
    // Setup the rasterizer state
    //
    _rasterDesc.AntialiasedLineEnable = false;
    _rasterDesc.CullMode = D3D11_CULL_NONE;
    _rasterDesc.DepthBias = 0;
    _rasterDesc.DepthBiasClamp = 0.0f;
    _rasterDesc.DepthClipEnable = true;
    _rasterDesc.FillMode = D3D11_FILL_SOLID;
    _rasterDesc.FrontCounterClockwise = false;
    _rasterDesc.MultisampleEnable = false;
    _rasterDesc.ScissorEnable = false;
    _rasterDesc.SlopeScaledDepthBias = 0.0f;

    D3D_VALIDATE(_device->CreateRasterizerState(&_rasterDesc, &_rasterState));
    _context->RSSetState(_rasterState);


    //
    // Setup the depth state
    //
    D3D11_DEPTH_STENCIL_DESC depthStateDesc;
    depthStateDesc.DepthEnable = true;
    depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStateDesc.StencilEnable = false;
    D3D_VALIDATE(_device->CreateDepthStencilState(&depthStateDesc, &_depthState));
    _context->OMSetDepthStencilState(_depthState, 1);


    //
    // Setup the sampler state
    //
    D3D11_SAMPLER_DESC samplerStateDesc;
    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.MipLODBias = 0.0f;
    samplerStateDesc.MaxAnisotropy = 1;
    samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerStateDesc.MinLOD = -FLT_MAX;
    samplerStateDesc.MaxLOD = FLT_MAX;
    D3D_VALIDATE(_device->CreateSamplerState(&samplerStateDesc, &_samplerState));
    _context->PSSetSamplers(0, 1, &_samplerState);

    ID3D11BlendState* d3dBlendState;
    D3D11_BLEND_DESC omDesc;
    ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
    omDesc.RenderTarget[0].BlendEnable = true;
    omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    D3D_VALIDATE(_device->CreateBlendState(&omDesc, &d3dBlendState));
    _context->OMSetBlendState(d3dBlendState, 0, 0xffffffff);

#ifdef _DEBUG
    _device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&_debug));
#endif

	_shaderManager.init(*this);
	const string shaderDir = params().assetDir + "shaders/";
	_shaderManager.registerShader(shaderDir + "quad.hlsl", "quad");
	
	_quadShader = &_shaderManager.getShaders("quad");

	TriMeshf quad;
	quad.indices.push_back(vec3ui(0, 1, 2));
	quad.indices.push_back(vec3ui(0, 2, 3));
	quad.vertices.push_back(TriMeshf::Vertex(vec3f(0.0f, 0.0f, 0.0f), vec2f(0.0f, 0.0f)));
	quad.vertices.push_back(TriMeshf::Vertex(vec3f(1.0f, 0.0f, 0.0f), vec2f(1.0f, 0.0f)));
	quad.vertices.push_back(TriMeshf::Vertex(vec3f(1.0f, 1.0f, 0.0f), vec2f(1.0f, 1.0f)));
	quad.vertices.push_back(TriMeshf::Vertex(vec3f(0.0f, 1.0f, 0.0f), vec2f(0.0f, 1.0f)));
	_quadMesh.load(*this, quad);

	_constantBuffer.init(*this);

    _quadToNDC = mat4f::translation(-1.0f, -1.0f, 0.0f) * mat4f::scale(2.0f);
}

void RendererD3D11::createViews() {

    //
    // Create a render target view
    //
    ID3D11Texture2D* backBuffer = nullptr;
    D3D_VALIDATE(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer));

    D3D_VALIDATE(_device->CreateRenderTargetView(backBuffer, nullptr, &_renderTargetView));
    backBuffer->Release();

    //
    // Create the depth buffer
    //
    D3D11_TEXTURE2D_DESC depthDesc;
    depthDesc.Width = _width;
    depthDesc.Height = _height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;
    D3D_VALIDATE(_device->CreateTexture2D(&depthDesc, nullptr, &_depthBuffer));

    //
    // Setup the depth stencil view
    //
    D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
    depthViewDesc.Flags = 0;
    depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil stencil view
    D3D_VALIDATE(_device->CreateDepthStencilView(_depthBuffer, &depthViewDesc, &_depthStencilView));
    _context->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

    //
    // Setup the viewport
    //
    D3D11_VIEWPORT viewport;
    viewport.Width = (FLOAT)_width;
    viewport.Height = (FLOAT)_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    _context->RSSetViewports(1, &viewport);
}

void RendererD3D11::updateWindowSize()
{
    _windowSize = getWindowSize();
    _windowToNDC = mat4f::translation(-1.0f, 1.0f, 0.0f) * mat4f::scale(2.0f / _windowSize.x, -2.0f / _windowSize.y, 1.0f);
    //glViewport(0, 0, (int)_windowSize.x, (int)_windowSize.y);
}

mat4f RendererD3D11::makeWindowTransform(const rect2f &rect, float depth)
{
    const mat4f scaleA = mat4f::scale(rect.extentX(), rect.extentY(), 1.0f);
    const mat4f translate = mat4f::translation(rect.min().x, rect.min().y, depth);
    return _windowToNDC * translate * scaleA;
}

mat4f RendererD3D11::makeWindowTransform(const rect2f &rect, float depth, float rotation)
{
    const mat4f scaleA = mat4f::scale(rect.extentX(), rect.extentY(), 1.0f);
    const mat4f translateA = mat4f::translation(rect.min().x, rect.min().y, depth);

    const mat4f translateB = mat4f::translation(vec3f(-rect.center(), 0.0f));
    const mat4f rotate = mat4f::rotationZ(rotation);
    const mat4f translateC = mat4f::translation(vec3f(rect.center(), 0.0f));

    return _windowToNDC * translateC * rotate * translateB * translateA * scaleA;
}

void RendererD3D11::bindMainRenderTarget()
{
    /*if (_motionBlurFramesLeft > 0)
    {
        if (_motionBlurRenderTargetA.dimensions() != vec2i(_windowSize))
        {
            _motionBlurRenderTargetA.init(*this, vec2i(_windowSize));
            _motionBlurRenderTargetB.init(*this, vec2i(_windowSize));
        }

        _motionBlurRenderTargetA.bindAsRenderTarget();
    }
    else
    {
        checkGLError();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        checkGLError();
    }*/
}

void RendererD3D11::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color)
{
	tex.bindD3D11();
	_quadShader->ps.bind();
	_quadShader->vs.bind();

	BasicConstantBuffer constants;
	constants.worldViewProj = makeWindowTransform(destinationRect, depth);
	//constants.worldViewProj = _quadToNDC;
	constants.modelColor = color;
	_constantBuffer.updateAndBind(constants, 0);
	_quadMesh.render();
}

void RendererD3D11::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color)
{
	tex.bindD3D11();
	_quadShader->ps.bind();
	_quadShader->vs.bind();

	BasicConstantBuffer constants;
	constants.worldViewProj = makeWindowTransform(destinationRect, depth, rotation);
	constants.modelColor = color;
	_constantBuffer.updateAndBind(constants, 0);
	_quadMesh.render();
}

void RendererD3D11::renderMotionBlur(const vec4f &color)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _motionProgram.bind();

    _motionProgram.setTransform(_quadToNDC);
    _motionProgram.setColor(color);
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::renderFullScreen(Texture &tex, const vec4f &color)
{
	tex.bindD3D11();
	_quadShader->ps.bind();
	_quadShader->vs.bind();

	BasicConstantBuffer constants;
	constants.worldViewProj = _quadToNDC;
	constants.modelColor = color;
	_constantBuffer.updateAndBind(constants, 0);
	_quadMesh.render();
}

void RendererD3D11::renderFullScreen(const vec4f &color)
{
	_quadShader->ps.bind();
	_quadShader->vs.bind();
	BasicConstantBuffer constants;
	constants.worldViewProj = _quadToNDC;
	constants.modelColor = color;
	_constantBuffer.updateAndBind(constants, 0);
	_quadMesh.render();
}

void RendererD3D11::renderGaussian(const vec2f &kernelOffset)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _gaussianProgram.bind();

    _gaussianProgram.setTransform(_quadToNDC);
    _gaussianProgram.setColor(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    _gaussianProgram.setKernelOffset(kernelOffset);
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::renderSplashA(const vec3f &focusColorA, const vec3f &focusColorB, const vec2f &pulse)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    checkGLError();

    _splashAProgram.bind();

    _splashAProgram.setTransform(_quadToNDC);
    _splashAProgram.setVec3("focusColorA", focusColorA);
    _splashAProgram.setVec3("focusColorB", focusColorB);
    _splashAProgram.setVec2("pulse", pulse);
    
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::renderSplashB(const vec2f &kernelOffset)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _splashBProgram.bind();

    _splashBProgram.setTransform(_quadToNDC);
    _splashBProgram.setColor(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    _splashBProgram.setKernelOffset(kernelOffset);
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::present()
{
	bindMainRenderTarget();

    /*
	if (_motionBlurFramesLeft > 0)
    {
        _motionBlurRenderTargetA.unbindRenderTarget();
        _motionBlurRenderTargetA.bindAsTexture();

        _motionBlurRenderTargetB.bindAsRenderTarget();

        float alpha = _motionBlurMinAlpha;

        if (_firstMotionBlurFrame)
        {
            vec2i w = getWindowSize();
            glBlitFramebuffer(0, 0, w.x, w.y, 0, 0, w.x, w.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            _firstMotionBlurFrame = false;
            _motionBlurRenderTargetB.unbindRenderTarget();
        }
        else
        {
            if (_motionBlurFramesLeft < 50)
                alpha = math::linearMap(0.0f, 50.0f, 1.0f, _motionBlurMinAlpha, (float)_motionBlurFramesLeft);

            renderMotionBlur(vec4f(1.0f, 1.0f, 1.0f, alpha));

            _motionBlurRenderTargetB.unbindRenderTarget();

            _motionBlurRenderTargetB.bindAsTexture();

            glDisable(GL_BLEND);
            renderFullScreen(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
            glEnable(GL_BLEND);

            _motionBlurFramesLeft--;
        }
    }*/

	_swapChain->Present(1, 0);
    updateWindowSize();

    bindMainRenderTarget();
}

vec2i RendererD3D11::getWindowSize()
{
    vec2i result;
    SDL_GetWindowSize(_window, &result.x, &result.y);
	return result;
}

CoordinateFrame RendererD3D11::getWindowCoordinateFrame()
{
    const vec2f windowSize = getWindowSize();

    float height = (float)windowSize.x;
    float width = (float)windowSize.y;

	vec2i canonical = GameUtil::getCanonicalSize();

	float aspectRatio = ((float) canonical.y) / (float) canonical.x;

	vec2f start;
	vec2f end;

	if (width > aspectRatio * height)
	{
		start = vec2f(0.0f,(width - height*aspectRatio) / 2.0f);
		end = vec2f(height, (width + height*aspectRatio) / 2.0f);
	}
	else
	{
		start = vec2f((height - width/aspectRatio) / 2.0f,0.0f);
		end = vec2f((height + width / aspectRatio) / 2.0f, width);
	}

    return CoordinateFrame(start, end, vec2f(canonical));
}

void RendererD3D11::clear()
{
	vec4f clearColor(0.0f, 0.0f, 0.0f, 1.0f);
	_context->ClearRenderTargetView(_renderTargetView, clearColor.array);
	_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    updateWindowSize();
}

#endif