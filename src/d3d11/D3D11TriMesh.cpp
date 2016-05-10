
#include "main.h"

#ifdef INCLUDE_D3D

const D3D11_INPUT_ELEMENT_DESC D3D11TriMesh::layout[layoutElementCount] =
{
	{ "position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "texCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

void D3D11TriMesh::release()
{
	SAFE_RELEASE(_vertexBuffer);
	SAFE_RELEASE(_indexBuffer);
}

void D3D11TriMesh::reset()
{
	release();
	initVB(*_graphics);
	initIB(*_graphics);
}

void D3D11TriMesh::initVB(RendererD3D11 &g)
{
    if (_triMesh.vertices.size() == 0) return;
	auto &device = g.device();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(TriMeshf::Vertex) * (UINT)_triMesh.vertices.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory( &data, sizeof(data) );
    data.pSysMem = &_triMesh.vertices[0];

	D3D_VALIDATE(device.CreateBuffer( &bufferDesc, &data, &_vertexBuffer ));
}

void D3D11TriMesh::initIB(RendererD3D11 &g)
{
    auto &indices = _triMesh.indices;
    if (indices.size() == 0) return;
	auto &device = g.device();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof( vec3ui ) * (UINT)indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory( &data, sizeof(data) );
    data.pSysMem = &indices[0];

	D3D_VALIDATE(device.CreateBuffer( &bufferDesc, &data, &_indexBuffer ));
}

void D3D11TriMesh::render() const
{
    if (_triMesh.indices.size() == 0) return;
	auto &context = _graphics->context();

	context.IASetIndexBuffer( _indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

    UINT stride = sizeof(TriMeshf::Vertex);
	UINT offset = 0;
	context.IASetVertexBuffers( 0, 1, &_vertexBuffer, &stride, &offset );

	context.IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    context.DrawIndexed((UINT)_triMesh.indices.size() * 3, 0, 0);
}

#endif