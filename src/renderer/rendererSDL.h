
class RendererSDL : public Renderer
{
public:
    void init(SDL_Window *window);

    SDL_Renderer* SDL()
    {
        return _renderer;
    }

    void render(Texture &tex, int x, int y);
    void present();
    void setRenderTarget(Texture &target);
    void setDefaultRenderTarget();

private:
    SDL_Renderer *_renderer;
};
