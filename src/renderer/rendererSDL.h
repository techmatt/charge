
//
// This should be a renderer, but I don't feel like keeping the renderer interface up-to-date until we need to abstract.
//
class RendererSDL : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type()
    {
        return RendererTypeSDL;
    }

    SDL_Renderer* SDL()
    {
        return _renderer;
    }

    void clear();

	//void render(Texture &tex, const rect2i &destinationRect, const CoordinateFrame &frame);
    //void render(Texture &tex, const rect2i &destinationRect, float angle, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, float angle, const CoordinateFrame &frame);
	void present();
    void setRenderTarget(Texture &target);
    void setDefaultRenderTarget();

    vec2f getWindowSize();

	CoordinateFrame getWindowCoordinateFrame();	// this is the coordinate frame of the rendered area in the entire window.

private:
    SDL_Renderer *_renderer;
    SDL_Window *_window;
};
