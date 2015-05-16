
#include "main.h"

void SDL::logError(const string &msg)
{
    cout << msg << " error: " << SDL_GetError() << endl;
}
