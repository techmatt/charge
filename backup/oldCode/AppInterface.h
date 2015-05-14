/*
AppInterface.h
Written by Matthew Fisher

The AppInterface class serves as an interface between the God class and the current application.
It contains a series of Control objects (when there is only one, I just call it MainControl.)
Each Control object is a collection of all the meshes/textures/lights etc. associated with
a given scene.  The AppInterface decides which Control currently has focus and redirects
all calls from the God class to the appopriate Control object.  For example, if an application
had an intro scene, a main menu, and a game window, all three might be seperate Control objects.
*/

class AppInterface
{
public:
	void Init(WindowObjects &O);			//called only once, initalizes objects
	void ReInit(WindowObjects &O);			//called whenever the device lost focus and now has it back
	void Render(WindowObjects &O);			//called once each frame when rendering is expected
	void ResetAllDevices(WindowObjects &O);	//called whenever devices might need to reset themselves
	void End(WindowObjects &O);								//frees all local memory

private:
	FrameController *c1;	//the MainControl object (only one control object exists, so it always has rendering focus.)
};
