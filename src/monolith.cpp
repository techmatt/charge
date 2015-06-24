
#include "main.h"

#include "main.cpp"

//
// core source files
//
#include "core/app.cpp"
#include "core/gameUtil.cpp"

//
// game source files
//
#include "game/board.cpp"
#include "game/charge.cpp"
#include "game/component.cpp"
#include "game/database.cpp"
#include "game/gameState.cpp"
#include "game/legacyLoader.cpp"
#include "game/buffer.cpp"

//
// gameUI source files
//
#include "gameUI/coordinates.cpp"
#include "gameUI/gameButton.cpp"
#include "gameUI/gameUI.cpp"
#include "gameUI/gameCanvas.cpp"
#include "gameUI/splashScreen.cpp"
#include "gameUI/selection.cpp"
#include "gameUI/gameController.cpp"
#include "gameUI/userSession.cpp"

//
// mLib source files
//
#include "mLib/common.cpp"
#include "mLib/imageLoaderLodePNG.cpp"
#include "mLib/lodepng.cpp"
#include "mLib/utility.cpp"
#include "mLib/FileDialog.cpp"

//
// renderer source files
//
#include "renderer/texture.cpp"
#include "renderer/renderTarget.cpp"
#include "renderer/rendererOpenGL.cpp"
#include "renderer/rendererSDL.cpp"
