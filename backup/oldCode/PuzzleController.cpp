#include "Main.h"

void PuzzleMessageBox(const String &Message, const String &Title, Game &G, WindowManager &Window)
{
	MessageBox(Window.GetHWND(), Message.CString(), Title.CString(), MB_OK);
}

enum PuzzleMenuType
{
	PuzzleMenuFileMenu,
    PuzzleMenuFileEnterPuzzleDesigner,
    PuzzleMenuFileSeperator0,
	PuzzleMenuFileOpenPuzzleForEditing,
    PuzzleMenuFileOpenPuzzleForPlaying,
    PuzzleMenuFileSavePuzzle,
    PuzzleMenuFileExitPuzzleDesigner,
	PuzzleMenuFileVerifyPuzzleList,
    PuzzleMenuFileCompilePuzzleList,
	PuzzleMenuFileSeperator1,
	PuzzleMenuFileQuit,

	PuzzleMenuPuzzleMenu,
	PuzzleMenuPuzzlePause,
	PuzzleMenuPuzzleGo,
	PuzzleMenuPuzzleStop,
	PuzzleMenuPuzzleSeperator0,
	PuzzleMenuPuzzleClear,
	PuzzleMenuPuzzleSeperator1,
    PuzzleMenuPuzzlePrevPuzzle,
	PuzzleMenuPuzzleNextPuzzle,
	PuzzleMenuPuzzleSeperator2,
	PuzzleMenuPuzzleSeeProvidedSolution,
	PuzzleMenuPuzzleSeeYourSolution,
    //PuzzleMenuPuzzleLoadSavedProgress,
	
	PuzzleMenuOptionsMenu,
    PuzzleMenuOptionsSound,
    PuzzleMenuOptionsMusic,
	PuzzleMenuOptionsAutoAdvanceLevels,
    PuzzleMenuOptionsShowFPS,
	PuzzleMenuOptionsToolTip,
	PuzzleMenuOptionsToolTipImmediate,
	PuzzleMenuOptionsToolTipDelay,
	PuzzleMenuOptionsToolTipNever,

	PuzzleMenuLevelSelectMenu,
	PuzzleMenuLevelSelectCategoryBase = 500,
	PuzzleMenuLevelSelectBase = 1000,
	PuzzleMenuLevelSelectMax = 1999,

	/*PuzzleMenuProvidedSolutionMenu,
	PuzzleMenuProvidedSolutionCategoryBase = 1500,
	PuzzleMenuProvidedSolutionSelectBase = 2000,
	PuzzleMenuProvidedSolutionSelectMax = 2999, 

	PuzzleMenuYourSolutionMenu,
	PuzzleMenuYourSolutionCategoryBase = 3500,
	PuzzleMenuYourSolutionSelectBase = 4000,
	PuzzleMenuYourSolutionSelectMax = 4999, */
};

/*UINT PuzzleInfoCountFromString(const String &Word)
{
	if(Word == "Y")
	{
		return 100000;
	}
	if(Word == "N")
	{
		return 0;
	}
	return Word.ConvertToInteger();
}*/

PuzzleInfo::PuzzleInfo(const String &DatabaseLine)
{
	Vector<String> Words;
	DatabaseLine.Partition('\t', Words);
	UINT CurWordIndex = 0;
	Category = Words[CurWordIndex++];
	Name = Words[CurWordIndex++];
	
	PuzzleName = Words[CurWordIndex++];
	PuzzleFile = String("UncompiledLevels\\") + PuzzleName;
	SolutionName = Words[CurWordIndex++];
	SolutionFile = String("UncompiledLevels\\") + SolutionName;
	
	DefinitionFile = String("UncompiledLevels\\") + Words[CurWordIndex++];
	ReminderText = Words[CurWordIndex++];
	StartTime = Words[CurWordIndex++].ConvertToUnsignedInteger();
	EndTime = Words[CurWordIndex++].ConvertToUnsignedInteger();
	AllowPreference = (Words[CurWordIndex++] == "Y");
	for(UINT ChargeLevel = 0; ChargeLevel < MaxChargeLevel; ChargeLevel++)
	{
		MinChargeCount[ChargeLevel] = Words[CurWordIndex++].ConvertToInteger();
		if(Words[CurWordIndex] == "Infinity")
		{
			MaxChargeCount[ChargeLevel] = 100000;
			CurWordIndex++;
		}
		else
		{
			MaxChargeCount[ChargeLevel] = Words[CurWordIndex++].ConvertToInteger();
		}
	}
}

String PuzzleController::GetReminderText()
{
	if(_CurPuzzleIndex < _Puzzles.Length())
	{
		return _Puzzles[_CurPuzzleIndex].ReminderText;
	}
	else
	{
		return "Now in edit mode";
	}
}

void PuzzleController::Init(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options, InputManager &Input)
{
    if(Options.PlayMusic)
    {
        G.Audio().Audio.PlayWaveFile(G.Audio().MusicSound, true);
    }
    G.Audio().PlaySounds = (Options.PlaySounds == 1);

    _CurPuzzleIndex = 0;
	_CurPuzzleType = PuzzleAttemptGame;
	_VerifyingPuzzleList = false;
    _GodPresent = Utility::FileExists("God.txt");
    //_GodPresent = false;

	_Menu.Init(Window);
	Menu &FileMenu = _Menu.MenuBar().AddSubMenu(Window, PuzzleMenuFileMenu, "File");
	Menu &PuzzleMenu = _Menu.MenuBar().AddSubMenu(Window, PuzzleMenuPuzzleMenu, "Puzzle");
	
    FileMenu.AddString(Window, PuzzleMenuFileEnterPuzzleDesigner, "Enter Puzzle Designer");
    FileMenu.AddSeperator(Window, PuzzleMenuFileSeperator0);
	FileMenu.AddString(Window, PuzzleMenuFileOpenPuzzleForEditing, "Open Puzzle For Editing");
    FileMenu.AddString(Window, PuzzleMenuFileOpenPuzzleForPlaying, "Open Puzzle For Playing");
    FileMenu.AddString(Window, PuzzleMenuFileSavePuzzle, "Save Puzzle");
    FileMenu.AddString(Window, PuzzleMenuFileExitPuzzleDesigner, "Exit Puzzle Designer");
    
    if(_GodPresent)
	{
        FileMenu.AddString(Window, PuzzleMenuFileVerifyPuzzleList, "Verify Puzzle List");
        FileMenu.AddString(Window, PuzzleMenuFileCompilePuzzleList, "Compile Puzzle List");
	}
    FileMenu.AddSeperator(Window, PuzzleMenuFileSeperator1);
	FileMenu.AddString(Window, PuzzleMenuFileQuit, "Quit\tAlt+F4");

	PuzzleMenu.AddString(Window, PuzzleMenuPuzzleGo, "Go\tEnter");
	PuzzleMenu.AddString(Window, PuzzleMenuPuzzleStop, "Stop\tEnd");
	PuzzleMenu.AddString(Window, PuzzleMenuPuzzlePause, "Pause\tPause or F12");
	PuzzleMenu.AddSeperator(Window, PuzzleMenuPuzzleSeperator0);
	PuzzleMenu.AddString(Window, PuzzleMenuPuzzleClear, "Clear");
	PuzzleMenu.AddSeperator(Window, PuzzleMenuPuzzleSeperator1);
    PuzzleMenu.AddString(Window, PuzzleMenuPuzzlePrevPuzzle, "Previous Puzzle\tLeft");
	PuzzleMenu.AddString(Window, PuzzleMenuPuzzleNextPuzzle, "Next Puzzle\tRight");
	PuzzleMenu.AddSeperator(Window, PuzzleMenuPuzzleSeperator2);
	PuzzleMenu.AddString(Window, PuzzleMenuPuzzleSeeProvidedSolution, "See Provided Solution\tUp");
	PuzzleMenu.AddString(Window, PuzzleMenuPuzzleSeeYourSolution, "See Your Solution\tDown");
    //PuzzleMenu.AddString(Window, PuzzleMenuPuzzleLoadSavedProgress, "Load Saved Progress\tUp");

    LoadPuzzleListFromDataFile(G, UI, Window, Options);
}

void PuzzleController::LoadPuzzleListFromDataFile(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
	if(_Puzzles.Length() == 0)
	{
        if(!Utility::FileExists("Puzzles.dat"))
        {
            PuzzleMessageBox("Puzzle data file, Puzzles.dat, not found", "Error", G, Window);
            return;
        }

        G.StopPuzzle();
        G.SetPuzzleMode();
        
        _Collection.LoadAll("Puzzles.dat");
        
        Vector<String> Lines;
        _Collection.GetFileLines("UncompiledLevels\\PuzzleList.txt", Lines);
	    for(UINT Index = 0; Index < Lines.Length(); Index++)
	    {
		    if(Lines[Index].Length() > 5)
		    {
			    _Puzzles.PushEnd(PuzzleInfo(Lines[Index]));
		    }
	    }

	    Menu &OptionsMenu = _Menu.MenuBar().AddSubMenu(Window, PuzzleMenuOptionsMenu, "Options");
	    OptionsMenu.AddString(Window, PuzzleMenuOptionsSound, "Sound");
        OptionsMenu.AddString(Window, PuzzleMenuOptionsMusic, "Music");
        OptionsMenu.AddString(Window, PuzzleMenuOptionsAutoAdvanceLevels, "Auto-advance");
        OptionsMenu.AddString(Window, PuzzleMenuOptionsShowFPS, "Show FPS");
	    Menu &TooltipMenu = OptionsMenu.AddSubMenu(Window, PuzzleMenuOptionsToolTip, "Tooltips");
	    TooltipMenu.AddString(Window, PuzzleMenuOptionsToolTipImmediate, "Immediate");
	    TooltipMenu.AddString(Window, PuzzleMenuOptionsToolTipDelay, "Delay");
	    TooltipMenu.AddString(Window, PuzzleMenuOptionsToolTipNever, "Never");

	    Menu &LevelMenu = _Menu.MenuBar().AddSubMenu(Window, PuzzleMenuLevelSelectMenu, "Level Select");
	    //Menu &ProvidedSolutionMenu = _Menu.MenuBar().AddSubMenu(Window, PuzzleMenuProvidedSolutionMenu, "Provided Solutions");
	    //Menu &YourSolutionMenu = _Menu.MenuBar().AddSubMenu(Window, PuzzleMenuYourSolutionMenu, "Your Solutions");

	    UINT CurPuzzleCategoryIndex = 0;
	    for(UINT PuzzleIndex = 0; PuzzleIndex < _Puzzles.Length(); PuzzleIndex++)
	    {
		    PuzzleInfo &CurPuzzle = _Puzzles[PuzzleIndex];
		    if(!LevelMenu.HasSubMenu(CurPuzzle.Category))
		    {
			    LevelMenu.AddSubMenu(Window, PuzzleMenuLevelSelectCategoryBase + CurPuzzleCategoryIndex, CurPuzzle.Category);
			    //ProvidedSolutionMenu.AddSubMenu(Window, PuzzleMenuProvidedSolutionCategoryBase + CurPuzzleCategoryIndex, CurPuzzle.Category);
			    //YourSolutionMenu.AddSubMenu(Window, PuzzleMenuYourSolutionCategoryBase + CurPuzzleCategoryIndex, CurPuzzle.Category);
			    CurPuzzleCategoryIndex++;
		    }

		    LevelMenu.GetSubMenu(CurPuzzle.Category).AddString(Window, PuzzleMenuLevelSelectBase + PuzzleIndex, CurPuzzle.Name);
		    //ProvidedSolutionMenu.GetSubMenu(CurPuzzle.Category).AddString(Window, PuzzleMenuProvidedSolutionSelectBase + PuzzleIndex, CurPuzzle.Name);
		    //YourSolutionMenu.GetSubMenu(CurPuzzle.Category).AddString(Window, PuzzleMenuYourSolutionSelectBase + PuzzleIndex, CurPuzzle.Name);
	    }
    }

    _CurPuzzleIndex = Math::Min(Options.HighestPuzzleIndexSolved + 1, int(_Puzzles.Length()) - 1);

    if(_GodPresent)
    {
        Options.HighestPuzzleIndexSolved = _Puzzles.Length() - 1;
    }

	LoadPuzzleBase(G, UI, Window, Options);
}

void PuzzleController::UpdatePuzzleMenus(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
    Menu &FileMenu = _Menu.MenuBar().GetSubMenu("File");
    Menu &OptionsMenu = _Menu.MenuBar().GetSubMenu("Options");
	Menu &TooltipMenu = OptionsMenu.GetSubMenu("Tooltips");

    if(_CurPuzzleType == PuzzleDesign)
    {
        FileMenu.SetItemState(Window, PuzzleMenuFileEnterPuzzleDesigner, false, false);

        FileMenu.SetItemState(Window, PuzzleMenuFileOpenPuzzleForEditing, true, false);
        FileMenu.SetItemState(Window, PuzzleMenuFileOpenPuzzleForPlaying, true, false);
        FileMenu.SetItemState(Window, PuzzleMenuFileSavePuzzle, true, false);
        FileMenu.SetItemState(Window, PuzzleMenuFileExitPuzzleDesigner, true, false);
    }
    else
    {
        FileMenu.SetItemState(Window, PuzzleMenuFileEnterPuzzleDesigner, true, false);

        FileMenu.SetItemState(Window, PuzzleMenuFileOpenPuzzleForEditing, false, false);
        FileMenu.SetItemState(Window, PuzzleMenuFileOpenPuzzleForPlaying, false, false);
        FileMenu.SetItemState(Window, PuzzleMenuFileSavePuzzle, false, false);
        FileMenu.SetItemState(Window, PuzzleMenuFileExitPuzzleDesigner, false, false);
    }
	if(_Puzzles.Length() > 0)
	{
		OptionsMenu.SetItemState(Window, PuzzleMenuOptionsAutoAdvanceLevels, true, Options.AutoAdvanceLevels == 1);
        OptionsMenu.SetItemState(Window, PuzzleMenuOptionsSound, true, Options.PlaySounds == 1);
        OptionsMenu.SetItemState(Window, PuzzleMenuOptionsMusic, true, Options.PlayMusic == 1);
        OptionsMenu.SetItemState(Window, PuzzleMenuOptionsShowFPS, true, Options.ShowFPS == 1);
		TooltipMenu.SetItemState(Window, PuzzleMenuOptionsToolTipImmediate, true, Options.ToolTip == ToolTipBehaviorImmediate);
		TooltipMenu.SetItemState(Window, PuzzleMenuOptionsToolTipDelay, true, Options.ToolTip == ToolTipBehavior1s);
		TooltipMenu.SetItemState(Window, PuzzleMenuOptionsToolTipNever, true, Options.ToolTip == ToolTipBehaviorNone);

        Menu &PuzzleMenu = _Menu.MenuBar().GetSubMenu("Puzzle");
        PuzzleMenu.SetItemState(Window, PuzzleMenuPuzzlePause, true, Options.Paused);

		Menu &LevelMenu = _Menu.MenuBar().GetSubMenu("Level Select");
		//Menu &ProvidedSolutionMenu = _Menu.MenuBar().GetSubMenu("Provided Solutions");
		//Menu &YourSolutionMenu = _Menu.MenuBar().GetSubMenu("Your Solutions");

		for(UINT PuzzleIndex = 0; PuzzleIndex < _Puzzles.Length(); PuzzleIndex++)
		{
			PuzzleInfo &CurPuzzle = _Puzzles[PuzzleIndex];
            bool LevelEnabled = Options.PuzzleAttemptable(PuzzleIndex);
            bool SolutionEnabled = Options.PuzzleSolutionViewable(PuzzleIndex);
            if(Options.PuzzleSkipped(PuzzleIndex))
            {
                LevelMenu.GetSubMenu(CurPuzzle.Category).SetItemName(Window, PuzzleMenuLevelSelectBase + PuzzleIndex, CurPuzzle.Name + " (skipped)");
            }
            else
            {
                LevelMenu.GetSubMenu(CurPuzzle.Category).SetItemName(Window, PuzzleMenuLevelSelectBase + PuzzleIndex, CurPuzzle.Name);
            }
			LevelMenu.GetSubMenu(CurPuzzle.Category).SetItemState(Window, PuzzleMenuLevelSelectBase + PuzzleIndex, LevelEnabled, PuzzleIndex == _CurPuzzleIndex);
			//ProvidedSolutionMenu.GetSubMenu(CurPuzzle.Category).SetItemState(Window, PuzzleMenuProvidedSolutionSelectBase + PuzzleIndex, SolutionEnabled, PuzzleIndex == _CurPuzzleIndex);
			//YourSolutionMenu.GetSubMenu(CurPuzzle.Category).SetItemState(Window, PuzzleMenuYourSolutionSelectBase + PuzzleIndex, SolutionEnabled, PuzzleIndex == _CurPuzzleIndex);
		}
	}
}

void PuzzleController::LoadPuzzleBase(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
	PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];

    Vector<String> Lines;
    _Collection.GetFileLines(CurPuzzle.PuzzleFile, Lines);
    G.LoadFromPuzzleFile(Lines, false, NULL);

    _Collection.GetFileLines(CurPuzzle.DefinitionFile, Lines);
	G.UpdateBuildingDatabase(Lines, false, CurPuzzle.AllowPreference);

	_CurPuzzleType = PuzzleAttemptGame;

    String WindowName = "Charge! - " + CurPuzzle.Name;
    if(_GodPresent)
    {
        WindowName += String(" (") + CurPuzzle.PuzzleName + String(")");
    }
	Window.RenameWindow(WindowName);

	UpdateCurrentPuzzle(G, UI, Window, Options);
}

void PuzzleController::LoadPuzzleProvidedSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
	PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];

    Vector<String> Lines;
    _Collection.GetFileLines(CurPuzzle.SolutionFile, Lines);
	G.LoadFromPuzzleFile(Lines, false, NULL);

    _Collection.GetFileLines(CurPuzzle.DefinitionFile, Lines);
	G.UpdateBuildingDatabase(Lines, false, CurPuzzle.AllowPreference);

	_CurPuzzleType = PuzzleProvidedSolution;

	Window.RenameWindow("Charge! - " + CurPuzzle.Name + ", provided solution");

	UpdateCurrentPuzzle(G, UI, Window, Options);
}

void PuzzleController::LoadPuzzleYourSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
	PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];
	String Filename = "YourSolutions\\Your" + CurPuzzle.SolutionName;
	if(Utility::FileExists(Filename))
	{
        Vector<String> Lines;
        Utility::GetFileLines(Filename, Lines);
        G.LoadFromPuzzleFile(Lines, false, NULL);
        
        _Collection.GetFileLines(CurPuzzle.DefinitionFile, Lines);
		G.UpdateBuildingDatabase(Lines, false, CurPuzzle.AllowPreference);

		_CurPuzzleType = PuzzleAttemptGame;

		Window.RenameWindow("Charge! - " + CurPuzzle.Name + ", your solution");

		UpdateCurrentPuzzle(G, UI, Window, Options);
	}
	else
	{
		//PuzzleMessageBox("Cannot find your solution file for this level, most likely because you have not yet solved it.  Loading any saved progress you have instead.", " ", G, Window);
        //LoadSavedProgress(G, UI, Window, Options);
        PuzzleMessageBox("Cannot find your solution file for this level, most likely because you have not yet solved it.", " ", G, Window);
	}
}

void PuzzleController::UpdateCurrentPuzzle(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
	UpdatePuzzleMenus(G, UI, Window, Options);
	UI.Reset(G);
}

void PuzzleController::NextPuzzle(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
    //SaveCurrentProgress(G, UI, Window, Options);
    if(_Puzzles.Length() > 0 && _CurPuzzleIndex != _Puzzles.Length() - 1)
	{
        if(Options.PuzzleAttemptable(_CurPuzzleIndex + 1))
		{
			_CurPuzzleIndex++;
			LoadPuzzleBase(G, UI, Window, Options);
		}
		else
		{
			PuzzleMessageBox("Cannot advance to the next puzzle because you have not completed this one.", " ", G, Window);
		}
	}
}

void PuzzleController::PrevPuzzle(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
    //SaveCurrentProgress(G, UI, Window, Options);
    if(_Puzzles.Length() > 0 && _CurPuzzleIndex != 0)
	{
		_CurPuzzleIndex--;
		LoadPuzzleBase(G, UI, Window, Options);
	}
}

void PuzzleController::SeeProvidedSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
	if(_Puzzles.Length() > 0)
	{
        if(Options.PuzzleSolutionViewable(_CurPuzzleIndex))
		{
			LoadPuzzleProvidedSolution(G, UI, Window, Options);
		}
		else
		{
			PuzzleMessageBox("Cannot see the solution until you have solved the puzzle.", " ", G, Window);
		}
	}
}

void PuzzleController::SeeYourSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
	if(_Puzzles.Length() > 0)
	{
		if(Options.PuzzleSolutionViewable(_CurPuzzleIndex))
		{
			LoadPuzzleYourSolution(G, UI, Window, Options);
		}
		else
		{
			PuzzleMessageBox("You have not yet solved this puzzle.", " ", G, Window);
		}
	}
}

/*void PuzzleController::SaveCurrentProgress(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
  if(_Puzzles.Length() > 0 && G.PuzzleModified())
  {
    PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];
    String Filename = "YourProgress\\Your" + CurPuzzle.PuzzleName;
    G.SaveToPuzzleFile(Filename, false);
  }
}

void PuzzleController::LoadSavedProgress(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
  if(_Puzzles.Length() > 0)
	{
		PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];
	  String Filename = "YourProgress\\Your" + CurPuzzle.PuzzleName;
	  if(Utility::FileExists(Filename))
	  {
          Vector<String> Lines;
          Utility::GetFileLines(Filename, Lines);
		  G.LoadFromPuzzleFile(Lines, false, NULL);
          _Collection.GetFileLines(CurPuzzle.DefinitionFile, Lines);
	      G.UpdateBuildingDatabase(Lines, false, CurPuzzle.AllowPreference);

		  _CurPuzzleType = PuzzleAttemptGame;

		  Window.RenameWindow("Charge! - " + CurPuzzle.Name + ", saved progress");

		  UpdateCurrentPuzzle(G, UI, Window, Options);
	  }
	  else
	  {
		  LoadPuzzleBase(G, UI, Window, Options);
	  }
	}
}*/

void PuzzleController::Exit(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
    //SaveCurrentProgress(G, UI, Window, Options);
}

void PuzzleController::CompilePuzzleList(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options)
{
    String Filename = "UncompiledLevels\\PuzzleList.txt";
	if(Utility::FileExists(Filename))
	{
        Vector<String> Lines;
        Vector<BYTE> CurData;
	    
        Utility::GetFileLines(Filename, Lines);
        FileCollection Collection;

        Utility::GetFileData(Filename, CurData);
        Collection.AddFile(Filename, CurData);

	    for(UINT Index = 0; Index < Lines.Length(); Index++)
	    {
		    if(Lines[Index].Length() > 5)
		    {
			    //PuzzlesToCompile.PushEnd(PuzzleInfo(Lines[Index]));
                PuzzleInfo CurInfo(Lines[Index]);
                Utility::GetFileData(CurInfo.PuzzleFile, CurData);
                Collection.AddFile(CurInfo.PuzzleFile, CurData);
                Utility::GetFileData(CurInfo.SolutionFile, CurData);
                Collection.AddFile(CurInfo.SolutionFile, CurData);
                Utility::GetFileData(CurInfo.DefinitionFile, CurData);
                Collection.AddFile(CurInfo.DefinitionFile, CurData);
		    }
	    }
        Collection.SaveAll("Puzzles.dat");
	}
	else
	{
		PuzzleMessageBox(String("File not found: ") + Filename, "Error", G, Window);
	}
}

void PuzzleController::Step(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options, InputManager &Input, FileCollection &Data)
{
	if(G.AttemptingPuzzle())
	{
		UINT MinTime = 0, MaxTime = 1000000;
		if(_Puzzles.Length() > 0)
		{
			PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];
			MinTime = CurPuzzle.StartTime * GameFramesPerSecond;
			MaxTime = CurPuzzle.EndTime * GameFramesPerSecond;
		}
		
        bool SuccessOverride = Input.KeyCheckOnce(KEY_F5) && Input.KeyCheck(KEY_Z) && Input.KeyCheck(KEY_N);
        //bool SuccessOverride = false;
		if(G.CurrentTime() >= MinTime || SuccessOverride)
		{
			bool VictoryCriteriaMet = true;
			if(_Puzzles.Length() > 0)
			{
				PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];
				for(UINT ChargeLevel = 0; ChargeLevel < MaxChargeLevel; ChargeLevel++)
				{
					UINT ChargeCount = G.CountChargesOfLevel(ChargeLevel + 1);
					if(ChargeCount < CurPuzzle.MinChargeCount[ChargeLevel] ||
					   ChargeCount > CurPuzzle.MaxChargeCount[ChargeLevel])
					{
						VictoryCriteriaMet = false;
					}
				}
			}

            if((G.PuzzleSolved() && VictoryCriteriaMet) || SuccessOverride)
			{
				if(!_VerifyingPuzzleList)
				{
					PuzzleMessageBox("Puzzle solved!", " ", G, Window);
				}
				G.StopPuzzle();
				if(_CurPuzzleType == PuzzleAttemptGame || _VerifyingPuzzleList)
				{
                    //SaveCurrentProgress(G, UI, Window, Options);
					PuzzleInfo &CurPuzzle = _Puzzles[_CurPuzzleIndex];
                    if(!_VerifyingPuzzleList)
                    {
					    String Filename = "YourSolutions\\Your" + CurPuzzle.SolutionName;
					    G.SaveToPuzzleFile(Filename, false);
                        Options.RegisterPuzzleSolved(_CurPuzzleIndex);
                        Options.SaveToFile();
                    }
				    if(Options.AutoAdvanceLevels == 1 || _VerifyingPuzzleList)
				    {
					    if(_CurPuzzleIndex == _Puzzles.Length() - 1)
					    {
						    _VerifyingPuzzleList = false;
					    }
					    NextPuzzle(G, UI, Window, Options);
					    if(_VerifyingPuzzleList)
					    {
						    SeeProvidedSolution(G, UI, Window, Options);
						    G.StartPuzzle();
					    }
				    }
                }
				UpdatePuzzleMenus(G, UI, Window, Options);
			}
		}
		if(G.CurrentTime() > MaxTime)
		{
			G.StopPuzzle();
		}
	}

	while(Input.EventPresent())
	{
		InputEvent CurEvent = Input.PopEvent();
		if(CurEvent.Type == InputEventMenu)
		{
			String Filename;
			
			if(CurEvent.Id >= PuzzleMenuLevelSelectBase && CurEvent.Id < PuzzleMenuLevelSelectMax)
			{
				_CurPuzzleIndex = CurEvent.Id - PuzzleMenuLevelSelectBase;
				LoadPuzzleBase(G, UI, Window, Options);
                G.SetPuzzleMode();
			}
			switch(CurEvent.Id)
			{
			case PuzzleMenuFileOpenPuzzleForEditing:
				if(DialogInterface::GetOpenFilename(Filename, "Load puzzle from..."))
				{
					if(Utility::FileExists(Filename))
					{
						_CurPuzzleType = PuzzleDesign;
                        G.ResetBuildingDatabase(Data);
                        Vector<String> Lines;
                        Utility::GetFileLines(Filename, Lines);
						G.LoadFromPuzzleFile(Lines, false, NULL);
						G.SetGodMode();
                        UpdateCurrentPuzzle(G, UI, Window, Options);
					}
				}
				break;
            case PuzzleMenuFileOpenPuzzleForPlaying:
				if(DialogInterface::GetOpenFilename(Filename, "Load puzzle from..."))
				{
					if(Utility::FileExists(Filename))
					{
						_CurPuzzleType = PuzzleDesign;
                        Vector<String> Lines;
                        Utility::GetFileLines(Filename, Lines);
						G.LoadFromPuzzleFile(Lines, true, &Data);
                        G.SetPuzzleMode();
                        UpdateCurrentPuzzle(G, UI, Window, Options);
					}
				}
				break;
			case PuzzleMenuFileExitPuzzleDesigner:
                _CurPuzzleType = PuzzleAttemptGame;
				LoadPuzzleListFromDataFile(G, UI, Window, Options);
                G.SetPuzzleMode();
				break;
            case PuzzleMenuFileEnterPuzzleDesigner:
                {
                    _CurPuzzleType = PuzzleDesign;
                    G.SetGodMode();
                    
                    G.ResetBuildingDatabase(Data);
                    UpdateCurrentPuzzle(G, UI, Window, Options);
                }
				break;
            case PuzzleMenuFileCompilePuzzleList:
				CompilePuzzleList(G, UI, Window, Options);
				break;
			case PuzzleMenuFileVerifyPuzzleList:
				if(_Puzzles.Length() == 0)
                {
				    LoadPuzzleListFromDataFile(G, UI, Window, Options);
                }
				SeeProvidedSolution(G, UI, Window, Options);
				G.StartPuzzle();
				_VerifyingPuzzleList = true;
				break;
			case PuzzleMenuFileSavePuzzle:
				{
					G.StopPuzzle();
					if(DialogInterface::GetSaveFilename(Filename, "Save puzzle to..."))
					{
						G.SaveToPuzzleFile(Filename, true);
					}
				}
				break;
            case PuzzleMenuFileQuit:
                PostQuitMessage(0);
                break;
			case PuzzleMenuPuzzleGo:
				G.StartPuzzle();
				break;
			case PuzzleMenuPuzzleStop:
				G.StopPuzzle();
				break;
			case PuzzleMenuPuzzleClear:
                if(_CurPuzzleType == PuzzleDesign)
                {
                    G.ClearAll();
                    G.ResetBuildingDatabase(Data);
                    UI.Reset(G);
                }
                else
                {
				    LoadPuzzleBase(G, UI, Window, Options);
                }
				break;
			case PuzzleMenuPuzzleNextPuzzle:
				NextPuzzle(G, UI, Window, Options);
				break;
			case PuzzleMenuPuzzlePrevPuzzle:
				PrevPuzzle(G, UI, Window, Options);
				break;
			case PuzzleMenuPuzzleSeeProvidedSolution:
				SeeProvidedSolution(G, UI, Window, Options);
				break;
			case PuzzleMenuPuzzleSeeYourSolution:
				SeeYourSolution(G, UI, Window, Options);
				break;
            /*case PuzzleMenuPuzzleLoadSavedProgress:
                LoadSavedProgress(G, UI, Window, Options);
                break;*/
			case PuzzleMenuPuzzlePause:
				Options.Paused = !Options.Paused;
				break;
			case PuzzleMenuOptionsToolTipImmediate:
				Options.ToolTip = ToolTipBehaviorImmediate;
				break;
			case PuzzleMenuOptionsToolTipDelay:
				Options.ToolTip = ToolTipBehavior1s;
				break;
			case PuzzleMenuOptionsToolTipNever:
				Options.ToolTip = ToolTipBehaviorNone;
				break;
			case PuzzleMenuOptionsAutoAdvanceLevels:
				Options.AutoAdvanceLevels = 1 - Options.AutoAdvanceLevels;
				break;
            case PuzzleMenuOptionsShowFPS:
                Options.ShowFPS = 1 - Options.ShowFPS;
				break;
            case PuzzleMenuOptionsSound:
                Options.PlaySounds = 1 - Options.PlaySounds;
                G.Audio().PlaySounds = (Options.PlaySounds == 1);
                break;
            case PuzzleMenuOptionsMusic:
                Options.PlayMusic = 1 - Options.PlayMusic;
                G.Audio().Audio.ResetAllSounds();
                if(Options.PlayMusic)
                {
                    G.Audio().Audio.PlayWaveFile(G.Audio().MusicSound, true);
                }
                break;
			}
			UpdatePuzzleMenus(G, UI, Window, Options);
		}
        Options.SaveToFile();
	}

    if(Input.KeyCheckOnce(KEY_ENTER))
    {
	    G.StartPuzzle();
    }
    if(_CurPuzzleType != PuzzleDesign)
    {
        if(Input.KeyCheckOnce(KEY_LEFT))
	    {
		    PrevPuzzle(G, UI, Window, Options);
	    }
	    if(Input.KeyCheckOnce(KEY_RIGHT))
	    {
		    NextPuzzle(G, UI, Window, Options);
	    }
	    if(Input.KeyCheckOnce(KEY_UP))
	    {
		    SeeProvidedSolution(G, UI, Window, Options);
	    }
	    if(Input.KeyCheckOnce(KEY_DOWN))
	    {
		    SeeYourSolution(G, UI, Window, Options);
	    }
        /*if(Input.KeyCheckOnce(KEY_UP))
	    {
		    LoadSavedProgress(G, UI, Window, Options);
	    }*/
    }
    if(Input.KeyCheckOnce(KEY_END))
    {
        G.StopPuzzle();
    }
	if(Input.KeyCheckOnce(KEY_PAUSE) || Input.KeyCheckOnce(KEY_F12))
	{
		Options.Paused = !Options.Paused;
		UpdatePuzzleMenus(G, UI, Window, Options);
	}
}
