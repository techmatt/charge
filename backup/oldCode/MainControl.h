/*
MainControl.h
Written by Matthew Fisher

MainControl includes everything that changes often between applications, such as what meshes to load,
and also determines what is rendered each frame.
*/

extern const float SecondsPerGameFrame;
extern const Coordinate ExpectedWindowDimensions;

class MainControl : public FrameController
{
public:
	void FreeMemory(WindowObjects &O);
	void ReInit(WindowObjects &O);	//called after we lose focus, telling us to restore
									//our textures and other data that may have been list
	void Init(WindowObjects &O);	//called only one at the beginning of our application
	void Render(WindowObjects &O);	//called each frame
    void ReleaseObjects(WindowObjects &O);  //called to release local graphics card data

private:
    
	Mesh SphereMesh, SphereMesh2, SphereMesh3, SpokeMesh;

    FileCollection Data;
	Game TheGame;
	GameUI TheGameUI;
	ThatWhichRendersGames ThatWhichRendersTheGame;
	ChargeOptions Options;

	PuzzleController Puzzle;

	UINT SelectedField;

	float Time;
	float LastUpdateTime;
};
