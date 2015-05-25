
//
// This should be a renderer, but I don't feel like keeping the renderer interface up-to-date until we need to abstract.
//
class RendererOpenGL : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type()
    {
        return RendererTypeOpenGL;
    }

    void render(Texture &tex, int x, int y);
	void render(Texture &tex, const rect2i &destinationRect, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2i &destinationRect, float angle, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, float angle, const CoordinateFrame &frame);

    void clear();
	void present();

    void setRenderTarget(Texture &target);
    void setDefaultRenderTarget();
    

    vec2f getWindowSize();

	CoordinateFrame getWindowCoordinateFrame();	// this is the coordinate frame of the rendered area in the entire window.

private:
    SDL_Window *_window;
    SDL_GLContext _context;
};
