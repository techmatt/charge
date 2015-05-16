
class GameUI
{
public:
    void init(RendererSDL &r);
    void updateBackground(RendererSDL &r);
    void render(RendererSDL &r);

private:
    Texture background;

    Texture wallpaper;
};