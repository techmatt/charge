
#include "main.h"

#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#endif

GameParameters g_gameParams;

void chargeFatalError(const string &text)
{
    cout << "Fatal error: " << text << endl;
    cin.get();
    exit(1);
}

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

    string paramFilename = "../assets/parameters.txt";
    
    ParameterFile parameterFile(paramFilename);
    initGameParams(parameterFile);

    Renderer *renderer = nullptr;

    if (params().useSDLFallback)
        renderer = new RendererSDL();
    else if (params().graphics == "OpenGL")
        renderer = new RendererOpenGL();
	else if (params().graphics == "D3D11")
	{
#ifdef INCLUDE_D3D
		renderer = new RendererD3D11();
#endif
	}
    else
        chargeFatalError("Unrecognized graphics type: " + params().graphics);

    App app(*renderer);
    return app.run();
    //return app.runRendererTest();
}
