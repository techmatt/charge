
#include "main.h"

void SplashScreen::init()
{

}

void SplashScreen::render()
{
    vec2f canonicalDims = GameUtil::getCanonicalSize();
    CoordinateFrame frame = app.renderer.getWindowCoordinateFrame();

    rect2f backgroundRect(vec2f(0.0f, 0.0f), canonicalDims);
    app.renderer.render(database().getTexture(app.renderer, "splash"), frame.toContainer(backgroundRect), 0.99f, vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    // splash screen should have visual depiction of each level solved state in a grid?
}

void SplashScreen::mouseUp(Uint8 button, int x, int y, bool shift, bool ctrl)
{
    app.activeEventHandler = &app.ui;
    app.activeRenderHandler = &app.canvas;
}

void SplashScreen::keyDown(SDL_Keycode key, bool shift, bool ctrl)
{
    app.activeEventHandler = &app.ui;
    app.activeRenderHandler = &app.canvas;
}