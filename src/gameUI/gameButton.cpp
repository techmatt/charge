
#include "main.h"

void GameButton::render(RendererSDL &renderer, bool selected)
{
    Texture &tex = database().getTexture(renderer, textureName);

    const rect2f screenRect = GameUtil::canonicalToWindow(renderer.getWindowSize(), canonicalRect);
    renderer.render(tex, screenRect);

    if (selected)
    {
        Texture &selectionTex = database().getTexture(renderer, "Selector");
        renderer.render(selectionTex, screenRect);
    }
}
