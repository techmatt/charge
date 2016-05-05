
void ml::D3D11VertexShader::load(
	GraphicsDevice &g,
	const std::string &filename,
	const std::string& entryPoint,
	const std::string& shaderModel,
	const std::vector<std::pair<std::string, std::string>>& shaderMacros)
{
	_graphics = &g.castD3D11();
	if (!util::fileExists(filename))
	{
		std::cout << "file not found: " << filename << std::endl;
		return;
	}
	release();
	SAFE_RELEASE(_blob);

	_filename = filename;
	g.castD3D11().registerAsset(this);

	_blob = D3D11Utility::CompileShader(_filename, entryPoint, shaderModel, shaderMacros);
	MLIB_ASSERT_STR(_blob != nullptr, "CompileShader failed");

	reset();
}

void ml::D3D11VertexShader::release()
{
	SAFE_RELEASE(_shader);
	SAFE_RELEASE(_standardLayout);
}

void ml::D3D11VertexShader::reset()
{
	release();

	auto &device = _graphics->getDevice();

	D3D_VALIDATE(device.CreateVertexShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader));

	device.CreateInputLayout(D3D11TriMesh::layout, D3D11TriMesh::layoutElementCount, _blob->GetBufferPointer(), _blob->GetBufferSize(), &_standardLayout);
}

void ml::D3D11VertexShader::bind() const
{
	auto &context = _graphics->getContext();
	context.VSSetShader(_shader, nullptr, 0);
	context.IASetInputLayout(_standardLayout);
}