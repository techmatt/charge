/*
God.h
Written by Matthew Fisher

The God class glues the operating system, the graphics device, the application interface,
and the window manager together.  It is essentially the first and the last code that is executed.
*/

extern bool RenderFPS;

class God {
public:
	void FreeMemory();
	void Init(HINSTANCE hInstance, int nCmdShow);			//initalize all the objects
	void MessageLoop(HINSTANCE hInstance, int nCmdShow);	//begin the main rendering loop

	void RenderFrame();				//render a new frame
	void ResetGraphicsObjects();	//reset all objects (needed after the application loses focus)
	void HandleReSize();			//the screen has been resized

private:
	String _WindowName;
	WindowObjects _Objects;          //set of objects associated with the window (frame timer, graphics device, window manager, input manager)
	AppInterface _App;				//the application interface, which loads graphics objects and tells the graphics device what to render
};
