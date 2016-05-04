
class Texture;

enum class RendererType
{
    SDL,
    OpenGL,
    D3D11,
};

class Renderer
{
public:
    virtual void init(SDL_Window *window) = 0;

    virtual SDL_Renderer* SDL()
    {
        MLIB_ERROR("SDL not supported on this renderer");
        return nullptr;
    }

    virtual RendererType type() = 0;

    virtual void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color) = 0;
    virtual void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color) = 0;
    virtual void present() = 0;
    virtual void clear() = 0;

    virtual vec2i getWindowSize() = 0;

    virtual void bindMainRenderTarget() = 0;

    virtual void renderFullScreen(Texture &tex, const vec4f &color) = 0;
    virtual void renderFullScreen(const vec4f &color) = 0;

    virtual bool motionBlurActive() { return false; }

    virtual void initMotionBlur(float minAlpha, int frameCount) {}

    virtual void renderSplashA(const vec3f &focusColorA, const vec3f &focusColorB, const vec2f &pulse) {}
    virtual void renderSplashB(const vec2f &kernelOffset) {}
    virtual void renderGaussian(const vec2f &kernelOffset) {}
    virtual void renderMotionBlur(const vec4f &color) {}

    virtual CoordinateFrame getWindowCoordinateFrame() = 0;	// this is the coordinate frame of the rendered area in the entire window.
};
