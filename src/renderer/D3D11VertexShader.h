
class D3D11VertexShader
{
public:
	D3D11VertexShader()
	{
		_shader = nullptr;
		_blob = nullptr;
		_standardLayout = nullptr;
		_graphics = nullptr;
	}
	~D3D11VertexShader()
	{
		SAFE_RELEASE(_shader);
		SAFE_RELEASE(_blob);
		SAFE_RELEASE(_standardLayout);
	}
	void load(
		RendererD3D11 &g,
		const std::string &filename, 
		const std::string& entryPoint = "vertexShaderMain", 
		const std::string& shaderModel = "vs_4_0",
		const std::vector<std::pair<std::string, std::string>>& shaderMacros = std::vector<std::pair<std::string, std::string>>());

	void release();
	void reset();

	void bind() const;

private:
	RendererD3D11 *_graphics;
	ID3D11VertexShader *_shader;
	ID3D10Blob *_blob;
	std::string _filename;

	ID3D11InputLayout *_standardLayout;
};
