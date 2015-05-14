/*
MainControl.cpp
Written by Matthew Fisher

MainControl includes everything that changes often between applications, such as what meshes to load,
and also determines what is rendered each frame.
*/

//All source files include Main.h
#include "Main.h"

const float SecondsPerGameFrame = 1.0f / 60.0f;
const UINT MaxGameSteps = 3;

const Coordinate ExpectedWindowDimensions(1236, 833);

void MainControl::FreeMemory(WindowObjects &O)
{
	//Effect.FreeMemory();
    Puzzle.Exit(TheGame, TheGameUI, O.GetWindowManager(), Options);
}

void MainControl::ReleaseObjects(WindowObjects &O)
{
    ThatWhichRendersTheGame._Backdrop.FreeMemory();
}

void MainControl::ReInit(WindowObjects &O)
{
    TheGame.SetGameActionTaken();
}

void MainControl::Init(WindowObjects &O)
{
    /*double ShadingRate;
    UINT BinaryDice;
    UINT FixCracks;
    UINT Heuristic;
    UINT Averaging;
    String StatsFile;
    String TestFile;

    {
      ofstream File("ShadingRateTestFrogCracks.cmd");
      StatsFile = "ShadingRateTestFrogCracks.txt";
      for(FixCracks = 0; FixCracks < 2; FixCracks++)
      {
        for(Heuristic = 0; Heuristic < 4; Heuristic++)
        {
          for(Averaging = 0; Averaging < 2; Averaging++)
          {
            for(ShadingRate = 0.5; ShadingRate < 5.0; ShadingRate += 0.25)
            {
                //-b 0 -u 0 -a 1 -e "Stats.txt" -w 1200 -h 1200 -s 3 -o "Output.png" -r 1 W:\reyes\tests\planeTex.txt
                TestFile = "monsterfrogDisp.txt";
                //TestFile = "planeTex.txt";
                BinaryDice = 1;
                File << "render -w 1024 -h 1024 -s 2 -o \"output.png\" -r " << ShadingRate << " -b " << BinaryDice << " -c " << FixCracks << " -u " << Heuristic << " -a " << Averaging << " -e " << StatsFile << " W:\\reyes\\tests\\" << TestFile << endl;
            }
          }
        }
      }
    }
    exit(1);*/
    /*
    printf("Usage: %s [options] <scenefile>\n", binaryName);
    printf("Options:\n");
    printf("  -w, --width   <WIDTH>      Width of rendered image (override scene)\n");
    printf("  -h, --height  <HEIGHT>     Height of rendered image (override scene)\n");
    printf("  -s, --samples <NUM>        NUMxNUM samples per pixel (override scene)\n");
    printf("  -o, --output <FILE>        Output rendered image to <FILENAME> (override scene)\n");
    printf("  -r  --shadingrate <NUM>    REYES target shading rate (override scene)\n");
    printf("  -b  --binarydice <1/0>     Set binary dicing to true or false (override scene)\n");
    printf("  -c  --fixcracks  <1/0>     Set crack fixing to true or false (override scene)\n");
    printf("  -j  --justdicing           Just perform split/dice, no image produced\n");
    printf("  -d  --dump                 Dumps grid db to disk\n");
    printf("  -u  --diceheuristic <INT>  Dice heuristic type\n");
    printf("  -a  --diceavg <1/0>        Controls whether dice heuristic uses average MP area\n");
    printf("  -e  --appendstats <FILE>   Appends statistics to the specified file\n");
    printf("  -?, --help                 This message\n");
    */

	ReInit(O);

	O.GetGraphicsDevice().SetVSync(true);
	O.GetWindowManager().RenameWindow("Charge!");
	O.GetWindowManager().FixedAspectRatio() = double(ExpectedWindowDimensions.y) / double(ExpectedWindowDimensions.x);
	
	Time = 0.0f;
	LastUpdateTime = 0.0f;

	//AllocConsole();
	
    if(Utility::FileExists("Data.dat"))
    {
        Data.LoadAll("Data.dat");
    }

    TheGame.Init(Data);
	TheGameUI.Reset(TheGame);
    
	ThatWhichRendersTheGame.Init(Data, TheGame, O.GetGraphicsDevice());
    
    Data.SaveAll("Data.dat");
	
    O.GetGraphicsDevice().DisableWireframe();
	SelectedField = 0;

    Options.LoadFromFile();

	Puzzle.Init(TheGame, TheGameUI, O.GetWindowManager(), Options, O.GetInputManager());
	
	//
	// Resize must be after the menu is created
	//
    RECT DesktopRect;
    if(GetClientRect(GetDesktopWindow(), &DesktopRect))
    {
        HWND TaskbarHandle = FindWindow("Shell_TrayWnd", NULL);
        RECT TaskbarRect;
        if(TaskbarHandle != NULL && GetWindowRect(TaskbarHandle, &TaskbarRect))
        {
            if(TaskbarRect.right - TaskbarRect.left > TaskbarRect.bottom - TaskbarRect.top)
            {
                DesktopRect.bottom -= (TaskbarRect.bottom - TaskbarRect.top);
            }
            else
            {
                DesktopRect.right -= TaskbarRect.right - TaskbarRect.left;
            }
        }
        
        Vec2 BorderDimensions = O.GetWindowManager().GetBorderDimensions();
        DesktopRect.right -= UINT(BorderDimensions.x);
        DesktopRect.bottom -= UINT(BorderDimensions.y);

        double Factor = 0.5;
        int NewWidth = Math::Round(ExpectedWindowDimensions.x * Factor), NewHeight = Math::Round(ExpectedWindowDimensions.y * Factor);
        bool Done = false;
        for(Factor = 0.5; Factor < 3.0 && !Done; Factor += 0.0001)
        {
            if(NewWidth >= DesktopRect.right || NewHeight >= DesktopRect.bottom)
            {
                Done = true;
            }
            else
            {
                NewWidth = Math::Round(ExpectedWindowDimensions.x * Factor);
                NewHeight = Math::Round(ExpectedWindowDimensions.y * Factor);
            }
        }
        O.GetWindowManager().ReSize(NewWidth, NewHeight);
    }
    else
    {
	    O.GetWindowManager().ReSize(ExpectedWindowDimensions.x * 2 / 3, ExpectedWindowDimensions.y * 2 / 3);
    }

    TheGame.SetGameActionTaken();

	SphereMesh.SetGD(O.GetGraphicsDevice());
	SphereMesh.CreateSphere(200.0f, 5);
	SphereMesh.ApplyMatrix(Matrix4::Scaling(Vec3(1.0f, 1.0f, 0.0f)));
	SphereMesh.Translate(Vec3(512.0f, 512.0f, 0.5f));

	SphereMesh2.SetGD(O.GetGraphicsDevice());
	SphereMesh2.CreateSphere(275.0f, 5);
	SphereMesh2.ApplyMatrix(Matrix4::Scaling(Vec3(1.0f, 1.0f, 0.0f)));
	SphereMesh2.Translate(Vec3(512.0f, 512.0f, 0.4f));

	SphereMesh3.SetGD(O.GetGraphicsDevice());
	SphereMesh3.CreateSphere(250.0f, 5);
	SphereMesh3.ApplyMatrix(Matrix4::Scaling(Vec3(1.0f, 1.0f, 0.0f)));
	SphereMesh3.Translate(Vec3(512.0f, 512.0f, 0.3f));

	SphereMesh2.SetColor(RGBColor::Green);
	SphereMesh3.SetColor(RGBColor::Blue);

	SpokeMesh.SetGD(O.GetGraphicsDevice());
	SpokeMesh.CreatePlane(1.0f, 2, 2);
}

void MainControl::Render(WindowObjects &O)
{
	float FPS = Math::Max(30.0f, O.GetFrameTimer().FPS());
	Time += 1.0f / FPS;

	//if(O.GetInputManager().KeyCheckOnce(KEY_F))
    {
        //O.GetGraphicsDevice().ToggleWireframe();
    }
	Matrix4 InverseViewportMatrix;
	//InverseViewportMatrix = Matrix4::Viewport(float(O.GetWindowManager().GetWidth()), float(O.GetWindowManager().GetHeight())).Inverse();
	InverseViewportMatrix = Matrix4::Viewport(float(ExpectedWindowDimensions.x), float(ExpectedWindowDimensions.y)).Inverse();

	/*Vec3 Result = Transform.TransformPoint(Vec3(float(O.GetWindowManager().GetWidth()), float(O.GetWindowManager().GetHeight()), 0.0f));
	Effect.SetMatrix("WorldViewProjection", Transform);
	Effect.SetTexture2D("DiffuseTexture", Tex);
	
	Effect.Set(O.GetGraphicsDevice());
	SphereMesh.Render();*/

	/*O.GetGraphicsDevice().LoadMatrix(MatrixController(InverseViewportMatrix));
	
	SphereMesh.Render();
	//SphereMesh2.Render();
	//SphereMesh3.Render();

	Vec3 Center(512.0f, 512.0f, 0.5f);
  //for(float Theta = 0.0f; Theta < 2.0f * Math::PIf; Theta += 2.0f * Math::PIf / 14)
  for(float Theta = 0.0f; Theta < 2.0f * Math::PIf; Theta += 2.0f * Math::PIf / 7)
  {
    const float SpokeRadius = 50.0f;
		//const float SpokeLength = 465.0f;
    const float SpokeLength = 265.0f;

		Vec3 Offset = Vec3(SpokeLength * cosf(Theta), SpokeLength * sinf(Theta), 0.0f);
		Offset = Vec3::Normalize(Offset) * SpokeLength;

		GetThickLineVertices(Center, SpokeRadius, Center + Offset, SpokeRadius, SpokeMesh.Vertices()[0].Pos, SpokeMesh.Vertices()[1].Pos, SpokeMesh.Vertices()[2].Pos, SpokeMesh.Vertices()[3].Pos);
		SpokeMesh.Render();
  }

	/*for(int OffsetA = -1; OffsetA <= 1; OffsetA++)
	{
		const float SpokeRadius = 50.0f;
		const float SpokeLength = 1000.0f;

		Vec3 Offset = Vec3(512.0f * OffsetA, 1024.0f, 0.0f);
		Offset = Vec3::Normalize(Offset) * SpokeLength;

		GetThickLineVertices(Center, SpokeRadius, Center + Offset, SpokeRadius, SpokeMesh.Vertices()[0].Pos, SpokeMesh.Vertices()[1].Pos, SpokeMesh.Vertices()[2].Pos, SpokeMesh.Vertices()[3].Pos);
		SpokeMesh.Render();

		Offset = Vec3(512.0f * OffsetA, -1024.0f, 0.0f);
		Offset = Vec3::Normalize(Offset) * SpokeLength;

		GetThickLineVertices(Center, SpokeRadius, Center + Offset, SpokeRadius, SpokeMesh.Vertices()[0].Pos, SpokeMesh.Vertices()[1].Pos, SpokeMesh.Vertices()[2].Pos, SpokeMesh.Vertices()[3].Pos);
		SpokeMesh.Render();

		Offset = Vec3(1024.0f, 512.0f * OffsetA, 0.0f);
		Offset = Vec3::Normalize(Offset) * SpokeLength;

		GetThickLineVertices(Center, SpokeRadius, Center + Offset, SpokeRadius, SpokeMesh.Vertices()[0].Pos, SpokeMesh.Vertices()[1].Pos, SpokeMesh.Vertices()[2].Pos, SpokeMesh.Vertices()[3].Pos);
		SpokeMesh.Render();

		Offset = Vec3(-1024.0f, 512.0f * OffsetA, 0.0f);
		Offset = Vec3::Normalize(Offset) * SpokeLength;

		GetThickLineVertices(Center, SpokeRadius, Center + Offset, SpokeRadius, SpokeMesh.Vertices()[0].Pos, SpokeMesh.Vertices()[1].Pos, SpokeMesh.Vertices()[2].Pos, SpokeMesh.Vertices()[3].Pos);
		SpokeMesh.Render();
	}*/

	UINT GameStepsToTake = 0;
	while(LastUpdateTime + SecondsPerGameFrame < Time)
	{
		if(GameStepsToTake < MaxGameSteps)
		{
			GameStepsToTake++;
		}
		LastUpdateTime += SecondsPerGameFrame;
	}
	if(O.GetInputManager().KeyCheck(KEY_SPACE))
	{
		GameStepsToTake += 5;
	}
	if(O.GetInputManager().KeyCheck(KEY_HOME))
	{
		GameStepsToTake += 50 * Math::Max(int(GameStepsToTake), 1);
	}
	if(Puzzle.VerifyingPuzzleList())
	{
		GameStepsToTake += 100;
	}
	if(Options.Paused)
	{
		GameStepsToTake = 0;
	}
	if(O.GetInputManager().KeyCheckOnce(KEY_NUMPADDIVIDE))
	{
		GameStepsToTake++;
	}
	for(UINT StepIndex = 0; StepIndex < GameStepsToTake; StepIndex++)
	{
        Building *SelectedGridBuilding = TheGameUI.GetSelectedGridBuilding(TheGame);
        Coordinate CircuitLocation = Coordinate::Invalid;
        if(SelectedGridBuilding != NULL && SelectedGridBuilding->Type() == BuildingCircuit)
        {
            CircuitLocation = SelectedGridBuilding->Location();
        }
        TheGame.Step(CircuitLocation);
		Puzzle.Step(TheGame, TheGameUI, O.GetWindowManager(), Options, O.GetInputManager(), Data);
	}
	Puzzle.Step(TheGame, TheGameUI, O.GetWindowManager(), Options, O.GetInputManager(), Data);

	//InverseViewportMatrix = Matrix4::Viewport(float(O.GetWindowManager().GetWidth()), float(O.GetWindowManager().GetHeight())).Inverse();
	//InverseViewportMatrix = Matrix4::Viewport(float(ExpectedWindowDimensions.x), float(ExpectedWindowDimensions.y)).Inverse();

	Coordinate WindowDimensions = Coordinate(O.GetWindowManager().GetWidth(), O.GetWindowManager().GetHeight());
	TheGameUI.Update(TheGame, O.GetInputManager(), WindowDimensions, ExpectedWindowDimensions, Options);
	ThatWhichRendersTheGame.Render(O.GetGraphicsDevice(), TheGame, TheGameUI, InverseViewportMatrix, WindowDimensions, ExpectedWindowDimensions);

	UINT TextYLevel = 1;
	
	O.GetGraphicsDevice().DrawString(Puzzle.GetReminderText(),
    RemapRectangle2FromExpectedToActual(Rectangle2(Vec2(11.0f, 788.0f), Vec2(768.0f, 817.0f)), WindowDimensions, ExpectedWindowDimensions), RGBColor::White);

    if(Options.Paused)
    {
        O.GetGraphicsDevice().DrawString("Game Paused", 0, TextYLevel++ * 20, RGBColor::Black);
    }

	Building *CurBuilding = TheGameUI.GetSelectedUpgradeBuilding(TheGame);
	if(CurBuilding != NULL && TheGame.GetGameMode() == GameModeGod)
	{
		/*O.GetGraphicsDevice().DrawDouble("Current time: ", TheGame.CurrentTime() / float(GameFramesPerSecond), 0, TextYLevel++ * 20);
		O.GetGraphicsDevice().DrawBoolean("In puzzle: ", TheGame.AttemptingPuzzle(), 0, TextYLevel++ * 20);

		O.GetGraphicsDevice().DrawInteger("Absorbed charge: ", CurBuilding->AbsorbedCharge(), 0, TextYLevel++ * 20);
		O.GetGraphicsDevice().DrawInteger("Last visit: ", CurBuilding->LastChargeVisit(), 0, TextYLevel++ * 20);*/
        if(SelectedField >= CurBuilding->GetFieldCount())
        {
            SelectedField = 0;
        }
        if(CurBuilding->GetFieldCount() > 0)
        {
            O.GetGraphicsDevice().DrawString("Select field to modify with numpad +/-", 0, TextYLevel++ * 20, RGBColor::Black);
            O.GetGraphicsDevice().DrawString("Reset current field with numpad *", 0, TextYLevel++ * 20, RGBColor::Black);
            O.GetGraphicsDevice().DrawString("Modify current field with numpad 0-9", 0, TextYLevel++ * 20, RGBColor::Black);

            if(O.GetInputManager().KeyCheckOnce(KEY_NUMPADADD))
	        {
		        SelectedField++;
		        if(SelectedField >= CurBuilding->GetFieldCount())
		        {
			        SelectedField = 0;
		        }
	        }
	        if(O.GetInputManager().KeyCheckOnce(KEY_NUMPADSUBTRACT))
	        {
		        if(SelectedField == 0)
		        {
			        SelectedField = CurBuilding->GetFieldCount() - 1;
		        }
		        else
		        {
			        SelectedField--;
		        }
	        }
        }
		for(UINT Index = 0; Index < CurBuilding->GetFieldCount(); Index++)
		{
			String Prefix;
            RGBColor FieldColor = RGBColor::Red;
			if(SelectedField == Index)
			{
                FieldColor = RGBColor::Blue;
				if(O.GetInputManager().KeyCheckOnce(KEY_NUMPADMULTIPLY))
				{
					CurBuilding->Fields()[Index] = 0;
				}
				for(UINT Number = 0; Number <= 9; Number++)
				{
					if(O.GetInputManager().KeyCheckOnce(KEY_NUMPAD0 + Number))
					{
						CurBuilding->Fields()[Index] = CurBuilding->Fields()[Index] * 10 + Number;
					}
				}
			}
			String Name = CurBuilding->GetFieldName(Index) + String(": ") + String(CurBuilding->Fields()[Index]);
            O.GetGraphicsDevice().DrawString(Name, 0, TextYLevel++ * 20, FieldColor);
		}
	}

    RenderFPS = (Options.ShowFPS == 1);
}
