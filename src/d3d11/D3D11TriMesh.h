
struct TriMeshf
{
	struct Vertex
	{
		Vertex() {}
		Vertex(const vec3f &_position, const vec2f &_texCoord)
		{
			position = _position;
			texCoord = _texCoord;
			color = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		}
		vec3f position;
		vec4f color;
		vec2f texCoord;
	};

	vector<Vertex> vertices;
	vector<vec3ui> indices;
};

class D3D11TriMesh
{
public:
    static const UINT layoutElementCount = 3;
    static const D3D11_INPUT_ELEMENT_DESC layout[layoutElementCount];

	D3D11TriMesh()
	{
        _graphics = nullptr;
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
	}

	D3D11TriMesh(RendererD3D11 &g, const TriMeshf& triMesh)
    {
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
		load(g, triMesh);
	}

	~D3D11TriMesh()
	{
		release();
	}

    void load(RendererD3D11 &g, const D3D11TriMesh& mesh)
    {
		_graphics = &g;
        _triMesh = mesh._triMesh;
        reset();
    }

	void load(RendererD3D11 &g, const TriMeshf& triMesh)
    {
        _graphics = &g;
        _triMesh = triMesh;
		reset();
	}

	void release();
	void reset();

	void render() const;

    const TriMeshf& getTriMesh() const
    {
        return _triMesh;
	}

    D3D11TriMesh(const D3D11TriMesh &t)
    {
        _vertexBuffer = nullptr;
        _indexBuffer = nullptr;
        load(*t._graphics, t);
    }
    D3D11TriMesh(D3D11TriMesh &&t)
    {
        _graphics = t._graphics; t._graphics = nullptr;
        _vertexBuffer = t._vertexBuffer; t._vertexBuffer = nullptr;
        _indexBuffer = t._indexBuffer; t._indexBuffer = nullptr;
        _triMesh = move(t._triMesh);
    }

    void operator = (const D3D11TriMesh& t)
    {
        _vertexBuffer = nullptr;
        _indexBuffer = nullptr;
        load(*t._graphics, t);
    }

    void operator = (D3D11TriMesh&& t)
    {
        _graphics = t._graphics; t._graphics = nullptr;
        _vertexBuffer = t._vertexBuffer; t._vertexBuffer = nullptr;
        _indexBuffer = t._indexBuffer; t._indexBuffer = nullptr;
        _triMesh = move(t._triMesh);
    }

private:
	RendererD3D11 *_graphics;
    void initVB(RendererD3D11 &g);
	void initIB(RendererD3D11 &g);

	ID3D11Buffer *_vertexBuffer;
	ID3D11Buffer *_indexBuffer;

	TriMeshf _triMesh;
};
