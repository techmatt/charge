
class D3D11Texture2D
{
public:
    D3D11Texture2D()
	{
        _graphics = nullptr;
        _texture = nullptr;
        _view = nullptr;
	}
    //
    // TODO: implement other copy constructors similar to D3D11Mesh
    //
    D3D11Texture2D(D3D11Texture2D &&t)
    {
        _image = move(t._image);
        _graphics = t._graphics;
        _view = t._view; t._view = nullptr;
        _texture = t._texture; t._texture = nullptr;
    }

    void operator = (D3D11Texture2D &&t)
    {
        _image = move(t._image);
        _graphics = t._graphics;
        _view = t._view; t._view = nullptr;
        _texture = t._texture; t._texture = nullptr;
    }

    ~D3D11Texture2D()
	{
		release();
	}

	D3D11Texture2D(RendererD3D11 &g, const Bitmap &bmp)
    {
        _texture = nullptr;
        _view = nullptr;
        load(g, bmp);
    }
    void load(RendererD3D11 &g, const Bitmap &image);

	void release();
	void reset();

    void bind() const;

	const Bitmap& getImage() const
    {
        return _image;
    }

private:
	RendererD3D11 *_graphics;
	Bitmap _image;
    ID3D11Texture2D *_texture;
    ID3D11ShaderResourceView *_view;
};
