
class D3D11PixelShader
{
public:
	D3D11PixelShader()
	{
		_shader = nullptr;
		_blob = nullptr;
		_graphics = nullptr;
	}

	~D3D11PixelShader()
	{
		release();
	}

	void load(
        RendererD3D11 &g,
		const string &filename, 
		const string &entryPoint = "pixelShaderMain", 
		const string &shaderModel = "ps_4_0");

	void release();
	void reset();

	void bind() const;

	UINT64 hash64();

private:
    RendererD3D11 *_graphics;
	ID3D11PixelShader *_shader;
	ID3DBlob *_blob;
	string _filename;
};
