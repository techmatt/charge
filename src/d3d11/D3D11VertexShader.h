
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
		const string &filename, 
		const string& entryPoint = "vertexShaderMain", 
		const string& shaderModel = "vs_4_0",
		const vector<pair<string, string>>& shaderMacros = vector<pair<string, string>>());

	void release();
	void reset();

	void bind() const;

private:
	RendererD3D11 *_graphics;
	ID3D11VertexShader *_shader;
	ID3D10Blob *_blob;
	string _filename;

	ID3D11InputLayout *_standardLayout;
};
