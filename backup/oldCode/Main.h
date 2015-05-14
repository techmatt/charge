/*
Main.h
Written by Matthew Fisher

Main.h is included by all source files and includes every header file in the correct order.
*/
#pragma once

//
// Config.h includes a series of #defines used to control compiling options
//
#include "Config.h"

//
// Engine.h includes everything that rarely changes between applications, such as vector/Matrix4 libraries,
// OpenGL/DirectX graphics devices, software rasterizers, etc.
//
#include "Engine.h"

#include "Enums.h"
#include "Coordinate.h"
#include "Building.h"
#include "Board.h"
#include "Charge.h"
#include "Misc.h"
#include "GameAudio.h"
#include "Game.h"
#include "GameUI.h"
#include "ThatWhichRendersGames.h"
#include "PuzzleController.h"

//
// MainControl.h includes everything that changes often between applications, such as what meshes to load,
// and also determines what is rendered each frame.
//
#include "MainControl.h"

//
// AppInterface serves as a go-between for the God class and the MainControl class.  Very straightforward
// and rarely needs to be changed; more useful if there are multiple MainControl types of classes.
//
#include "AppInterface.h"

//
// The God class glues the operating system, the graphics device, the application interface,
// and the window manager together.  It is essentially the first and the last code that is executed.
//
#include "God.h"
