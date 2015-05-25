
#include "main.h"

void drawscene(SDL_Window *window, Renderer &renderer)
{
    GLQuad quad;
    quad.init();

    database().getTexture(renderer, "Background").bindOpenGL();

    GLProgram program;
    program.load("tutorial2.vert", "tutorial2.frag");
    program.bind();

    /* Loop our display increasing the number of shown vertexes each time.
    * Start with 2 vertexes (a line) and increase to 3 (a triangle) and 4 (a diamond) */
    for (int i = 2; i <= 4; i++)
    {
        /* Make our background black */
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);

        Texture &t = database().getTexture(renderer, "Background");
        
        t.bindOpenGL();

        quad.render();

        /* Swap our buffers to make our changes visible */
        SDL_GL_SwapWindow(window);

        /* Sleep for 2 seconds */
        SDL_Delay(2000);
    }

    /* Cleanup all the things we bound and allocated */
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

int App::run()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL::logError("SDL_Init");
        return 1;
    }

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

    if (data.renderer.type() == RendererTypeOpenGL)
        drawscene(window, data.renderer);

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
        data.renderer.clear();

        data.ui.render();

        data.renderer.present();
    }

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}
