
#include "main.h"

void GameUI::init(RendererSDL &r)
{
    wallpaper = database().getTexture(r, "Background");
}

void GameUI::updateBackground(RendererSDL &r)
{
    r.setDefaultRenderTarget();
    vec2i dims = r.getWindowSize();
    if (vec2i((int)background.bmp().width(), (int)background.bmp().height()) != dims)
    {
        cout << "Resizing render target: " << dims << endl;
        background.loadRenderTarget(dims.x, dims.y);
    }

    r.setRenderTarget(background);

    r.render(wallpaper, rect2i(0, 0, dims.x, dims.y));
    
    r.setDefaultRenderTarget();
}

void GameUI::render(RendererSDL &r)
{
    updateBackground(r);

    r.setDefaultRenderTarget();

    r.render(background, 0, 0);
}