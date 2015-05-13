
#include "main.h"

//Screen attributes
/*const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void logSDLError(std::ostream &os, const std::string &msg){
    os << msg << " error: " << SDL_GetError() << std::endl;
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
    SDL_Texture *texture = nullptr;
    //Load the image
    SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
    //If the loading went ok, convert to texture and return the texture
    if (loadedImage != nullptr){
        texture = SDL_CreateTextureFromSurface(ren, loadedImage);
        SDL_FreeSurface(loadedImage);
        //Make sure converting went ok too
        if (texture == nullptr){
            logSDLError(std::cout, "CreateTextureFromSurface");
        }
    }
    else {
        logSDLError(std::cout, "LoadBMP");
    }
    return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    //Query the texture to get its width and height to use
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

int App::run()
{
    //Start up SDL and make sure it went ok
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        logSDLError(std::cout, "SDL_Init");
        return 1;
    }

    //Setup our window and renderer
    SDL_Window *window = SDL_CreateWindow("Lesson 2", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        logSDLError(std::cout, "CreateWindow");
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
    {
        logSDLError(std::cout, "CreateRenderer");
        SDL_Quit();
        return 1;
    }

    //The textures we'll be using
    const std::string resPath = "../";
    SDL_Texture *background = loadTexture(resPath + "background.bmp", renderer);
    SDL_Texture *image = loadTexture(resPath + "image.bmp", renderer);
    //Make sure they both loaded ok
    if (background == nullptr || image == nullptr)
    {
        SDL_Quit();
        return 1;
    }

    //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
    for (int i = 0; i < 3; ++i)
    {
        //Clear the window
        SDL_RenderClear(renderer);

        //Get the width and height from the texture so we know how much to move x,y by
        //to tile it correctly
        int bW, bH;
        SDL_QueryTexture(background, NULL, NULL, &bW, &bH);
        //We want to tile our background so draw it 4 times
        renderTexture(background, renderer, 0, 0);
        renderTexture(background, renderer, bW, 0);
        renderTexture(background, renderer, 0, bH);
        renderTexture(background, renderer, bW, bH);

        //Draw our image in the center of the window
        //We need the foreground image's width to properly compute the position
        //of it's top left corner so that the image will be centered
        int iW, iH;
        SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
        int x = SCREEN_WIDTH / 2 - iW / 2;
        int y = SCREEN_HEIGHT / 2 - iH / 2;
        renderTexture(image, renderer, x, y);

        //Update the screen
        SDL_RenderPresent(renderer);
        //Take a quick break after all that hard work
        SDL_Delay(1000);
    }

    //cleanup(background, image, renderer, window);

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}*/

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

    //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
    for (int i = 0; i < 3; ++i)
    {
        //Clear the window
        SDL_RenderClear(renderer.SDL());

        //Get the width and height from the texture so we know how much to move x,y by
        //to tile it correctly
        int bW, bH;
        SDL_QueryTexture(texBkg.SDL(), NULL, NULL, &bW, &bH);
        //We want to tile our background so draw it 4 times
        renderer.render(texBkg, 0, 0);
        renderer.render(texBkg, bW, 0);
        renderer.render(texBkg, 0, bH);
        renderer.render(texBkg, bW, bH);

        //Update the screen
        SDL_RenderPresent(renderer.SDL());

        //Take a quick break after all that hard work
        SDL_Delay(1000);
    }

    //cleanup(background, image, renderer, window);

    SDL_Quit();

    cout << "Press any key to continue..." << endl;
    cin.get();

    return 0;
}