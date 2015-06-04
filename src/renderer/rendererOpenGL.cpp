
#include "main.h"

void RendererOpenGL::init(SDL_Window *window)
{
    _window = window;

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    _context = SDL_GL_CreateContext(window);
    if (_context == nullptr)
    {
        cout << "SDL_GL_CreateContext error: " << SDL_GetError() << endl;
        SDL_Quit();
    }

    GLenum err = glewInit();
    MLIB_ASSERT_STR(err == GLEW_OK, "glewInit failed");

    int VAOSupported = glewIsSupported("GL_ARB_vertex_array_object");
    MLIB_ASSERT_STR(VAOSupported != 0, "GL_ARB_vertex_array_object not supported");

    const string shaderDir = params().assetDir + "shaders/";

    _gaussianProgram.load(shaderDir + "gaussian.vert", shaderDir + "gaussian.frag");

    _quadProgram.load(shaderDir + "quad.vert", shaderDir + "quad.frag");
    _quadProgram.bind();

    _quad.init();

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBlendEquation(GL_FUNC_ADD);

    _quadToNDC = mat4f::translation(-1.0f, -1.0f, 0.0f) * mat4f::scale(2.0f);

    _font = TTF_OpenFont((params().assetDir + "fonts/arial.ttf").c_str(), 50);
    
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    //SDL_GL_SetSwapInterval(1);
    //SDL_GL_MakeCurrent(window, _context);
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

void RendererOpenGL::updateWindowSize()
{
    _windowSize = getWindowSize();
    _windowToNDC = mat4f::translation(-1.0f, 1.0f, 0.0f) * mat4f::scale(2.0f / _windowSize.x, -2.0f / _windowSize.y, 1.0f);
    glViewport(0, 0, (int)_windowSize.x, (int)_windowSize.y);
}

mat4f RendererOpenGL::makeWindowTransform(const rect2f &rect, float depth)
{
    const mat4f scaleA = mat4f::scale(rect.extentX(), rect.extentY(), 1.0f);
    const mat4f translate = mat4f::translation(rect.min().x, rect.min().y, depth);
    return _windowToNDC * translate * scaleA;
}

mat4f RendererOpenGL::makeWindowTransform(const rect2f &rect, float depth, float rotation)
{
    const mat4f scaleA = mat4f::scale(rect.extentX(), rect.extentY(), 1.0f);
    const mat4f translateA = mat4f::translation(rect.min().x, rect.min().y, depth);

    const mat4f translateB = mat4f::translation(vec3f(-rect.center(), 0.0f));
    const mat4f rotate = mat4f::rotationZ(rotation);
    const mat4f translateC = mat4f::translation(vec3f(rect.center(), 0.0f));

    return _windowToNDC * translateC * rotate * translateB * translateA * scaleA;
}

void RendererOpenGL::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color)
{
	SDL_Rect dst;
    dst.x = (int)(destinationRect.min().x);
    dst.y = (int)(destinationRect.min().y);
    dst.w = (int)(destinationRect.max().x) - dst.x;
    dst.h = (int)(destinationRect.max().y) - dst.y;

    tex.bindOpenGL();
    _quadProgram.setTransform(makeWindowTransform(destinationRect, depth));
    _quadProgram.setColor(color);
    _quad.render();
}

void RendererOpenGL::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color)
{
    SDL_Rect dst;
    dst.x = (int)(destinationRect.min().x);
    dst.y = (int)(destinationRect.min().y);
    dst.w = (int)(destinationRect.max().x) - dst.x;
    dst.h = (int)(destinationRect.max().y) - dst.y;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    tex.bindOpenGL();
    _quadProgram.setTransform(makeWindowTransform(destinationRect, depth, rotation));
    _quadProgram.setColor(color);
    _quad.render();
}

void RendererOpenGL::renderFullScreen(const vec4f &color)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    _quadProgram.setTransform(_quadToNDC);
    _quadProgram.setColor(color);
    _quad.render();
}

void RendererOpenGL::renderGaussian(const vec2f &kernelOffset)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _gaussianProgram.bind();

    _gaussianProgram.setTransform(_quadToNDC);
    _gaussianProgram.setColor(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    _gaussianProgram.setKernelOffset(kernelOffset);
    _quad.render();

    _quadProgram.bind();
}

void RendererOpenGL::present()
{
	SDL_GL_SwapWindow(_window);
    updateWindowSize();
}

void RendererOpenGL::setRenderTarget(Texture &target)
{
	//SDL_SetRenderTarget(_renderer, target.SDL());
}

void RendererOpenGL::setDefaultRenderTarget()
{
	//SDL_SetRenderTarget(_renderer, NULL);
}

vec2i RendererOpenGL::getWindowSize()
{
    vec2i result;
    SDL_GetWindowSize(_window, &result.x, &result.y);
	return result;
}

CoordinateFrame RendererOpenGL::getWindowCoordinateFrame()
{
    const vec2f windowSize = getWindowSize();

    float height = (float)windowSize.x;
    float width = (float)windowSize.y;

	vec2i canonical = GameUtil::getCanonicalSize();

	float aspectRatio = ((float) canonical.y) / (float) canonical.x;

	vec2f start;
	vec2f end;

	if (width > aspectRatio * height)
	{
		start = vec2f(0.0f,(width - height*aspectRatio) / 2.0f);
		end = vec2f(height, (width + height*aspectRatio) / 2.0f);
	}
	else
	{
		start = vec2f((height - width/aspectRatio) / 2.0f,0.0f);
		end = vec2f((height + width / aspectRatio) / 2.0f, width);
	}

    return CoordinateFrame(start, end, vec2f(canonical));
}

void RendererOpenGL::clear()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateWindowSize();
}
