
#include "main.h"

void GameButton::render(RendererSDL &renderer, bool selected)
{
    Texture &borderTex = database().getTexture(renderer, "Border");
    Texture &baseTex = database().getTexture(renderer, "WireBase");

    Texture &componentTex = database().getTexture(renderer, name, chargePrimary, chargeSecondary);
    
    const rect2f screenRect = GameUtil::canonicalToWindow(renderer.getWindowSize(), canonicalRect);
    renderer.render(borderTex, screenRect);
    renderer.render(baseTex, screenRect);
    renderer.render(componentTex, screenRect);

    if (selected)
    {
        Texture &selectionTex = database().getTexture(renderer, "Selector");
        renderer.render(selectionTex, screenRect);
    }
}