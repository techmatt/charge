
#include "main.h"

void RendererOpenGL::init(SDL_Window *window)
{
    _context = SDL_GL_CreateContext(window);
    if (_context == nullptr)
    {
        SDL::logError("CreateContext");
        SDL_Quit();
    }

    //SDL_GL_SetSwapInterval(1);
    //SDL_GL_MakeCurrent(window, _context);

	_window = window;

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

void RendererOpenGL::render(Texture &tex, int x, int y)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;

	//SDL_QueryTexture(tex.SDL(), NULL, NULL, &dst.w, &dst.h);

	//SDL_RenderCopy(_renderer, tex.SDL(), NULL, &dst);
}

void RendererOpenGL::render(Texture &tex, const rect2f &destinationRect, const CoordinateFrame &frame)
{
	rect2f transformedDestinationRect = frame.toContainer(destinationRect);
	SDL_Rect dst;
	dst.x = (int) (transformedDestinationRect.min().x);
	dst.y = (int) (transformedDestinationRect.min().y);
	dst.w = (int) (transformedDestinationRect.max().x)-dst.x;
	dst.h = (int) (transformedDestinationRect.max().y)-dst.y;

	//SDL_RenderCopy(_renderer, tex.SDL(), NULL, &dst);
}

void RendererOpenGL::render(Texture &tex, const rect2i &destinationRect, const CoordinateFrame &frame)
{
	//render(tex, rect2f(destinationRect.min(), destinationRect.max()),frame);
}

void RendererOpenGL::render(Texture &tex, const rect2f &destinationRect, float angle, const CoordinateFrame &frame)
{
	rect2f transformedDestinationRect = frame.toContainer(destinationRect);
	SDL_Rect dst;
	dst.x = (int)transformedDestinationRect.min().x;
	dst.y = (int)transformedDestinationRect.min().y;
	dst.w = (int)transformedDestinationRect.extentX();
	dst.h = (int)transformedDestinationRect.extentY();

	//SDL_RenderCopyEx(_renderer, tex.SDL(), NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}
void RendererOpenGL::render(Texture &tex, const rect2i &destinationRect, float angle, const CoordinateFrame &frame)
{
	render(tex, rect2f(destinationRect.min(), destinationRect.max()),angle, frame);
}

void RendererOpenGL::present()
{
	//SDL_RenderPresent(_renderer);
    //SDL_GL_SwapWindow(_window);
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