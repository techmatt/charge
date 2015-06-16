
#ifndef __CHARGE_MAIN
#define __CHARGE_MAIN

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_image.h>
#include <SDL_TTF.h>
#include <SDL_mixer.h>

// TODO: try and link glew statically instead
//#define GL3_PROTOTYPES 1
#include <gl/glew.h>

#include "mLib/mLib.h"

using namespace std;

//
// Forward declarations
//
struct Board;
struct Component;
struct Database;
struct GameState;
struct Charge;
struct GameLocation;
struct ComponentModifiers;
struct UndoBuffer;
struct ComponentSet;
struct AppData;
class Renderer;
class RendererSDL;
class Texture;


#include "game/enums.h"
#include "core/constants.h"

#include "gameUI/coordinates.h"
#include "core/parameters.h"
#include "game/database.h"
#include "core/gameUtil.h"

#include "audio/audioDevice.h"

#include "renderer/renderer.h"
#include "renderer/rendererOpenGL.h"
#include "renderer/texture.h"
#include "renderer/renderTarget.h"

#include "game/misc.h"
#include "game/component.h"
#include "game/charge.h"
#include "game/explodingCharge.h"
#include "game/board.h"
#include "game/componentBuildState.h"
#include "game/gameState.h"
#include "game/legacyLoader.h"

#include "game/buffer.h"

#include "gameUI/userSession.h"
#include "gameUI/selection.h"
#include "gameUI/gameButton.h"
#include "gameUI/UIHelper.h"
#include "gameUI/gameCanvas.h"
#include "gameUI/gameUI.h"
#include "gameUI/splashScreen.h"
#include "gameUI/gameController.h"

#include "core/app.h"



#endif
