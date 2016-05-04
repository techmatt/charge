
#include "main.h"

void Texture::drawText(Renderer &renderer, TTF_Font *font, const string &text, RGBColor color, int wrapWidth, float glowStrength, RGBColor glowColor)
{
    if (text.size() == 0)
    {
        return;
    }

    _renderer = &renderer;

    const SDL_Color SDLColor = { color.r, color.g, color.b, color.a };
    
    SDL_Surface *surface;
    if (wrapWidth == 0)
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), SDLColor);
    else
        surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), SDLColor, wrapWidth);
        

    _bmp.allocate(surface->w, surface->h);
    for (int y = 0; y < surface->h; y++)
        memcpy(&_bmp(0, y), (BYTE *)surface->pixels + y * surface->pitch, sizeof(RGBColor) * _bmp.width());

    for (const auto &p : _bmp)
    {
        const BYTE b = p.value.b;
        p.value.b = p.value.r;
        p.value.r = b;
    }

    if (glowStrength > 0.0f)
    {
        _bmp = GameUtil::processGlow(_bmp, glowStrength, glowColor.toVec3f(), color.toVec3f());
    }

    if (renderer.type() == RendererType::SDL)
        initSDL();
    else
        initOpenGL(false);

    SDL_FreeSurface(surface);
}

void Texture::load(Renderer &renderer, const string &filename)
{
    _renderer = &renderer;
    _bmp = LodePNG::load(filename);

    if (renderer.type() == RendererType::SDL)
        initSDL();

    if (renderer.type() == RendererType::OpenGL)
        initOpenGL(true);
}

void Texture::load(Renderer &renderer, const Bitmap &bmp)
{
    _renderer = &renderer;
	_bmp = bmp;

    if (renderer.type() == RendererType::SDL)
        initSDL();

    if (renderer.type() == RendererType::OpenGL)
        initOpenGL(true);
}

void Texture::initSDL()
{
    releaseSDLMemory();

    _SDLTexture = nullptr;
    
    //SDL_PIXELFORMAT_ARGB8888 seems to be the default texture for windows
    SDL_CreateTexture(_renderer->SDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, (int)_bmp.width(), (int)_bmp.height());

    _SDLSurface = SDL_CreateRGBSurfaceFrom(_bmp.data(), (int)_bmp.width(), (int)_bmp.height(), 32, (int)_bmp.width() * sizeof(RGBColor), 0x0000FF, 0x00FF00, 0xFF0000, 0xFF000000);
    if (_SDLSurface == nullptr)
    {
        //SDL::logError("SDL_CreateRGBSurfaceFrom");
        return;
    }

    _SDLTexture = SDL_CreateTextureFromSurface(_renderer->SDL(), _SDLSurface);

    if (_SDLTexture == nullptr)
    {
        //SDL::logError("SDL_CreateTextureFromSurface");
        return;
    }

    //SDL_FreeSurface(loadedImage);
}

void Texture::initOpenGL(bool useMipmaps)
{
	if(debugCalls) cout << "Begin initOpenGL" << endl;
    checkGLError();
	if (debugCalls) cout << "glGenTextures" << endl;
    glGenTextures(1, &_OpenGLTexture);
	checkGLError();
	if (debugCalls) cout << "glBindTexture" << endl;
    glBindTexture(GL_TEXTURE_2D, _OpenGLTexture);
    checkGLError();

    const GLsizei width = (GLsizei)_bmp.width();
    const GLsizei height = (GLsizei)_bmp.height();

	useMipmaps &= params().useMipmaps;
    if (useMipmaps)
    {
        int mipMapCount = (int)log2((double)std::min(width, height));
        mipMapCount = math::clamp(mipMapCount - 1, 1, 8);
		if (debugCalls) cout << "glTexStorage2D" << endl;
        glTexStorage2D(GL_TEXTURE_2D, mipMapCount, GL_RGBA8, width, height);
        checkGLError();

		if (debugCalls) cout << "glTexSubImage2D" << endl;
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, _bmp.data());
        checkGLError();

		if (debugCalls) cout << "glGenerateMipmap" << endl;
        glGenerateMipmap(GL_TEXTURE_2D);
        checkGLError();

		if (debugCalls) cout << "glTexParameteri" << endl;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        checkGLError();
    }
    else
    {
		if (debugCalls) cout << "glTexImage2D" << endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _bmp.data());
        checkGLError();

		if (debugCalls) cout << "glTexParameteri" << endl;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        checkGLError();
    }

	if (debugCalls) cout << "glTexParameteri" << endl;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGLError();

    checkGLError();
	if (debugCalls) cout << "glBindTexture" << endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGLError();
}

void Texture::bindOpenGL()
{
    //SDL_GL_BindTexture(_SDLTexture, nullptr, nullptr);
    glBindTexture(GL_TEXTURE_2D, _OpenGLTexture);
    checkGLError();
}