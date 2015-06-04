
class Texture;

enum RendererType
{
    RendererTypeSDL,
    RendererTypeOpenGL,
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
    virtual void setRenderTarget(Texture &target) = 0;
    virtual void setDefaultRenderTarget() = 0;
    virtual void clear() = 0;

    virtual vec2i getWindowSize() = 0;

    virtual CoordinateFrame getWindowCoordinateFrame() = 0;	// this is the coordinate frame of the rendered area in the entire window.
};
