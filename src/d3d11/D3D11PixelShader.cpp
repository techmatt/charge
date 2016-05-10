
#include "main.h"

#ifdef INCLUDE_D3D

void D3D11PixelShader::load(
	RendererD3D11 &g, 
	const string &filename, 
	const string& entryPoint, 
	const string& shaderModel)
{
    _graphics = &g;

	release();
	SAFE_RELEASE(_blob);

	_filename = filename;
	
	vector<pair<string, string>> shaderMacros;
	_blob = D3D11Utility::CompileShader(_filename, entryPoint, shaderModel, shaderMacros);
	MLIB_ASSERT_STR(_blob != nullptr, "CompileShader failed");

	reset();
}

void D3D11PixelShader::release()
{
	SAFE_RELEASE(_shader);
}

void D3D11PixelShader::reset()
{
	release();

	auto &device = _graphics->device();

	D3D_VALIDATE(device.CreatePixelShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader));
}

void D3D11PixelShader::bind() const
{
	_graphics->context().PSSetShader(_shader, nullptr, 0);
}

#endif