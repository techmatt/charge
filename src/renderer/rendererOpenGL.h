
//
// This should be a renderer, but I don't feel like keeping the renderer interface up-to-date until we need to abstract.
//
class RendererOpenGL : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type() override
    {
        return RendererTypeOpenGL;
    }

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color) override;
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color) override;

    void renderFullScreen(const vec4f &color);
    void renderGaussian(const vec2f &kernelOffset);

    void clear() override;
    void present() override;

    void setRenderTarget(Texture &target) override;
    void setDefaultRenderTarget() override;
    
    vec2f getWindowSize() override;

	CoordinateFrame getWindowCoordinateFrame();	// this is the coordinate frame of the rendered area in the entire window.

    TTF_Font* font()
    {
        return _font;
    }

private:
    void updateWindowSize();

    mat4f makeWindowTransform(const rect2f &rect, float depth);
    mat4f makeWindowTransform(const rect2f &rect, float depth, float rotation);

    SDL_Window *_window;
    SDL_GLContext _context;
    
    //
    // we could abstract fonts into its own class, if needed
    //
    TTF_Font *_font;

    GLQuad _quad;
    GLProgram _quadProgram;
    GLProgram _gaussianProgram;

    vec2f _windowSize;
    mat4f _windowToNDC;
    mat4f _quadToNDC;
};
