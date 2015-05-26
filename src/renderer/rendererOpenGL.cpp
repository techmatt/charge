
#include "main.h"

void RendererOpenGL::init(SDL_Window *window)
{
    _window = window;

    _context = SDL_GL_CreateContext(window);
    if (_context == nullptr)
    {
        SDL::logError("CreateContext");
        SDL_Quit();
    }

    GLenum err = glewInit();
    MLIB_ASSERT_STR(err == GLEW_OK, "glewInit failed");

    int VAOSupported = glewIsSupported("GL_ARB_vertex_array_object");
    MLIB_ASSERT_STR(VAOSupported != 0, "GL_ARB_vertex_array_object not supported");

    _quadProgram.load("tutorial2.vert", "tutorial2.frag");
    _quadProgram.bind();

    _quad.init();

    glEnable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    glEnable(GL_BLEND);

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    //SDL_GL_SetSwapInterval(1);
    //SDL_GL_MakeCurrent(window, _context);
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

mat4f RendererOpenGL::makeWindowTransform(const rect2f &rect)
{
    const mat4f scaleA = mat4f::scale(rect.extentX(), rect.extentY(), 1.0f);
    const mat4f translate = mat4f::translation(rect.min().x, rect.min().y, 0.0f);
    const mat4f windowToNDC = mat4f::translation(-1.0f, 1.0f, 0.0f) * mat4f::scale(2.0f / _windowSize.x, -2.0f / _windowSize.y, 1.0f);
    return windowToNDC * translate * scaleA;
}

void RendererOpenGL::render(Texture &tex, const rect2f &destinationRect)
{
	SDL_Rect dst;
    dst.x = (int)(destinationRect.min().x);
    dst.y = (int)(destinationRect.min().y);
    dst.w = (int)(destinationRect.max().x) - dst.x;
    dst.h = (int)(destinationRect.max().y) - dst.y;

    tex.bindOpenGL();
    _quadProgram.setTransform(makeWindowTransform(destinationRect));
    _quad.render();
}

void RendererOpenGL::render(Texture &tex, const rect2f &destinationRect, float angle)
{
    SDL_Rect dst;
    dst.x = (int)(destinationRect.min().x);
    dst.y = (int)(destinationRect.min().y);
    dst.w = (int)(destinationRect.max().x) - dst.x;
    dst.h = (int)(destinationRect.max().y) - dst.y;

    //tex.bindOpenGL();

    /*if (rand() % 2 == 0)
        database().getTexture(*this, "Background").bindOpenGL();
    else
        database().getTexture(*this, "Circuit").bindOpenGL();

    _quad.render();*/

	//SDL_RenderCopyEx(_renderer, tex.SDL(), NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

void RendererOpenGL::present()
{
	SDL_GL_SwapWindow(_window);

    _windowSize = getWindowSize();
}

void RendererOpenGL::setRenderTarget(Texture &target)
{
	//SDL_SetRenderTarget(_renderer, target.SDL());
}

void RendererOpenGL::setDefaultRenderTarget()
{
	//SDL_SetRenderTarget(_renderer, NULL);
}

vec2f RendererOpenGL::getWindowSize()
{
    vec2i result;
    SDL_GetWindowSize(_window, &result.x, &result.y);
	return vec2f(result);
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
    glClear(GL_COLOR_BUFFER_BIT);

    _windowSize = getWindowSize();
}
