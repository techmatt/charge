
void ml::D3D11PixelShader::load(
	GraphicsDevice &g, 
	const std::string &filename, 
	const std::string& entryPoint, 
	const std::string& shaderModel,
	const std::vector<std::pair<std::string, std::string>>& shaderMacros)
{
    _graphics = &g.castD3D11();

	release();
	SAFE_RELEASE(_blob);

	_filename = filename;
	g.castD3D11().registerAsset(this);

	_blob = D3D11Utility::CompileShader(_filename, entryPoint, shaderModel, shaderMacros);
	MLIB_ASSERT_STR(_blob != nullptr, "CompileShader failed");

	reset();
}

void ml::D3D11PixelShader::release()
{
	SAFE_RELEASE(_shader);
}

void ml::D3D11PixelShader::reset()
{
	release();

	auto &device = _graphics->getDevice();

	D3D_VALIDATE(device.CreatePixelShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader));
}

void ml::D3D11PixelShader::bind() const
{
	_graphics->getContext().PSSetShader(_shader, nullptr, 0);
}