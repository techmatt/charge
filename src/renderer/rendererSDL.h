
class RendererSDL : public Renderer
{
public:
    void init(SDL_Window *window);

    SDL_Renderer* SDL()
    {
        return _renderer;
    }

    void render(Texture &tex, int x, int y);
    void render(Texture &tex, const rect2i &destinationRect);
    void present();
    void setRenderTarget(Texture &target);
    void setDefaultRenderTarget();

    vec2i getWindowSize();

private:
    SDL_Renderer *_renderer;
    SDL_Window *_window;
};
