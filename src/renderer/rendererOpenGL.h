
//
// This should be a renderer, but I don't feel like keeping the renderer interface up-to-date until we need to abstract.
//
class RendererOpenGL : public Renderer
{
public:
    void init(SDL_Window *window);

    RendererType type() override
    {
        return RendererType::OpenGL;
    }

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color) override;
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color) override;

    void renderSplashA(const vec3f &focusColorA, const vec3f &focusColorB, float pulse);
    void renderSplashB(const vec2f &kernelOffset);

    void renderFullScreen(const vec4f &color);
    void renderGaussian(const vec2f &kernelOffset);

    void clear() override;
    void present() override;

    void setRenderTarget(Texture &target) override;
    void setDefaultRenderTarget() override;
    
    vec2i getWindowSize() override;

	CoordinateFrame getWindowCoordinateFrame();	// this is the coordinate frame of the rendered area in the entire window.

    TTF_Font* getFont(const string &fontName);

private:
    void updateWindowSize();

    mat4f makeWindowTransform(const rect2f &rect, float depth);
    mat4f makeWindowTransform(const rect2f &rect, float depth, float rotation);

    SDL_Window *_window;
    SDL_GLContext _context;
    
    //
    // we could abstract fonts into its own class, if needed
    //
    map<string, TTF_Font*> _fonts;

    GLQuad _quad;
    GLProgram _quadProgram;
    GLProgram _gaussianProgram;
    GLProgram _splashAProgram;
    GLProgram _splashBProgram;

    vec2f _windowSize;
    mat4f _windowToNDC;
    mat4f _quadToNDC;
};
