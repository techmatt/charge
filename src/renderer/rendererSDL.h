
class RendererSDL : public Renderer
{
public:
    void init(SDL_Window *window);

    SDL_Renderer* SDL()
    {
        return _renderer;
    }

    void render(Texture &tex, int x, int y);


private:
    SDL_Renderer *_renderer;
};
