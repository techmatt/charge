/*
Engine10.h
Written by Matthew Fisher

Many objects are universal and don't need to change often.  The source for all such files
are included in this file.  Most people would compile all these into a single *.lib or *.dll,
but I prefer to keep all my source readily accessible.
*/

/****************************/
/*		Core Functions		*/
/****************************/
//These are generic functions that have no specific place anywhere else.

//Shell definition of several classes/structures.  Necessary because some classes are cross-referencing
#include "Engine10\Core Functions\ClassList.h"

//All #includes that are generic or written by other people
#include "Engine10\Core Functions\ExternalFiles.h"

//Collection of useful constants, macros, and functions
#include "Engine10\Core Functions\Stdhdr.h"

//The Vector class contains dynamic arrays of a template type.  This is equivalent to the Standard Template 
//Library's vector class, but for various reasons I like to have my own.
#include "Engine10\Core Functions\ArrayVector.h"
#include "Engine10\Core Functions\DataStream.h"
#include "Engine10\Core Functions\Grid.h"

//The LinkedList class contains linked lists of a template type.  There is also a STL equivalent that I
//choose not to use.
#include "Engine10\Core Functions\LinkedList.h"

//definition of various keys.
#include "Engine10\Core Functions\Keydefs.h"

//Basic string class.
#include "Engine10\Core Functions\String.h"

//Compressor & decompressor.
#include "Engine10\Core Functions\Compressor.h"

#include "Engine10\Core Functions\Mutex.h"

/****************************/
/*		Math Structures		*/
/****************************/
//All forms of mathematical structures and functions are here.

//Generic 32-bit RGBA color structure
#include "Engine10\Graphics Structures\RGBColor.h"

//Transformations between color spaces
#include "Engine10\Graphics Structures\ColorSpaces.h"

//Vector, which defines 3D and 4D vectors, is the most used set of functions anywhere in the project.
#include "Engine10\Math Structures\Vector.h"

//A standard 3D line
#include "Engine10\Math Structures\Line3D.h"

//A standard 3D plane (space plane.)  Essentially just the surface defined by a*x + b*y + c*z + d = 0
#include "Engine10\Math Structures\Plane.h"

//a quaternion structure (a quaternion is used to reprsent rotations.)  Currently, I haven't implemented this structure.
#include "Engine10\Math Structures\Quaternion.h"

//a 4x4 Matrix4 structure.  Used very often for affine vector transformations
#include "Engine10\Math Structures\Matrix4.h"

//Dense Matrix4 class
#include "Engine10\Math Structures\DenseMatrix.h"

//Sparse Matrix4 class, used by LinearSolver
#include "Engine10\Math Structures\SparseMatrix.h"

//Linear optimizer.  Uses (bi-)conjugate gradient descent or TAUCS's direct solver.
#include "Engine10\Math Structures\LinearSolver.h"

//Generic non-linear optimizer.  Uses conjugate gradient descent (or other inferior/sub-component algorithms like line search.)
#include "Engine10\Math Structures\Optimizer.h"

//Generic function predictor.  Given sample points for a function predicts the function on the entire domain.
#include "Engine10\Math Structures\Predictor.h"

//Neural network implemented under the predictor framework
#include "Engine10\Math Structures\NeuralNetwork.h"

/****************************/
/*	 Graphics Structures	*/
/****************************/
//These are structures specific to computer graphics rendering

//A bitmap class (a 2D array of RGBColor's)
#include "Engine10\Graphics Structures\Bitmap.h"

//Encapsulates functionality pertaining to non-standard image compression routines
#include "Engine10\Graphics Structures\ImageCompressor.h"

//A simple ZBuffer structure, for software rendering.  Stores the depth of each screen pixel.
#include "Engine10\Graphics Structures\ZBuffer.h"

//MatrixController manages the 3 transformations (world, view, projection) central to the graphics pipeline
#include "Engine10\Graphics Structures\MatrixController.h"

//Camera represents a 3-D free floating camera.
#include "Engine10\Graphics Structures\Camera.h"

//Rendering functions for lines and polygons in software.  Not very efficent.
#include "Engine10\Graphics Structures\PrimitiveRender.h"

//MeshVertex represents a single vertex in a mesh structure.
#include "Engine10\Graphics Structures\MeshVertex.h"

//Clipper is used to take an arbitrary polygon and clip it against the viewing frustrum.
#include "Engine10\Graphics Structures\Clipper.h"

//Viewport represents a rectangle on the screen used for rendering
#include "Engine10\Graphics Structures\Viewport.h"

#include "Engine10\Graphics Structures\KDTree3.h"
#include "Engine10\Graphics Structures\KDTreeN.h"
#include "Engine10\Graphics Structures\KDTreeNCloseQuery.h"
#include "Engine10\Graphics Structures\PointSet.h"

//Interface to Poisson reconstruction DLL
#include "Engine10\Advanced Meshes\PoissonReconstruction.h"
#include "Engine10\Advanced Meshes\PoissonReconstructionHelper.h"

//Point set is informationa about a collection of points and possibly normal information, to be used in mesh
//reconstructors
#include "Engine10\Graphics Structures\PoissonReconstructor.h"

/****************************/
/*	   Mesh Structures		*/
/****************************/
//These files all center around creating and manipulating triangle meshes.
//These meshes are "simple" in that they contain no adjaceny information.

//The HTable structure represents a geometric hash of 3-space used by BaseMesh for various indexing operations.
#include "Engine10\Base Mesh\HTable.h"

//BaseMesh is an abstract mesh class that defines basic mesh functionality.  It also includes source for most
//of the manipulation (shape generation, file loading, etc.) that is possible under this generic structure.
#include "Engine10\Base Mesh\BaseMesh.h"

//D3D9Mesh is a DirectX instance of the BaseMesh class
#include "Engine10\Advanced Meshes\D3D9Mesh.h"
#include "Engine10\Advanced Meshes\D3D10Mesh.h"

//Mesh is a standard C++ array instance of the BaseMesh class.  It's the only kind that can be used by OpenGL,
//but it will run fine under DirectX as well (if not as quickly as a D3D9Mesh.)
#include "Engine10\Advanced Meshes\Mesh.h"

/************************************/
/*	   Subdivision Structures		*/
/************************************/
//These files all center around edge-based mesh structures and an implementation
//of the loop subdivision algorithm that relies upon an edge-based structure.

//definition of the Triangle structure
#include "Engine10\Subdivision\Triangle.h"

//functions for an indivudial vertex
#include "Engine10\Subdivision\Vertex.h"

//Edge-based functions
#include "Engine10\Subdivision\FullEdge.h"

//EdgeMesh class encapsulates and stores arrays of all components in the edge-based data structure
#include "Engine10\Subdivision\EdgeMesh.h"

/****************************/
/*		   Textures			*/
/****************************/
//These files all control rendering textured polygons through DirectX and OpenGL.

//BaseTexture is an abstract class that defines base-level texture functionality.
#include "Engine10\Textures\BaseTexture.h"

//DirectX implementation of BaseTexture
#include "Engine10\Textures\D3D9Texture.h"
#include "Engine10\Textures\D3D10Texture.h"

//OpenGL implementation of BaseTexture
#include "Engine10\Textures\OpenGLTexture.h"

//Generic texture class that maps to either a DirectX or OpenGL texture based upon what the current window is using.
#include "Engine10\Textures\Texture.h"

/****************************/
/*	 DirectX Objects    	*/
/****************************/
//Objects that encapsulate shader and render-to-texture functionality in DirectX

//Abstract class used for all resetable graphics objects
#include "Engine10\DXObjects\BaseDX9GraphicsObject.h"

//Objects that can be used as the render target
#include "Engine10\DXObjects\D3D9RenderTargetTexture.h"
#include "Engine10\DXObjects\D3D9RenderTargetSurface.h"

//Vertex decleration used for shaders
#include "Engine10\DXObjects\VDecleration9.h"

//Pixel shaders
#include "Engine10\DXObjects\PShader9.h"

//Vertex shaders
#include "Engine10\DXObjects\VShader9.h"

//Effects
#include "Engine10\DXObjects\D3D10Effect.h"

//D3D Mesh that can reset itself with a RAM copy
#include "Engine10\DXObjects\D3D9SafeMesh.h"

/****************************/
/*	   Graphics Devices		*/
/****************************/
//A graphics device controls the interaction between Windows and a graphics API.

//GraphicsDevice is the abstract class that contains functionality that all graphics devices must expose.
//any GraphicsDevice can do simple things like render meshes into the main rendering window and handle
//the world, view and projection transforms.
#include "Engine10\Graphics Devices\GraphicsDevice.h"

//a D3D instance of GraphicsDevice
#include "Engine10\Graphics Devices\D3D9GraphicsDevice.h"
#include "Engine10\Graphics Devices\D3D10GraphicsDevice.h"

//a OpenGL instance of GraphicsDevice
#include "Engine10\Graphics Devices\OpenGLGraphicsDevice.h"

//a software instance of GraphicsDevice
#include "Engine10\Graphics Devices\SoftwareGraphicsDevice.h"

/****************************/
/*	  Audio		            */
/****************************/
// Handles all things sound-related
//

//WaveFile stores a sound file in RAM
#include "Engine10\Audio\WaveFile.h"

//WaveFileReader loads a wave file from disk
#include "Engine10\Audio\WaveFileReader.h"

//An audio device plays WaveFiles
#include "Engine10\Audio\AudioDevice.h"

/****************************/
/*	  Utility Objects		*/
/****************************/
// These simplify access to existing classes
//

//MovieExporter manages screen capture and exports as a sequence of images for compilation
//into a movie.
#include "Engine10\Utility Objects\MovieExporter.h"

//XFileRenderer is a class that encapsulates loading XFiles, including all the material and texture data,
//(although not the effect data) and rendering the resulting mesh.
#include "Engine10\Utility Objects\XFileRenderer.h"

//the Indicator class rapidly renders spheres or cylinders between arbitrary points in three space
//by using stored mesh data and loading a new Matrix4 each time a cylinder or sphere needs "indicating."
//also can do basic viewing frustrum (camera) analysis or rendering.
#include "Engine10\Utility Objects\Indicator.h"

//the Parameter class loads a paramater file as a simple set of "Parameter=Option" lines
#include "Engine10\Utility Objects\ParameterFile.h"

//UVAtlas uses D3D to generate a paramaterization of a mesh in the plane
#include "Engine10\Utility Objects\UVAtlas.h"

//The frame timer reads the system time and maintains the frames per second counter.
#include "Engine10\Utility Objects\FrameTimer.h"

//Profiler is used to record the percent of time spent in code
#include "Engine10\Utility Objects\Profiler.h"

//FileCollection mimics a bunch of files merged into one
#include "Engine10\Utility Objects\FileCollection.h"

//MenuInterface handles all interaction with the menu bar
#include "Engine10\Utility Objects\MenuInterface.h"

//DialogInterface brings up save and open dialogs
#include "Engine10\Utility Objects\DialogInterface.h"

/****************************/
/*	 Windows Controllers	*/
/****************************/
//These files handle the interaction between the application and Windows.  This includes files that contain
//WinMain and create the main application window.  There is also a frame timer and a screen capture mechanism.

//The input manager stores and controls access to keyboard and mouse data
#include "Engine10\Windows Controller\InputManager.h"

//Creating a new window involves having a callback function, WndProc, that is called whenever Windows has
//an event to send to your window/application.  EventHandler contains the WndProc file as well as the global
//array of keys that tells which keys are current pressed/depressed.
#include "Engine10\Windows Controller\EventHandler.h"

//WindowManager handles creating and managing the application's main window
#include "Engine10\Windows Controller\WindowManager.h"

//A set of useful objects associated with each window (the graphics device, window manager, input manager, and
//frame timer.)
#include "Engine10\Windows Controller\WindowObjects.h"

//Abstract class for an object which controls all rendering each frame.
#include "Engine10\Windows Controller\FrameController.h"

//The WinMain function itself.  Just creates the God class and runs it.  See God.h/God.cpp
#include "Engine10\Windows Controller\WinMain.h"
