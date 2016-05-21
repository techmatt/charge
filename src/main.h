
#ifndef __CHARGE_MAIN
#define __CHARGE_MAIN

#define GLEW_NO_GLU
//#define INCLUDE_D3D
//#define USE_D3D_COMPILER

#ifdef INCLUDE_D3D
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#endif

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#endif

// TODO: try and link glew statically instead
//#define GL3_PROTOTYPES 1

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif

#include "mLib/mLib.h"

using namespace std;

void chargeFatalError(const string &text);

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
class RendererOpenGL;
class RendererD3D11;
class Texture;
class D3D11Texture2D;

#include "game/enums.h"
#include "core/constants.h"

#include "gameUI/coordinates.h"
#include "core/parameters.h"
#include "game/database.h"
#include "core/gameUtil.h"

#include "audio/audioDevice.h"

#include "renderer/texture.h"
#include "renderer/renderTarget.h"
#include "renderer/renderer.h"

#ifdef INCLUDE_D3D
#include "d3d11.h"
#include "D3DCommon.h"
#include "D3DCompiler.h"
#define D3D_VALIDATE(statement) { HRESULT hr = statement;  if(FAILED(hr)) { chargeFatalError(#statement); } }
#include "SDL_syswm.h"
#include "d3d11/D3D11Utility.h"
#include "d3d11/D3D11ConstantBuffer.h"
#include "d3d11/D3D11VertexShader.h"
#include "d3d11/D3D11PixelShader.h"
#include "d3d11/D3D11ShaderManager.h"
#include "d3d11/D3D11TriMesh.h"
#include "renderer/rendererD3D11.h"
#include "d3d11/D3D11RenderTarget.h"
#include "d3d11/D3D11Texture2D.h"
#endif

#include "renderer/rendererOpenGL.h"
#include "renderer/rendererSDL.h"


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
