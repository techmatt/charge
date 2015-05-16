
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

    RendererSDL renderer;
    renderer.init(window);

    //The textures we'll be using
    const std::string resPath = "../";
    Texture texBkg(renderer, resPath + "background.png");

    GameUI ui;
	
	GameState state;
	state.init();

	SDL_Event e;
    ui.init(renderer);

	bool quit = false;
	while (!quit)
    {
		while (SDL_PollEvent(&e)){
			//If user closes the window
			if (e.type == SDL_QUIT){
				quit = true;
			}
			//If user presses any key
			if (e.type == SDL_KEYDOWN){
				quit = true;
			}
			//If user clicks the mouse
			if (e.type == SDL_MOUSEBUTTONDOWN){
				//quit = true;
			}
		}

        //Clear the window
        SDL_RenderClear(renderer.SDL());

        ui.render(renderer, state);

        //Update the screen
        renderer.present();
    }

    //cleanup(background, image, renderer, window);

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}