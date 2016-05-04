
template<class T>
class D3D11ConstantBuffer
{
public:
	D3D11ConstantBuffer()
	{
        _graphics = nullptr;
		_buffer = nullptr;
	}

	~D3D11ConstantBuffer()
	{
		release();
	}

	void init(RendererD3D11 &g)
	{
        _graphics = &g;
		reset();
	}

	void release()
	{
		SAFE_RELEASE(m_buffer);
	}

	void reset()
	{
		release();

		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(T);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
        D3D_VALIDATE(_graphics->device().CreateBuffer(&desc, nullptr, &_buffer));
	}

    void updateAndBind(const T &data, UINT constantBufferIndex)
    {
        update(data);
		bind(constantBufferIndex);
    }

	void update(const T &data)
	{
        _graphics->getContext().UpdateSubresource(_buffer, 0, nullptr, &data, 0, 0);
	}

	void bind(UINT constantBufferIndex) 
	{
		bindVertexShader(constantBufferIndex);
		bindPixelShader(constantBufferIndex);
	}

	void bindVertexShader(UINT constantBufferIndex)
	{
        _graphics->getContext().VSSetConstantBuffers(constantBufferIndex, 1, &_buffer);
	}

	void bindPixelShader(UINT constantBufferIndex)
	{
        _graphics->getContext().PSSetConstantBuffers(constantBufferIndex, 1, &_buffer);
	}

private:
    RendererD3D11 *_graphics;
	ID3D11Buffer *_buffer;
};
