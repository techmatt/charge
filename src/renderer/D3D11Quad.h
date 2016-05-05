
#ifndef APPLICATION_D3D11_D3D11TRIMESH_H_
#define APPLICATION_D3D11_D3D11TRIMESH_H_

namespace ml {

class D3D11TriMesh : public GraphicsAsset
{
public:
    static const UINT layoutElementCount = 4;
    static const D3D11_INPUT_ELEMENT_DESC layout[layoutElementCount];

	D3D11TriMesh()
	{
        _graphics = nullptr;
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
	}

	template<class T>
	D3D11TriMesh(GraphicsDevice &g, const MeshData<T>& meshData)
    {
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
		load(g, meshData);
	}

	template<class T>
	D3D11TriMesh(GraphicsDevice &g, const TriMesh<T>& triMesh)
    {
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
		load(g, triMesh);
	}

	~D3D11TriMesh()
	{
		release();
	}

    void load(GraphicsDevice &g, const D3D11TriMesh& mesh)
    {
		_graphics = &g.castD3D11();
        _triMesh = mesh._triMesh;
        reset();
    }

	template<class T>
	void load(GraphicsDevice &g, const TriMesh<T>& triMesh)
    {
        _graphics = &g.castD3D11();
        _triMesh = triMesh;
		reset();
	}

	template<class T>
	void load(GraphicsDevice &g, const MeshData<T>& meshData)
    {
        load(g, TriMesh<T>(meshData));
	}

	void release();
	void reset();

	void render() const;

	//! \deprecated Backwards-compatible version of updateColors with unused GraphicsDevice& argument
	void updateColors(GraphicsDevice &g, const std::vector<vec4f> &vertexColors);

	//! Updates colors of this D3D11TriMesh to vertexColors. Precondition: vertexColors has same length as vertices otherwise exception is thrown
	void updateColors(const std::vector<vec4f> &vertexColors);

	//! computes and returns the bounding box; no caching
    bbox3f computeBoundingBox() const
    {
        return _triMesh.computeBoundingBox();
    }

    const TriMeshf& getTriMesh() const
    {
        return _triMesh;
	}

	void getMeshData(MeshDataf& meshData) const
    {
        meshData = _triMesh.getMeshData();
	}

	MeshDataf getMeshData() const {
		MeshDataf meshData;
		getMeshData(meshData);
		return meshData;
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
        _triMesh = std::move(t._triMesh);
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
        _triMesh = std::move(t._triMesh);
    }

private:
	D3D11GraphicsDevice *_graphics;
    void initVB(GraphicsDevice &g);
	void initIB(GraphicsDevice &g);

	ID3D11Buffer *_vertexBuffer;
	ID3D11Buffer *_indexBuffer;
	
    TriMeshf _triMesh;
};

}  // namespace ml

#endif  // APPLICATION_D3D11_D3D11TRIMESH_H_