/*
AppInterface.cpp
Written by Matthew Fisher

Serves as an interface between the God class and the current application.
See AppInterface.h for a full description of the AppInterface class and its member functions/variables.
*/

//All source files include Main.h
#include "Main.h"

void AppInterface::Init(WindowObjects &O)
{
	c1 = new MainControl;	//this MainControl object will receive all our initalization/rendering calls
	c1->Init(O);		    //Initalize c1 (loads meshes, textures, etc.)
}

void AppInterface::ReInit(WindowObjects &O)
{
	c1->ReInit(O);		    //If data (such as textures, meshes, etc.) were lost, restore them
							//This happens when our window loses focus
}

void AppInterface::Render(WindowObjects &O)
{
	c1->Render(O);		    //Render the current frame
}

void AppInterface::ResetAllDevices(WindowObjects &O)
{
	//If any devices need resetting, they can be done here.  However, almost all objects can be
	//reset in ReInit, so normally nothing needs to be done here.
    c1->ReleaseObjects(O);
}

void AppInterface::End(WindowObjects &O)
{
	c1->FreeMemory(O);
	delete c1;
}
