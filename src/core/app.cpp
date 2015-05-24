
#include "main.h"

int App::run()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL::logError("SDL_Init");
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    //Setup our window and renderer
    int windowHeight = math::round(params().canonicalDims.y) * 2;
    int windowWidth = math::round(params().canonicalDims.x) * 2;
	SDL_Window *window = SDL_CreateWindow("Charge!", 50, 50, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE); //SDL_WINDOW_FULLSCREEN_DESKTOP

    if (window == nullptr)
    {
        SDL::logError("CreateWindow");
        SDL_Quit();
        return 1;
    }

    data.renderer.init(window);

    database().initTextures(data.renderer);

    data.state.init();

    data.ui.init();

    SDL_Event event;

	bool quit = false;
	while (!quit)
    {
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT)
            {
				quit = true;
			}
			if (event.type == SDL_KEYDOWN)
            {
                data.ui.keyDown(event.key.keysym.sym);
			}
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                data.ui.mouseDown(event.button.button, event.button.x, event.button.y);
			}
            if (event.type == SDL_MOUSEMOTION)
            {
                data.ui.mouseMove(event.motion.state, event.motion.x, event.motion.y);
            }
		}

        //
        // advance the game
        //
        data.ui.step();

        //
        // render the game
        //
        SDL_RenderClear(data.renderer.SDL());

        data.ui.render();

        data.renderer.present();
    }

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}
