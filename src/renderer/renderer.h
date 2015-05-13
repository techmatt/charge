
class Texture;

class Renderer
{
    virtual void init(SDL_Window *window) = 0;
    virtual void render(Texture &tex, int x, int y) = 0;
};
