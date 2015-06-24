
#include "main.h"

#ifdef __APPLE__
#include <unistd.h>
#endif

GameParameters g_gameParams;

void initGameParams(const ParameterFile &params)
{
    g_gameParams.load(params);
    g_gameParams.database = new Database();
    g_gameParams.database->init();
}

int main(int argc, char* argv[])
{
#ifdef __APPLE__
    chdir("Charge.app/Contents/MacOS");
#endif

    //char cwd[1024];
    //getcwd(cwd, sizeof(cwd));
    //printf("%s\n",cwd);
    
    ParameterFile parameterFile("../assets/parameters.txt");
    initGameParams(parameterFile);

    Renderer *renderer;

    if (params().useSDLFallback)
        renderer = new RendererSDL();
    else
        renderer = new RendererOpenGL();

    App app(*renderer);
    return app.run();
    //return app.runRendererTest();
}
