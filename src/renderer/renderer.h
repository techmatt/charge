
class Texture;

class Renderer
{
    virtual void init(SDL_Window *window) = 0;
    virtual void render(Texture &tex, int x, int y) = 0;
    virtual void present() = 0;
    virtual void setRenderTarget(Texture &target) = 0;
    virtual void setDefaultRenderTarget() = 0;
};
