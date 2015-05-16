
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
    SDL_Window *window = SDL_CreateWindow("Charge!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
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
    ui.init(renderer);

    //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
    for (int i = 0; i < 3; ++i)
    {
        //Clear the window
        SDL_RenderClear(renderer.SDL());

        //Get the width and height from the texture so we know how much to move x,y by
        //to tile it correctly
        /*int bW, bH;
        Uint32 format;
        int access;
        int qA = SDL_PIXELFORMAT_BGRA8888;
        int qB = SDL_PIXELFORMAT_ARGB8888;
        SDL_QueryTexture(texBkg.SDL(), &format, &access, &bW, &bH);
        //We want to tile our background so draw it 4 times
        renderer.render(texBkg, 0, 0);
        renderer.render(texBkg, bW, 0);
        renderer.render(texBkg, 0, bH);
        renderer.render(texBkg, bW, bH);*/

        ui.render(renderer);

        //Update the screen
        renderer.present();
        
        //Take a quick break after all that hard work
        SDL_Delay(1000);
    }

    //cleanup(background, image, renderer, window);

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}