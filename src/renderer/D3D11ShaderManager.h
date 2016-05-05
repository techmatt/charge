
struct D3D11ShaderPair
{
	D3D11PixelShader ps;
	D3D11VertexShader vs;
};

class D3D11ShaderManager
{
public:
	D3D11ShaderManager()
	{
		_graphics = NULL;
	}

	void init(GraphicsDevice &g);

	//! loads a vertex/pixel shader pair
	void registerShader(
		const std::string &filename,
		const std::string &shaderName,
		const std::string& entryPointVS = "vertexShaderMain",
		const std::string& shaderModelVS = "vs_4_0",
		const std::string& entryPointPS = "pixelShaderMain",
		const std::string& shaderModelPS = "ps_4_0",
		const std::vector<std::pair<std::string, std::string>>& shaderMacros = std::vector<std::pair<std::string, std::string>>());

	D3D11ShaderPair& getShaders(const std::string &shaderName)
	{
		MLIB_ASSERT_STR(_shaders.count(shaderName) > 0, "shader not found in shader manager");
		return _shaders.find(shaderName)->second;
	}

    const D3D11ShaderPair& getShaders(const std::string &shaderName) const
    {
        MLIB_ASSERT_STR(_shaders.count(shaderName) > 0, "shader not found in shader manager");
        return _shaders.find(shaderName)->second;
    }

	void bindShaders(const std::string &shaderName) const
	{
		auto &shaders = getShaders(shaderName);
		shaders.ps.bind();
		shaders.vs.bind();
	}

private:
	D3D11GraphicsDevice *_graphics;
	std::map<std::string, D3D11ShaderPair> _shaders;
};
