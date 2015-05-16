
class GameUI
{
public:
    void init(RendererSDL &r);
    void updateBackground();
    void render();

private:
    Texture backgroundRenderTarget;

    Texture wallpaper;
    
};