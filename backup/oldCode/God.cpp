/*
God.cpp
Written by Matthew Fisher

The God class glues the operating system, the graphics device, the application interface,
and the window manager together.  It is essentially the first and the last code that is executed.
See God.h for a description of the class and member functions.
*/

//All source files include Main.h
#include "Main.h"

const int GraphicsDeviceType = GD_D3D9;
//GraphicsDeviceType tells God what type of graphics device/API to use.
//possible choices are:
//GD_D3D9		-> DirectX 9.0 API
//GD_D3D10		-> DirectX 10.0 API
//GD_OpenGL		-> OpenGL API
//GD_Software	-> Software rendering
//DirectX offers more functionality, but many people find it harder to understand and it's Windows only
//OpenGL is more widely supported but doesn't have nice things like mesh simplification built in
//Software is written by me, by hand, and is slow and has very limited functionality
//you'll need #define USE_DIRECTX or #define USE_OPENGL (in Main.h) to use DirectX or OpenGL

const int WindowBorderWidth = 8;
const int WindowBorderHeight = 27;

const int WindowWidth = 1236 + WindowBorderWidth;
const int WindowHeight = 824 + WindowBorderHeight;	//the default windowed-mode window width and height.  The window can be manually resized.

const bool VSync = true;			//true to wait for the monitor to finish the entire display (vertical retracing) before
									//the front buffer is flipped.  Setting this to false results in higher frame rate, but may
									//induce flickering.

bool RenderFPS = true;

void God::Init(HINSTANCE hInstance, int nCmdShow)
{
	GraphicsDevice *GD = NULL;
	_WindowName = "Charge!";

    //
	// Create a new GraphicsDevice object based upon the type of graphics device requested in GraphicsDeviceType
    //
	if(GraphicsDeviceType == GD_Software)
    {
        GD = new SoftwareGraphicsDevice;
    }
#ifdef USE_DIRECTX
	else if(GraphicsDeviceType == GD_D3D9)
    {
        GD = new D3D9GraphicsDevice;
    }
#endif
#ifdef USE_DIRECTX10
	else if(GraphicsDeviceType == GD_D3D10)
    {
        GD = new D3D10GraphicsDevice;
    }
#endif
#ifdef USE_OPENGL
	else if(GraphicsDeviceType == GD_OpenGL)
    {
        GD = new OpenGLGraphicsDevice;
    }
#endif
	else
	{
		Assert(NULL, "Invalid choice of graphics device type.");
	}
    Assert(GD != NULL, "Failed to allocate graphics device.");
    _Objects.SetGraphicsDevice(GD);

    g_WndProcContext = &_Objects;
	_Objects.GetWindowManager().InitAll(hInstance, nCmdShow, false, WindowWidth, WindowHeight, _WindowName);
    
	GD->SetVSync(VSync);	//set the GD vertical synchronization property
	GD->Init3D(_Objects);	//initalize the graphics device's 3D properties

	GD->SetScreenColor(RGBColor::Black);	//default clear color is black
	
	_Objects.GetFrameTimer().Start(60.0f);		//start at a guessed frame rate of 60 fps; this will be fixed in a few seconds to the "real" value
	_Objects.GetFrameTimer().Pause();			//ignore the first few frames from causing an abnormally low FPS

	_App.Init(_Objects);	//initalize this application's graphics _Objects
}

void God::RenderFrame()
{
	_Objects.GetGraphicsDevice().StartRender(_Objects);   //have the Graphics device prepare a new frame

	if(_Objects.GetGraphicsDevice().GetSystemActive())	//if we are good to render (we wouldn't be if say we were in the middle of a window change)
	{
		_Objects.GetFrameTimer().Frame();		//update the FPS counter
		_App.Render(_Objects);	                //have the application render its _Objects through the graphics device
        if(RenderFPS)
        {
            _Objects.GetGraphicsDevice().DrawString(String("fps: ") + String(_Objects.GetFrameTimer().GetFramesPerSec()), 0, 0, RGBColor::Black);
        }
	}
	else
	{
		_Objects.GetFrameTimer().Pause();		//if we aren't good to render, don't update the FPS counter
	}
	_Objects.GetGraphicsDevice().FinishRender(_Objects, _App);	//put the rendered scene onto the screen
}

void God::ResetGraphicsObjects()
{
	_Objects.GetFrameTimer().Pause();
	_App.ReInit(_Objects);	//reinialize all the scene _Objects
}

void God::HandleReSize()
{
	// avoid jerkiness in the frame timer
	_Objects.GetFrameTimer().Pause();
	_Objects.GetGraphicsDevice().ReSize(_Objects, _App);
}

void God::MessageLoop(HINSTANCE hInstance, int nCmdShow)
{
	//the main Windows message loop

	MSG msg;
	BOOL bGotMsg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );	//get a new message

    while( WM_QUIT != msg.message  )	//while we don't have the quit message
    {
        bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );	//get a new message
        if( bGotMsg )					//if we have a message
        {
            TranslateMessage( &msg );	//send it out
            DispatchMessage( &msg );
        }
        else
        {
            //
			// If we have no messages we decide it's a good time to render 
            //

            _Objects.GetInputManager().Frame(_Objects.GetWindowManager());

			RenderFrame();									//render a new frame
			if(_Objects.AcknowledgeReSize())
            {
                HandleReSize();		//if we're in a window and the user resizes it
            }
			if(_Objects.GetGraphicsDevice().NeedToRestoreDevices())					//if we've lost devices,
			{
				_Objects.GetFrameTimer().Pause();				//pause the timer for a short bit,
				ResetGraphicsObjects();						    //reset the graphics _Objects,
				_Objects.GetGraphicsDevice().DevicesRestored();	//tell the GD everything's okay
			}
        }
    }
}

void God::FreeMemory()
{
	_App.End(_Objects);

	_Objects.GetWindowManager().FreeMemory();	//free the window
	_Objects.GetGraphicsDevice().FreeMemory();	//free the graphics API data
}