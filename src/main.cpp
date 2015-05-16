
#include "main.h"

GameParameters g_gameParams;

void initGameParams(const ParameterFile &params)
{
    g_gameParams.load(params);
    g_gameParams.database = new Database();
    g_gameParams.database->init();
}

int SDL_main(int argc, char* argv[])
{
    ParameterFile parameterFile("../assets/parameters.txt");
    initGameParams(parameterFile);

    App app;
    return app.run();
}
