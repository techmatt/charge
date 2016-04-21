
#include "main.h"

/*
this is code I added to SDL, SDL_windowsevents.c to control the aspect ratio ->
case WM_SIZING:
{
float ratio = (float)g_SDL_matt_window_aspect_ratio;
if (ratio > 0.0)
{
RECT screenRect;
GetWindowRect(GetDesktopWindow(), &screenRect);

RECT *rect = (RECT *)lParam;

RECT clientRect, windowRect;
GetClientRect(hwnd, &clientRect);
GetWindowRect(hwnd, &windowRect);
int borderWidth = windowRect.right - windowRect.left - clientRect.right;
int borderHeight = windowRect.bottom - windowRect.top - clientRect.bottom;

int proposedX = rect->right - rect->left - borderWidth;
int proposedY = rect->bottom - rect->top - borderHeight;

float newWidth = (float)proposedY / ratio + borderWidth;
float newHeight = (float)proposedX * ratio + borderHeight;

if (newHeight > screenRect.bottom)
{
newHeight = (float)screenRect.bottom;
newWidth = (float)proposedY / ratio + borderWidth;
rect->right = rect->left + (int)(newWidth + 0.5f);
}

switch (wParam)
{
case WMSZ_BOTTOM:
rect->right = rect->left + (int)(newWidth + 0.5f);
break;
case WMSZ_LEFT:
case WMSZ_TOPLEFT:
case WMSZ_TOPRIGHT:
rect->top = rect->bottom - (int)(newHeight + 0.5f);
break;
case WMSZ_RIGHT:
case WMSZ_BOTTOMRIGHT:
case WMSZ_BOTTOMLEFT:
rect->bottom = rect->top + (int)(newHeight + 0.5f);
break;
case WMSZ_TOP:
rect->left = rect->right - (int)(newWidth + 0.5f);
break;
}

return TRUE;
}
}
*/

void App::initRenderer()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        cout << "SDL_Init error: " << SDL_GetError() << endl;
        return;
    }

    TTF_Init();

    //Setup our window and renderer
    //g_SDL_matt_window_aspect_ratio = (double)params().canonicalDims.y / (double)params().canonicalDims.x;
    int windowHeight = math::round(params().canonicalDims.y);
    int windowWidth = math::round(params().canonicalDims.x);

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
    {
        const int minDim = min(displayMode.w, displayMode.h);
        windowHeight = math::round((double)minDim * 0.9);
        windowWidth = math::round(windowHeight * params().canonicalDims.x / params().canonicalDims.y);

        // in case of weird aspect ratio monitors
        while (windowWidth > displayMode.w)
        {
            windowHeight = math::round(windowHeight * 0.8);
            windowWidth = math::round(windowWidth * 0.8);
        }
    }

    SDL_Window *window = SDL_CreateWindow("Charge!", 15, 35, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE); //SDL_WINDOW_FULLSCREEN_DESKTOP

    if (window == nullptr)
    {
        cout << "SDL_CreateWindow error: " << SDL_GetError() << endl;
        return;
    }

    data.audio.init();

    data.renderer.init(window);
}

int App::runRendererTest()
{
    initRenderer();

    SDL_Event event;

    bool quit = false;

    Bitmap bmp(80, 100);
    for (const auto &p : bmp)
        p.value = RGBColor(p.x % 256, p.y % 256, (p.x * p.y) % 256);

    while (!quit)
    {
        bool eventFound = false;

        while (SDL_PollEvent(&event))
        {
            eventFound = true;
            if (event.type == SDL_QUIT || event.type == SDL_APP_TERMINATING)
            {
                quit = true;
            }
        }

        //
        // render the game
        //
        data.renderer.bindMainRenderTarget();
        data.renderer.clear();

        Texture tex;
        tex.load(data.renderer, bmp);

        data.renderer.render(tex, rect2f(10.0f, 20.0f, 80.0f, 190.0f), 0.5f, vec4f(1.0f, 1.0f, 1.0f, 1.0f));

        data.renderer.present();
    }

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}

int App::run()
{
    initRenderer();

    database().initTextures(data.renderer);

    data.state.init();

	data.undoBuffer.init();
	data.undoBuffer.reset(data.state);

    data.ui.init();
    data.canvas.init();
    data.controller.init();

    //
    // should init last to control motion blur state
    //
    data.splash.init();

    data.activeEventHandler = &data.splash;
    data.activeRenderHandler = &data.splash;

    SDL_Event event;

	bool quit = false;
    bool minimized = false;

	while (!quit)
    {
        bool eventFound = false;

		while (SDL_PollEvent(&event))
        {
            eventFound = true;
            if (event.type == SDL_QUIT || event.type == SDL_APP_TERMINATING)
            {
				quit = true;
			}
			if (event.type == SDL_KEYDOWN)
            {
                const bool shift = (event.key.keysym.mod & KMOD_SHIFT) != 0;
                const bool ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
                data.activeEventHandler->keyDown(event.key.keysym.sym, shift, ctrl);
			}
            if (event.type == SDL_KEYUP)
            {
                data.activeEventHandler->keyUp(event.key.keysym.sym);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                const SDL_Keymod mod = SDL_GetModState();
                const bool shift = (mod & KMOD_SHIFT) != 0;
                const bool ctrl = (mod & KMOD_CTRL) != 0;
                data.activeEventHandler->mouseDown(event.button.button, event.button.x, event.button.y, shift, ctrl);
			}
			if (event.type == SDL_MOUSEBUTTONUP)
			{
                const SDL_Keymod mod = SDL_GetModState();
                const bool shift = (mod & KMOD_SHIFT) != 0;
                const bool ctrl = (mod & KMOD_CTRL) != 0;
                data.activeEventHandler->mouseUp(event.button.button, event.button.x, event.button.y, shift, ctrl);
			}
            if (event.type == SDL_MOUSEMOTION)
            {
                data.activeEventHandler->mouseMove(event.motion.state, event.motion.x, event.motion.y);
            }
            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                    minimized = true;
                if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED || event.window.event == SDL_WINDOWEVENT_RESTORED || event.window.event == SDL_WINDOWEVENT_ENTER)
                    minimized = false;
            }
		}

        //
        // advance the game
        //
        data.frameTimer.frame();
        data.controller.step();

        //
        // render frame only if necessary
        //
        bool renderFrame = !minimized && (eventFound || data.renderer.motionBlurActive() || data.controller.puzzleMode == PuzzleMode::Executing);
        if (renderFrame)
        {
            //
            // render the game
            //
            data.renderer.bindMainRenderTarget();
            data.renderer.clear();

            data.activeRenderHandler->render();

            data.renderer.present();
        }

        int extraSleepTimeMS = math::round(((1.0f / constants::FPSlimit) - data.frameTimer.elapsedTime()) * 1000.0f);
        if (extraSleepTimeMS >= 3)
            util::sleep(extraSleepTimeMS);
    }

    data.session.save();
    data.session.saveProgress(data);

    SDL_Quit();

    //cout << "Press any key to continue..." << endl;
    //cin.get();

    return 0;
}
