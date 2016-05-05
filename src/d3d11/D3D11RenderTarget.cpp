
#include "main.h"

void D3D11RenderTarget::release()
{
		
	for (unsigned int i = 0; i < getNumTargets(); i++) {
		if (_targets)		SAFE_RELEASE(_targets[i]);
		if (_targetsRTV)	SAFE_RELEASE(_targetsRTV[i]);
		if (hasSRVs() && _targetsSRV)	SAFE_RELEASE(_targetsSRV[i]);
	}

	SAFE_DELETE_ARRAY(_targets);
	SAFE_DELETE_ARRAY(_targetsRTV);
	SAFE_DELETE_ARRAY(_targetsSRV);

	SAFE_RELEASE(_depthStencil);
	SAFE_RELEASE(_depthStencilDSV);
	if (hasSRVs()) SAFE_RELEASE(_depthStensilSRV);

	for (unsigned int i = 0; i < getNumTargets(); i++) {
		if (_captureTextures) SAFE_RELEASE(_captureTextures[i]);
	}
	SAFE_DELETE_ARRAY(_captureTextures);

	SAFE_RELEASE(_captureDepth);
}

void D3D11RenderTarget::reset()
{
	release();

	if (_width == 0 || _height == 0)
		return;

	auto &device = _graphics->device();
	auto &context = _graphics->context();

	//
	// Create the render target
	//
	_targets = new ID3D11Texture2D*[getNumTargets()];
	_targetsRTV = new ID3D11RenderTargetView*[getNumTargets()];
	if (hasSRVs())	_targetsSRV = new ID3D11ShaderResourceView*[getNumTargets()];
	_captureTextures = new ID3D11Texture2D*[getNumTargets()];

	for (unsigned int i = 0; i < getNumTargets(); i++) {
		D3D11_TEXTURE2D_DESC renderDesc;
		renderDesc.Width = _width;
		renderDesc.Height = _height;
		renderDesc.MipLevels = 0;
		renderDesc.ArraySize = 1;
		//renderDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		renderDesc.Format = _textureFormats[i];
		renderDesc.SampleDesc.Count = 1;
		renderDesc.SampleDesc.Quality = 0;
		renderDesc.Usage = D3D11_USAGE_DEFAULT;
		renderDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		renderDesc.CPUAccessFlags = 0;
		renderDesc.MiscFlags = 0;

		D3D_VALIDATE(device.CreateTexture2D(&renderDesc, nullptr, &_targets[i]));				// create the texture
		D3D_VALIDATE(device.CreateRenderTargetView(_targets[i], nullptr, &_targetsRTV[i]));	// create the render target view
		if (hasSRVs())	D3D_VALIDATE(device.CreateShaderResourceView(_targets[i], nullptr, &_targetsSRV[i]));	// create the shader resource view


		// Create the color capture buffer
		renderDesc.BindFlags = 0;
		renderDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		renderDesc.Usage = D3D11_USAGE_STAGING;
		D3D_VALIDATE(device.CreateTexture2D(&renderDesc, nullptr, &_captureTextures[i]));
	}

	// Create the depth buffer
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = _width;
	depthDesc.Height = _height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;	//we just assume the depth buffer to always have 32 bit
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	D3D_VALIDATE(device.CreateTexture2D(&depthDesc, nullptr, &_depthStencil));

	// Create the depth view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
	depthViewDesc.Flags = 0;
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0;
	D3D_VALIDATE(device.CreateDepthStencilView(_depthStencil, nullptr, &_depthStencilDSV));


	// Create the depth capture buffer
	depthDesc.BindFlags = 0;
	depthDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	depthDesc.Usage = D3D11_USAGE_STAGING;
	D3D_VALIDATE(device.CreateTexture2D(&depthDesc, nullptr, &_captureDepth));
}

void D3D11RenderTarget::bind()
{
	MLIB_ASSERT(_targets != nullptr);


	auto &context = _graphics->context();
	context.OMSetRenderTargets(getNumTargets(), _targetsRTV, _depthStencilDSV);

	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)_width;
	viewport.Height = (FLOAT)_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	context.RSSetViewports(1, &viewport);
}

void D3D11RenderTarget::clear(const vec4f& clearColor, float clearDepth)
{
	auto &context = _graphics->context();
	for (unsigned int i = 0; i < getNumTargets(); i++) {
		context.ClearRenderTargetView(_targetsRTV[i], clearColor.array);
	}
	context.ClearDepthStencilView(_depthStencilDSV, D3D11_CLEAR_DEPTH, clearDepth, 0);
}

void D3D11RenderTarget::clearColor(const vec4f& clearColor)
{
	auto &context = _graphics->context();
	for (unsigned int i = 0; i < getNumTargets(); i++) {
		context.ClearRenderTargetView(_targetsRTV[i], clearColor.array);
	}
}

void D3D11RenderTarget::clearDepth(float clearDepth)
{
	auto &context = _graphics->context();
	context.ClearDepthStencilView(_depthStencilDSV, D3D11_CLEAR_DEPTH, clearDepth, 0);
}
