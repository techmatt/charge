
#include "main.h"

void RendererD3D11::init(SDL_Window *window)
{
    _window = window;

    _motionBlurFramesLeft = 0;
    _motionBlurMinAlpha = 1.0f;

    const string shaderDir = params().assetDir + "shaders/";

    //_gaussianProgram.load(shaderDir + "gaussian.vert", shaderDir + "gaussian.frag");
    //_splashAProgram.load(shaderDir + "splashA.vert", shaderDir + "splashA.frag");
    //_splashBProgram.load(shaderDir + "splashB.vert", shaderDir + "splashB.frag");
    //_motionProgram.load(shaderDir + "motion.vert", shaderDir + "motion.frag");
    //_quadProgram.load(shaderDir + "quad.vert", shaderDir + "quad.frag");

    _quadToNDC = mat4f::translation(-1.0f, -1.0f, 0.0f) * mat4f::scale(2.0f);
}

void RendererD3D11::updateWindowSize()
{
    _windowSize = getWindowSize();
    _windowToNDC = mat4f::translation(-1.0f, 1.0f, 0.0f) * mat4f::scale(2.0f / _windowSize.x, -2.0f / _windowSize.y, 1.0f);
    //glViewport(0, 0, (int)_windowSize.x, (int)_windowSize.y);
}

mat4f RendererD3D11::makeWindowTransform(const rect2f &rect, float depth)
{
    const mat4f scaleA = mat4f::scale(rect.extentX(), rect.extentY(), 1.0f);
    const mat4f translate = mat4f::translation(rect.min().x, rect.min().y, depth);
    return _windowToNDC * translate * scaleA;
}

mat4f RendererD3D11::makeWindowTransform(const rect2f &rect, float depth, float rotation)
{
    const mat4f scaleA = mat4f::scale(rect.extentX(), rect.extentY(), 1.0f);
    const mat4f translateA = mat4f::translation(rect.min().x, rect.min().y, depth);

    const mat4f translateB = mat4f::translation(vec3f(-rect.center(), 0.0f));
    const mat4f rotate = mat4f::rotationZ(rotation);
    const mat4f translateC = mat4f::translation(vec3f(rect.center(), 0.0f));

    return _windowToNDC * translateC * rotate * translateB * translateA * scaleA;
}

void RendererD3D11::bindMainRenderTarget()
{
    /*if (_motionBlurFramesLeft > 0)
    {
        if (_motionBlurRenderTargetA.dimensions() != vec2i(_windowSize))
        {
            _motionBlurRenderTargetA.init(*this, vec2i(_windowSize));
            _motionBlurRenderTargetB.init(*this, vec2i(_windowSize));
        }

        _motionBlurRenderTargetA.bindAsRenderTarget();
    }
    else
    {
        checkGLError();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        checkGLError();
    }*/
}

void RendererD3D11::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color)
{
    /*SDL_Rect dst;
    dst.x = (int)(destinationRect.min().x);
    dst.y = (int)(destinationRect.min().y);
    dst.w = (int)(destinationRect.max().x) - dst.x;
    dst.h = (int)(destinationRect.max().y) - dst.y;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    tex.bindOpenGL();
    _quadProgram.setTransform(makeWindowTransform(destinationRect, depth));
    _quadProgram.setColor(color);
    _quad.render();
    glBindTexture(GL_TEXTURE_2D, 0);*/
}

void RendererD3D11::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color)
{
    /*SDL_Rect dst;
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
    glBindTexture(GL_TEXTURE_2D, 0);*/
}

void RendererD3D11::renderMotionBlur(const vec4f &color)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _motionProgram.bind();

    _motionProgram.setTransform(_quadToNDC);
    _motionProgram.setColor(color);
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::renderFullScreen(Texture &tex, const vec4f &color)
{
    /*tex.bindOpenGL();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    checkGLError();

    _quadProgram.setTransform(_quadToNDC);
    checkGLError();
    _quadProgram.setColor(color);
    checkGLError();
    _quad.render();
    checkGLError();*/
}

void RendererD3D11::renderFullScreen(const vec4f &color)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    _quadProgram.setTransform(_quadToNDC);
    _quadProgram.setColor(color);
    _quad.render();*/
}

void RendererD3D11::renderGaussian(const vec2f &kernelOffset)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _gaussianProgram.bind();

    _gaussianProgram.setTransform(_quadToNDC);
    _gaussianProgram.setColor(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    _gaussianProgram.setKernelOffset(kernelOffset);
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::renderSplashA(const vec3f &focusColorA, const vec3f &focusColorB, const vec2f &pulse)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    checkGLError();

    _splashAProgram.bind();

    _splashAProgram.setTransform(_quadToNDC);
    _splashAProgram.setVec3("focusColorA", focusColorA);
    _splashAProgram.setVec3("focusColorB", focusColorB);
    _splashAProgram.setVec2("pulse", pulse);
    
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::renderSplashB(const vec2f &kernelOffset)
{
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _splashBProgram.bind();

    _splashBProgram.setTransform(_quadToNDC);
    _splashBProgram.setColor(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    _splashBProgram.setKernelOffset(kernelOffset);
    _quad.render();

    _quadProgram.bind();*/
}

void RendererD3D11::present()
{
    /*bindMainRenderTarget();

    if (_motionBlurFramesLeft > 0)
    {
        _motionBlurRenderTargetA.unbindRenderTarget();
        _motionBlurRenderTargetA.bindAsTexture();

        _motionBlurRenderTargetB.bindAsRenderTarget();

        float alpha = _motionBlurMinAlpha;

        if (_firstMotionBlurFrame)
        {
            vec2i w = getWindowSize();
            glBlitFramebuffer(0, 0, w.x, w.y, 0, 0, w.x, w.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            _firstMotionBlurFrame = false;
            _motionBlurRenderTargetB.unbindRenderTarget();
        }
        else
        {
            if (_motionBlurFramesLeft < 50)
                alpha = math::linearMap(0.0f, 50.0f, 1.0f, _motionBlurMinAlpha, (float)_motionBlurFramesLeft);

            renderMotionBlur(vec4f(1.0f, 1.0f, 1.0f, alpha));

            _motionBlurRenderTargetB.unbindRenderTarget();

            _motionBlurRenderTargetB.bindAsTexture();

            glDisable(GL_BLEND);
            renderFullScreen(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
            glEnable(GL_BLEND);

            _motionBlurFramesLeft--;
        }
    }

	SDL_GL_SwapWindow(_window);
    updateWindowSize();

    bindMainRenderTarget();*/
}

vec2i RendererD3D11::getWindowSize()
{
    vec2i result;
    SDL_GetWindowSize(_window, &result.x, &result.y);
	return result;
}

CoordinateFrame RendererD3D11::getWindowCoordinateFrame()
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

void RendererD3D11::clear()
{
    /*glClearColor(0.0, 0.0, 0.0, 1.0);
    checkGLError();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkGLError();

    updateWindowSize();*/
}
