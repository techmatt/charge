
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

	void init(RendererD3D11 &g);

	//! loads a vertex/pixel shader pair
	void registerShader(
		const string &filename,
		const string &shaderName,
		const string& entryPointVS = "vertexShaderMain",
		const string& shaderModelVS = "vs_4_0",
		const string& entryPointPS = "pixelShaderMain",
		const string& shaderModelPS = "ps_4_0");

	D3D11ShaderPair& getShaders(const string &shaderName)
	{
		MLIB_ASSERT_STR(_shaders.count(shaderName) > 0, "shader not found in shader manager");
		return _shaders.find(shaderName)->second;
	}

    const D3D11ShaderPair& getShaders(const string &shaderName) const
    {
        MLIB_ASSERT_STR(_shaders.count(shaderName) > 0, "shader not found in shader manager");
        return _shaders.find(shaderName)->second;
    }

	void bindShaders(const string &shaderName) const
	{
		auto &shaders = getShaders(shaderName);
		shaders.ps.bind();
		shaders.vs.bind();
	}

private:
	RendererD3D11 *_graphics;
	map<string, D3D11ShaderPair> _shaders;
};
