
//
// This should be a renderer, but I don't feel like keeping the renderer interface up-to-date until we need to abstract.
//
class RendererSDL : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type()
    {
        return RendererType::SDL;
    }

    SDL_Renderer* SDL()
    {
        return _renderer;
    }

    void clear();

	void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color);
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color);
	void present();
    void setRenderTarget(Texture &target);
    void setDefaultRenderTarget();

    vec2i getWindowSize();

	CoordinateFrame getWindowCoordinateFrame();	// this is the coordinate frame of the rendered area in the entire window.

    //
    // empty functions
    //
    void renderSplashA(const vec3f &focusColorA, const vec3f &focusColorB, const vec2f &pulse) {}
    void renderSplashB(const vec2f &kernelOffset) {}

    void renderFullScreen(Texture &tex, const vec4f &color);
    void renderFullScreen(const vec4f &color);
    void renderGaussian(const vec2f &kernelOffset) {}

    void renderMotionBlur(const vec4f &color) {}

    // binds the motion blur render target (if active) or the window's render target
    void bindMainRenderTarget();

    void initMotionBlur(float minAlpha, int frameCount) {}
    
    float _motionBlurMinAlpha;
    int _motionBlurFramesLeft;
    bool _firstMotionBlurFrame;

private:
    SDL_Renderer *_renderer;
    SDL_Window *_window;
};
