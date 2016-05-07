
#include "main.h"

void D3D11Texture2D::load(RendererD3D11 &g, const Bitmap &image)
{
    _graphics = &g;
    release();
    
    _image = image;

    reset();
}

void D3D11Texture2D::release()
{
    SAFE_RELEASE(_texture);
    SAFE_RELEASE(_view);
}

void D3D11Texture2D::reset()
{
    release();

	if (_image.dimX() == 0 || _image.dimY() == 0)
        return;

    auto &device = _graphics->device();
	auto &context = _graphics->context();

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = (UINT)_image.dimX();
    desc.Height = (UINT)_image.dimY();
    desc.MipLevels = 0;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    D3D_VALIDATE(device.CreateTexture2D(&desc, nullptr, &_texture));
    D3D_VALIDATE(device.CreateShaderResourceView(_texture, nullptr, &_view));

    context.UpdateSubresource(_texture, 0, nullptr, _image.data(), (UINT)_image.dimX() * sizeof(vec4uc), (UINT)_image.dimY() * (UINT)_image.dimY() * sizeof(vec4uc));

    context.GenerateMips(_view);
}

void D3D11Texture2D::bind() const
{
    if (_view == nullptr)
        return;
    _graphics->context().PSSetShaderResources(0, 1, &_view);
}
