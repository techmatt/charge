
#include "main.h"

const bool dumpHighlightMode = false;

void SplashScreen::init()
{
    srand((unsigned int)time(nullptr));
	//checkGLError();
    app.renderer.initMotionBlur(0.5f, numeric_limits<int>::max());
    //checkGLError();

    app.renderer.bindMainRenderTarget();

    for (const string &line : util::getFileLines(params().assetDir + "highlightsSplashA.txt"))
    {
        auto parts = util::split(line, ' ');
        if (parts.size() == 3)
            splashHighlights.push_back(vec3f(convert::toFloat(parts[0]), convert::toFloat(parts[1]), convert::toFloat(parts[2])));
    }
}

void SplashScreen::render()
{
    vec2f canonicalDims = GameUtil::getCanonicalSize();
    CoordinateFrame frame = app.renderer.getWindowCoordinateFrame();

    //rect2f backgroundRect(vec2f(0.0f, 0.0f), canonicalDims);
    //app.renderer.render(database().getTexture(app.renderer, "splash"), frame.toContainer(backgroundRect), 0.99f, vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    app.renderer.renderFullScreen(database().getTexture(app.renderer, "splash"), vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    if (app.renderer.type() == RendererType::OpenGL)
        bloom();

    // splash screen should have visual depiction of each level solved state in a grid?
}

void SplashScreen::bloom()
{
    checkGLError();

    static float time = 0.0f;
    static int frameIndex = 0;

    const Bitmap &bmp = database().getTexture(app.renderer, "splash").bmp();

    const float s = 0.98f;

    frameIndex++;
    time += 0.03f;

    if (frameIndex % 100 == 0)
    {
        vec3f newTarget;
        if (rand() % 8 == 0)
            newTarget = bmp(rand() % bmp.width(), rand() % bmp.height()).toVec3f();
        else
            newTarget = splashHighlights[rand() % splashHighlights.size()];
        if (rand() % 2 == 0) focusColorATarget = newTarget;
        else focusColorBTarget = newTarget;
    }

    if (!dumpHighlightMode)
    {
        focusColorA = focusColorA * s + focusColorATarget * (1.0f - s);
        focusColorB = focusColorB * s + focusColorBTarget * (1.0f - s);
    }

    const vec2i bloomSize = app.renderer.getWindowSize() / 4;
    if (bloomTexture0.dimensions() != bloomSize)
    {
        bloomTexture0.init(app.renderer, bloomSize);
        bloomTexture1.init(app.renderer, bloomSize);
    }

    checkGLError();

    glViewport(0, 0, bloomSize.x, bloomSize.y);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    bloomTexture0.bindAsRenderTarget();

    glClear(GL_COLOR_BUFFER_BIT);

    checkGLError();
    
    database().getTexture(app.renderer, "splash").bindOpenGL();

    //vec3f colorA(113.0f / 255.0f, 178.0f / 255.0f, 124.0f / 255.0f);

    float intensityA = dumpHighlightMode ? 1.0f : (sin(time) * 0.5f + 0.5f);
    float intensityB = dumpHighlightMode ? 1.0f : (sin(time * 0.75f + math::PIf) * 0.5f + 0.5f);
    checkGLError();
    app.renderer.renderSplashA(focusColorA, focusColorB, vec2f(intensityA, intensityB));

    //LodePNG::save(bloomTexture0.getImage(), "bloomTextureA.png");

    //
    // blur in X to trailTexture1
    //
    checkGLError();
    bloomTexture1.bindAsRenderTarget();

    bloomTexture0.bindAsTexture();

    glClear(GL_COLOR_BUFFER_BIT);
    app.renderer.renderSplashB(vec2f(1.0f, 0.0f) / (float)bloomSize.x);

    //LodePNG::save(bloomTexture1.getImage(), "bloomTextureB.png");

    //
    // blur in Y back to trailTexture0
    //
    bloomTexture0.bindAsRenderTarget();

    bloomTexture1.bindAsTexture();

    glClear(GL_COLOR_BUFFER_BIT);
    app.renderer.renderSplashB(vec2f(0.0f, 1.0f) / (float)bloomSize.y);

    glEnable(GL_BLEND);
    
    app.renderer.bindMainRenderTarget();

    glViewport(0, 0, app.renderer.getWindowSize().x, app.renderer.getWindowSize().y);

    //LodePNG::save(bloomTexture0.getImage(), "bloomTextureC.png");

    bloomTexture0.bindAsTexture();

    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ONE);

    //app.renderer.renderGaussian(vec2f(0.0f, 1.0f) / (float)trailTexture0.dimensions().y);
    app.renderer.renderFullScreen(vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    /*if (GetAsyncKeyState(VK_F8))
    {
        cout << "Saving trail textures..." << endl;
        LodePNG::save(bloomTexture0.getImage(), "bloomTexture0.png");
        LodePNG::save(bloomTexture1.getImage(), "bloomTexture1.png");
    }*/
}
void SplashScreen::transferToPuzzleMode(int slotIndex)
{
    app.activeEventHandler = &app.ui;
    app.activeRenderHandler = &app.canvas;
    app.session.init(slotIndex);
    app.controller.loadLevelPackPuzzle("Campaign", app.session.currentCampaignLevel(), "BasePuzzle");

    app.renderer.initMotionBlur(0.1f, 80);
}

void SplashScreen::mouseMove(Uint32 buttonState, int x, int y)
{
    if (dumpHighlightMode)
    {
        const Bitmap &bmp = database().getTexture(app.renderer, "splash").bmp();

        float x2 = math::linearMap(0.0f, (float)app.renderer.getWindowSize().x, 0.0f, (float)bmp.width(), (float)x);
        float y2 = math::linearMap(0.0f, (float)app.renderer.getWindowSize().y, (float)bmp.height(), 0.0f, (float)y);

        int bmpX = math::clamp((int)x2, 0, (int)bmp.width() - 1);
        int bmpY = math::clamp((int)y2, 0, (int)bmp.height() - 1);

        focusColorA = vec4f(bmp(bmpX, bmpY)).getVec3();
    }
}

void SplashScreen::mouseUp(Uint8 button, int x, int y, int clicks, bool shift, bool ctrl)
{
    if (dumpHighlightMode)
    {
        ofstream highlightFile("highlights.txt", ios::app);
        highlightFile << focusColorA << endl;
    }
    else
    {
        transferToPuzzleMode(0);
    }
}

void SplashScreen::keyDown(SDL_Keycode key, bool shift, bool ctrl, bool alt)
{
    transferToPuzzleMode(0);
}
