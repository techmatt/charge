
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
}