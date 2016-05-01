
#include "main.h"

#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
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
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyPrivateFrameworksURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
        return 1;
    }
    chdir(path);
#endif

    
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
