
#include "main.h"

void D3D11VertexShader::load(
	RendererD3D11 &g,
	const string &filename,
	const string& entryPoint,
	const string& shaderModel,
	const vector<pair<string, string>>& shaderMacros)
{
	_graphics = &g;
	if (!util::fileExists(filename))
	{
		cout << "file not found: " << filename << endl;
		return;
	}
	release();
	SAFE_RELEASE(_blob);

	_filename = filename;

	_blob = D3D11Utility::CompileShader(_filename, entryPoint, shaderModel, shaderMacros);
	MLIB_ASSERT_STR(_blob != nullptr, "CompileShader failed");

	reset();
}

void D3D11VertexShader::release()
{
	SAFE_RELEASE(_shader);
	SAFE_RELEASE(_standardLayout);
}

void D3D11VertexShader::reset()
{
	release();

	auto &device = _graphics->device();

	D3D_VALIDATE(device.CreateVertexShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader));

	device.CreateInputLayout(D3D11TriMesh::layout, D3D11TriMesh::layoutElementCount, _blob->GetBufferPointer(), _blob->GetBufferSize(), &_standardLayout);
}

void D3D11VertexShader::bind() const
{
	auto &context = _graphics->context();
	context.VSSetShader(_shader, nullptr, 0);
	context.IASetInputLayout(_standardLayout);
}