
#ifndef __CHARGE_MAIN
#define __CHARGE_MAIN

#include <SDL.h>
#include <SDL_image.h>

#include "mLib/mLib.h"

using namespace std;

//
// Forward declarations
//
struct Board;
struct Component;

#include "core/constants.h"

#include "game/database.h"
#include "core/parameters.h"
#include "core/gameUtil.h"
#include "core/SDLUtil.h"

#include "renderer/renderer.h"
#include "renderer/rendererSDL.h"
#include "renderer/texture.h"

#include "game/misc.h"
#include "game/component.h"
#include "game/charge.h"
#include "game/circuit.h"
#include "game/board.h"
#include "game/gameState.h"

#include "gameUI/gameUI.h"

#include "core/app.h"

#endif
