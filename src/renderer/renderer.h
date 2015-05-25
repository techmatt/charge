
class Texture;

class Renderer
{
public:
    virtual void init(SDL_Window *window) = 0;

    virtual SDL_Renderer* SDL()
    {
        MLIB_ERROR("SDL renderer not supported on this renderer");
        return nullptr;
    }

    virtual void render(Texture &tex, int x, int y) = 0;
    virtual void render(Texture &tex, const rect2i &destinationRect, const CoordinateFrame &frame) = 0;
    virtual void render(Texture &tex, const rect2i &destinationRect, float angle, const CoordinateFrame &frame) = 0;
    virtual void render(Texture &tex, const rect2f &destinationRect, const CoordinateFrame &frame) = 0;
    virtual void render(Texture &tex, const rect2f &destinationRect, float angle, const CoordinateFrame &frame) = 0;
    virtual void present() = 0;
    virtual void setRenderTarget(Texture &target) = 0;
    virtual void setDefaultRenderTarget() = 0;

    virtual vec2f getWindowSize() = 0;

    virtual CoordinateFrame getWindowCoordinateFrame() = 0;	// this is the coordinate frame of the rendered area in the entire window.
};
