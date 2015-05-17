
#include "main.h"

int App::run()
{
    //Start up SDL and make sure it went ok
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL::logError("SDL_Init");
        return 1;
    }

    //Setup our window and renderer
	SDL_Window *window = SDL_CreateWindow("Charge!", 100, 100, 1536, 1024, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        SDL::logError("CreateWindow");
        SDL_Quit();
        return 1;
    }

    
    data.renderer.init(window);

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

        //Clear the window
        SDL_RenderClear(data.renderer.SDL());

        data.ui.render();

        //Update the screen
        data.renderer.present();
    }

    //cleanup(background, image, renderer, window);

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}