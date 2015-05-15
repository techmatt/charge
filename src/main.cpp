
#include "main.h"

GameParameters g_gameParams;

int SDL_main(int argc, char* argv[])
{
    ParameterFile parameterFile("../assets/parameters.txt");
    initGameParams(parameterFile);

    App app;
    return app.run();
}
