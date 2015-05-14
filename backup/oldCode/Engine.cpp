/*
Engine10.cpp
Written by Matthew Fisher

Many objects are universal and don't need to change often.  The source for all such files
are included in this file.  Most people would compile all these into a single *.lib or *.dll,
but I prefer to keep all my source readily accessible.  See Engine10.h for a defintion of all files
included here.
*/

//All source files include Main.h
#include "Main.h"

/****************************/
/*		Core Functions		*/
/****************************/
//These are generic functions that have no specific place anywhere else.

#include "Engine10\Core Functions\Stdhdr.cpp"
#include "Engine10\Core Functions\String.cpp"
#include "Engine10\Core Functions\DataStream.cpp"
#include "Engine10\Core Functions\Compressor.cpp"
#include "Engine10\Core Functions\Mutex.cpp"

/****************************/
/*		Math Structures		*/
/****************************/
//All forms of mathematical structures and functions are here.

#include "Engine10\Math Structures\VectorNonInline.cpp"
#include "Engine10\Math Structures\Line3D.cpp"
#include "Engine10\Math Structures\Plane.cpp"
#include "Engine10\Math Structures\Quaternion.cpp"
#include "Engine10\Math Structures\Matrix4.cpp"
#include "Engine10\Math Structures\SparseMatrix.cpp"
#include "Engine10\Math Structures\DenseMatrix.cpp"
#include "Engine10\Math Structures\LinearSolver.cpp"
#include "Engine10\Math Structures\Optimizer.cpp"

/****************************/
/*	 Graphics Structures	*/
/****************************/
//These are structures specific to computer graphics rendering

#include "Engine10\Graphics Structures\RGBColor.cpp"
#include "Engine10\Graphics Structures\ColorSpaces.cpp"
#include "Engine10\Graphics Structures\Bitmap.cpp"
#include "Engine10\Graphics Structures\ImageCompressor.cpp"
#include "Engine10\Graphics Structures\ZBuffer.cpp"
#include "Engine10\Graphics Structures\MatrixController.cpp"
#include "Engine10\Graphics Structures\Camera.cpp"
#include "Engine10\Graphics Structures\PrimitiveRender.cpp"
#include "Engine10\Graphics Structures\MeshVertex.cpp"
#include "Engine10\Graphics Structures\Clipper.cpp"
#include "Engine10\Graphics Structures\Viewport.cpp"
#include "Engine10\Graphics Structures\KDTree3.cpp"
#include "Engine10\Graphics Structures\KDTreeN.cpp"
#include "Engine10\Graphics Structures\KDTreeNCloseQuery.cpp"
#include "Engine10\Graphics Structures\PointSet.cpp"
#include "Engine10\Advanced Meshes\PoissonReconstructionHelper.cpp"
#include "Engine10\Graphics Structures\PoissonReconstructor.cpp"

/****************************/
/*	   Mesh Structures		*/
/****************************/
//These files all center around creating and manipulating triangle meshes.
//These meshes are "simple" in that they contain no adjaceny information.

#include "Engine10\Base Mesh\HTable.cpp"
#include "Engine10\Base Mesh\BaseMesh.cpp"

#include "Engine10\Advanced Meshes\D3D9Mesh.cpp"
#include "Engine10\Advanced Meshes\D3D10Mesh.cpp"
#include "Engine10\Advanced Meshes\Mesh.cpp"

/************************************/
/*	   Subdivision Structures		*/
/************************************/
//These files all center around edge-based mesh structures and an implementation
//of the loop subdivision algorithm that relies upon an edge-based structure.

#include "Engine10\Subdivision\Triangle.cpp"
#include "Engine10\Subdivision\FullEdge.cpp"
#include "Engine10\Subdivision\Vertex.cpp"
#include "Engine10\Subdivision\EdgeMesh.cpp"

/****************************/
/*	 DirectX Objects    	*/
/****************************/
//Objects that encapsulate shader and render-to-texture functionality in DirectX

#include "Engine10\DXObjects\D3D9RenderTargetTexture.cpp"
#include "Engine10\DXObjects\D3D9RenderTargetSurface.cpp"
#include "Engine10\DXObjects\VDecleration9.cpp"
#include "Engine10\DXObjects\PShader9.cpp"
#include "Engine10\DXObjects\VShader9.cpp"
#include "Engine10\DXObjects\D3D9SafeMesh.cpp"
#include "Engine10\DXObjects\D3D10Effect.cpp"

/****************************/
/*		   Textures			*/
/****************************/
//These files all control rendering textured polygons through DirectX and OpenGL.

#include "Engine10\Textures\BaseTexture.cpp"
#include "Engine10\Textures\D3D9Texture.cpp"
#include "Engine10\Textures\D3D10Texture.cpp"
#include "Engine10\Textures\OpenGLTexture.cpp"
#include "Engine10\Textures\Texture.cpp"

/****************************/
/*	   Graphics Devices		*/
/****************************/
//A graphics device controls the interaction between Windows and a graphics API.

#include "Engine10\Graphics Devices\GraphicsDevice.cpp"
#include "Engine10\Graphics Devices\D3D9GraphicsDevice.cpp"
#include "Engine10\Graphics Devices\D3D10GraphicsDevice.cpp"
#include "Engine10\Graphics Devices\OpenGLGraphicsDevice.cpp"
#include "Engine10\Graphics Devices\SoftwareGraphicsDevice.cpp"

/****************************/
/*	  Audio		            */
/****************************/
// Handles all things sound-related
//

#include "Engine10\Audio\AudioDevice.cpp"
#include "Engine10\Audio\WaveFile.cpp"
#include "Engine10\Audio\WaveFileReader.cpp"

/****************************/
/*	  Utility Objects		*/
/****************************/
// These simplify access to existing classes
//

#include "Engine10\Utility Objects\MovieExporter.cpp"
#include "Engine10\Utility Objects\XFileRenderer.cpp"
#include "Engine10\Utility Objects\Indicator.cpp"
#include "Engine10\Utility Objects\ParameterFile.cpp"
#include "Engine10\Utility Objects\UVAtlas.cpp"
#include "Engine10\Utility Objects\FrameTimer.cpp"
#include "Engine10\Utility Objects\Profiler.cpp"
#include "Engine10\Utility Objects\FileCollection.cpp"
#include "Engine10\Utility Objects\MenuInterface.cpp"
#include "Engine10\Utility Objects\DialogInterface.cpp"

/****************************/
/*	 Windows Controllers	*/
/****************************/
//These files handle the interaction between the application and Windows.  This
//includes files that contain WinMain and create the main application window.
//There is also a frame timer and a screen capture mechanism.

#include "Engine10\Windows Controller\InputManager.cpp"
#include "Engine10\Windows Controller\EventHandler.cpp"
#include "Engine10\Windows Controller\WindowManager.cpp"
#include "Engine10\Windows Controller\WindowObjects.cpp"
#include "Engine10\Windows Controller\WinMain.cpp"
