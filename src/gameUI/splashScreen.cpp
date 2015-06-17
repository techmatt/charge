
#include "main.h"

// GetAsyncKeyState
#include "windows.h"

void SplashScreen::init()
{

}

void SplashScreen::render()
{
    vec2f canonicalDims = GameUtil::getCanonicalSize();
    CoordinateFrame frame = app.renderer.getWindowCoordinateFrame();

    rect2f backgroundRect(vec2f(0.0f, 0.0f), canonicalDims);
    app.renderer.render(database().getTexture(app.renderer, "splash"), frame.toContainer(backgroundRect), 0.99f, vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    bloom();

    // splash screen should have visual depiction of each level solved state in a grid?
}

void SplashScreen::bloom()
{
    const vec2i bloomSize = app.renderer.getWindowSize() / 2;
    if (bloomTexture0.dimensions() != bloomSize)
    {
        bloomTexture0.init(app.renderer, bloomSize);
        bloomTexture1.init(app.renderer, bloomSize);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    bloomTexture0.bindAsRenderTarget();

    glClear(GL_COLOR_BUFFER_BIT);
    
    database().getTexture(app.renderer, "splash").bindOpenGL();

    app.renderer.renderSplashA(vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f), 1.0f);

    bloomTexture0.unbindRenderTarget();

    //
    // blur in X to trailTexture1
    //
    bloomTexture1.bindAsRenderTarget();

    bloomTexture0.bindAsTexture();

    glClear(GL_COLOR_BUFFER_BIT);
    app.renderer.renderSplashB(vec2f(1.0f, 0.0f) / (float)bloomSize.x);

    bloomTexture1.unbindRenderTarget();

    //LodePNG::save(trailTexture1.getImage(), "trailTextureB.png");

    //
    // blur in Y back to trailTexture0
    //
    bloomTexture0.bindAsRenderTarget();

    bloomTexture1.bindAsTexture();

    glClear(GL_COLOR_BUFFER_BIT);
    app.renderer.renderSplashB(vec2f(0.0f, 1.0f) / (float)bloomSize.y);

    glEnable(GL_BLEND);
    bloomTexture0.unbindRenderTarget();

    bloomTexture0.bindAsTexture();

    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ONE);

    //app.renderer.renderGaussian(vec2f(0.0f, 1.0f) / (float)trailTexture0.dimensions().y);
    app.renderer.renderFullScreen(vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    if (GetAsyncKeyState(VK_F8))
    {
        cout << "Saving trail textures..." << endl;
        LodePNG::save(bloomTexture0.getImage(), "bloomTexture0.png");
        LodePNG::save(bloomTexture1.getImage(), "bloomTexture1.png");
    }
}
void SplashScreen::transferToPuzzleMode(int slotIndex)
{
    app.activeEventHandler = &app.ui;
    app.activeRenderHandler = &app.canvas;
    app.session.init(slotIndex);
    app.controller.loadCampaignPuzzle(app.session.currentCampaignLevel());
}

void SplashScreen::mouseUp(Uint8 button, int x, int y, bool shift, bool ctrl)
{
    transferToPuzzleMode(0);
}

void SplashScreen::keyDown(SDL_Keycode key, bool shift, bool ctrl)
{
    transferToPuzzleMode(0);
}
