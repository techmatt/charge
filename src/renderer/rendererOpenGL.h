
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

	void render(Texture &tex, const rect2f &destinationRect);
    void render(Texture &tex, const rect2f &destinationRect, float angle);

    void clear();
	void present();

    void setRenderTarget(Texture &target);
    void setDefaultRenderTarget();
    
    vec2f getWindowSize();

	CoordinateFrame getWindowCoordinateFrame();	// this is the coordinate frame of the rendered area in the entire window.

private:
    void updateWindowSize();

    mat4f makeWindowTransform(const rect2f &rect);
    mat4f makeWindowTransform(const rect2f &rect, float rotation);

    SDL_Window *_window;
    SDL_GLContext _context;

    GLQuad _quad;
    GLProgram _quadProgram;

    vec2f _windowSize;
    mat4f _windowToNDC;
};
