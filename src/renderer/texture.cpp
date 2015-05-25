
#include "main.h"

void Texture::load(RendererSDL &renderer, const string &filename)
{
    _renderTarget = false;
    _bmp = LodePNG::load(filename);
    initSDL(renderer);
    //initOpenGL();
}

void Texture::load(RendererSDL &renderer, const Bitmap &bmp)
{
	_renderTarget = false;
	_bmp = bmp;
	initSDL(renderer);
    //initOpenGL();
}

void Texture::loadRenderTarget(int width, int height)
{
    _bmp.allocate(width, height);
    _renderTarget = true;
}

void Texture::initSDL(RendererSDL &renderer)
{
    releaseSDLMemrory();

    _SDLTexture = nullptr;
    
    if (_renderTarget)
    {
        //SDL_PIXELFORMAT_ARGB8888 seems to be the default texture for windows
        SDL_CreateTexture(renderer.SDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, (int)_bmp.width(), (int)_bmp.height());
    }
    else
    {
		_SDLSurface = SDL_CreateRGBSurfaceFrom(_bmp.data(), (int)_bmp.width(), (int)_bmp.height(), 32, (int)_bmp.width() * sizeof(RGBColor), 0x0000FF, 0x00FF00, 0xFF0000, 0xFF000000);
        if (_SDLSurface == nullptr)
        {
            SDL::logError("SDL_CreateRGBSurfaceFrom");
            return;
        }

        _SDLTexture = SDL_CreateTextureFromSurface(renderer.SDL(), _SDLSurface);

        if (_SDLTexture == nullptr)
        {
            SDL::logError("SDL_CreateTextureFromSurface");
            return;
        }
    }

    //SDL_FreeSurface(loadedImage);
}

void Texture::initOpenGL()
{
    glGenTextures(1, &_OpenGLTexture);
    glBindTexture(GL_TEXTURE_2D, _OpenGLTexture);
    //Specify the texture's data.  This function is a bit tricky, and it's hard to find helpful documentation.  A summary:
    //   GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
    //               0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
    //                         not cached smaller copies)
    //         GL_RGBA:    The internal format of the texture.  This is how OpenGL will store the texture internally (kinda)--
    //                         it's essentially the texture's type.
    //      surface->w:    The width of the texture
    //      surface->h:    The height of the texture
    //               0:    The border.  Don't worry about this if you're just starting.
    //          GL_RGB:    The format that the *data* is in--NOT the texture!  Our test image doesn't have an alpha channel,
    //                         so this must be RGB.
    //GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
    //                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
    //                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
    //                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
    // surface->pixels:    The actual data.  As above, SDL's array of bytes.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _bmp.dimX(), _bmp.dimY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _bmp.data());
    //Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
    //*smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
    //each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
    //further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
    //them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bindOpenGL()
{
    //SDL_GL_BindTexture(_SDLTexture, nullptr, nullptr);
    glBindTexture(GL_TEXTURE_2D, _OpenGLTexture);
}