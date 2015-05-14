#include "Main.h"

void Game::SetPuzzleMode()
{
	_CurMode = GameModePuzzle;
}

void Game::SetGodMode()
{
	_CurMode = GameModeGod;
}

void Game::UpdateBuildingDatabase(const Vector<String> &Lines, bool InitialLoad, bool NewAllowPreferences)
{
	for(UINT Index = 0; Index < BuildingCount; Index++)
	{
		_BuildingInfo[Index].Hotkey = 0;
		_BuildingInfo[Index].MenuCoordinate.x = -1;
		_BuildingInfo[Index].MenuCoordinate.y = -1;
	}
	for(UINT LineIndex = 0; LineIndex < Lines.Length(); LineIndex++)
	{
		const String &CurLine = Lines[LineIndex];
		if(CurLine.Length() > 5)
		{
			BuildingInfo NewInfo;
			NewInfo = BuildingInfo(CurLine);
			BuildingType CurType = GetBuildingTypeFromString(String("Building") + NewInfo.Name);
			if(InitialLoad)
			{
				_DefaultBuildingInfo[CurType] = NewInfo;
			}
			else
			{
				NewInfo.Description = _DefaultBuildingInfo[CurType].Description;
				if(NewInfo.MaxBuildable > 0)
				{
					NewInfo.Hotkey = _DefaultBuildingInfo[CurType].Hotkey;
					NewInfo.MenuCoordinate = _DefaultBuildingInfo[CurType].MenuCoordinate;
				}
				else
				{
					NewInfo.Hotkey = 0;
					NewInfo.MenuCoordinate.x = -1;
				}
			}
			_BuildingInfo[CurType] = NewInfo;
		}
	}
	for(UINT Index = 0; Index < BuildingCount; Index++)
	{
		Assert(_BuildingInfo[Index].Name != "None", String("Database entry missing: ") + GetBuildingTypeString(BuildingType(Index)));
	}
	_AllowPreferences = NewAllowPreferences;
}

void Game::LoadBuildingDatabase(FileCollection &Data)
{
    Vector<String> Lines;
    FileCollectionFile *Result = Data.AddAndUpdateFile("Database.txt", "UncompiledData\\Database.txt");
    Result->GetFileLines(Lines);
	UpdateBuildingDatabase(Lines, true, true);
}

void Game::Init(FileCollection &Data)
{
	LoadBuildingDatabase(Data);
	_Board.Init(24, 24);
	_Buildings.FreeMemory();
	_CurrentTime = 0;
	_PowerSourcesActive = false;
	_AttemptingPuzzle = false;
	SetGodMode();
	_Audio.Init();
    _GameActionTaken = true;
}

void Game::UpgradeBuilding(Building &Bldg, BuildingType NewType)
{
  _GameActionTaken = true;
	if(_AttemptingPuzzle)
	{
		StopPuzzle();
	}
	Bldg.Upgrade(NewType);
}

bool Game::CircuitAtLocation(const Coordinate &GridCoordinate)
{
	Building *Result = BuildingFromLocation(GridCoordinate);
	return (Result != NULL && Result->Type() == BuildingCircuit && Result->Location() == GridCoordinate);
}

Building* Game::CircuitFromLocation(const Coordinate &GridCoordinate)
{
	Building *Result = BuildingFromLocation(GridCoordinate);
	Assert(Result != NULL, "NULL circuit");
	Assert(Result->Type() == BuildingCircuit, "Non-circuit found");
	return Result;
}

Building* Game::BuildingFromLocation(const Coordinate &GridCoordinate)
{
	if(_Board.Cells().ValidCoordinates(GridCoordinate.y, GridCoordinate.x))
	{
		return _Board.Cells().GetElement(GridCoordinate.y, GridCoordinate.x).Bldg;
	}
	else
	{
		return NULL;
	}
}

void Game::AddBuilding(const Coordinate &Location, const Coordinate &ParentCircuit, BuildingType Type, UINT PreferenceLevel, UINT *Fields, BuildingPuzzleType PuzzlePiece, const Coordinate &CircuitCopyLocation)
{
    _GameActionTaken = true;
	if(_AttemptingPuzzle)
	{
		StopPuzzle();
	}
	BuildingType ActualType = Type;
	Building *SourceCircuit = NULL;
	if(Type == BuildingCircuitPaste)
	{
		ActualType = BuildingCircuit;
		SourceCircuit = BuildingFromLocation(CircuitCopyLocation);
        if(SourceCircuit == NULL || SourceCircuit->Type() != BuildingCircuit)
        {
            return;
        }
		if(SourceCircuit != NULL && (SourceCircuit->PuzzlePiece() == BuildingPuzzlePiece || SourceCircuit->PuzzlePiece() == BuildingCopiedCircuit))
		{
			PuzzlePiece = BuildingCopiedCircuit;
		}
	}
	if(ActualType == BuildingCircuit && ParentCircuit != Coordinate::Invalid)
	{
		return;
	}
	Building *NewBuilding = new Building(Location, ParentCircuit, ActualType, PreferenceLevel, Fields, PuzzlePiece);
	_Buildings.PushEnd(NewBuilding);
	if(NewBuilding->OnCircuitBoard())
	{
		Building *CircuitBuilding = CircuitFromLocation(ParentCircuit);
		CircuitBuilding->CircuitBoard().AddBuilding(NewBuilding);
	}
	else
	{
		_Board.AddBuilding(NewBuilding);
		if(ActualType == BuildingCircuit)
		{
			NewBuilding->InitCircuitBoard(*this, true);
			if(Type == BuildingCircuitPaste)
			{
				if(SourceCircuit != NULL && SourceCircuit->Type() == BuildingCircuit)
				{
					if(PuzzlePiece == BuildingCopiedCircuit)
					{
						PuzzlePiece = BuildingPuzzlePiece;
					}
				
					Grid<BoardCell> &CopyCells = SourceCircuit->CircuitBoard().Cells();
					Grid<BoardCell> &PasteCells = NewBuilding->CircuitBoard().Cells();
					for(UINT Row = 0; Row < CopyCells.Rows(); Row++)
					{
						for(UINT Col = 0; Col < CopyCells.Cols(); Col++)
						{
							Building *CopyBuilding = CopyCells.GetElement(Row, Col).Bldg;
							if(CopyBuilding != NULL && CopyBuilding->Location() == Coordinate(Col, Row))
							{
								if(CopyBuilding->IsCircuitBoundary())
								{
									Building *PasteBoundary = PasteCells.GetElement(Row, Col).Bldg;
									Assert(PasteBoundary != NULL && PasteBoundary->IsCircuitBoundary(), "Invalid circuit boundary");
									PasteBoundary->Upgrade(CopyBuilding->Type());
									PasteBoundary->SetChargePreference(CopyBuilding->ChargePreference());
									PasteBoundary->SetPuzzlePiece(PuzzlePiece);
								}
								else
								{
									AddBuilding(CopyBuilding->Location(), Location, CopyBuilding->Type(), CopyBuilding->ChargePreference(),
											    CopyBuilding->Fields(), PuzzlePiece, CircuitCopyLocation);
								}
							}
						}
					}
				}
			}
		}
	}
}

void Game::RemoveBuilding(Building *Bldg)
{
  _GameActionTaken = true;
	if(_AttemptingPuzzle)
	{
		StopPuzzle();
	}
	if(Bldg->Type() == BuildingCircuit)
	{
		for(int ChargeIndex = 0; ChargeIndex < int(_Charges.Length()); ChargeIndex++)
		{
			Charge &CurCharge = *_Charges[ChargeIndex];
			if(CurCharge._Source.Circuit == Bldg->Location() ||
			   CurCharge._Destination.Circuit == Bldg->Location())
			{
				_ExplodingCharges.PushEnd(ExplodingCharge(CurCharge._Source.Location, CurCharge._Source.Circuit, 10 + CurCharge.Level() * 5, CurCharge.Level()));
				_Charges.RemoveSwap(ChargeIndex);
				ChargeIndex--;
			}
		}

		Grid<BoardCell> &Cells = Bldg->CircuitBoard().Cells();
		for(UINT y = 0; y < Cells.Rows(); y++)
		{
			for(UINT x = 0; x < Cells.Cols(); x++)
			{
				Building *CurBuilding = Cells.GetElement(y, x).Bldg;
				if(CurBuilding != NULL)
				{
					RemoveBuilding(CurBuilding);
				}
			}
		}
		Bldg->FreeMemory();
	}
	for(UINT BuildingIndex = 0; BuildingIndex < _Buildings.Length(); BuildingIndex++)
	{
		if(_Buildings[BuildingIndex] == Bldg)
		{
			_Buildings.RemoveSwap(BuildingIndex);
			if(Bldg->OnCircuitBoard())
			{
				Building *ParentCircuit = CircuitFromLocation(Bldg->ParentCircuit());
				ParentCircuit->CircuitBoard().RemoveBuilding(Bldg);
			}
			else
			{
				_Board.RemoveBuilding(Bldg);
			}
			delete Bldg;
			return;
		}
	}
	SignalError("Building not found");
}

/*void Game::DischargeBuilding(Building &Bldg)
{
	if(_AttemptingPuzzle)
	{
		StopPuzzle();
	}
	Bldg.Discharge();
	for(int ChargeIndex = 0; ChargeIndex < int(_Charges.Length()); ChargeIndex++)
	{
		Charge *CurCharge = _Charges[ChargeIndex];
		if(CurCharge->_Destination.Circuit == Bldg.ParentCircuit() &&
			CurCharge->_Destination.Location == Bldg.Location())
		{
			_ExplodingCharges.PushEnd(ExplodingCharge(CurCharge->_Destination.Location, CurCharge->_Destination.Circuit, 10 + CurCharge->Level() * 5, CurCharge->Level()));
			delete CurCharge;
			_Charges.RemoveSwap(ChargeIndex);
			ChargeIndex--;
		}
	}
}*/

void Game::Step(const Coordinate &SelectedCircuitLocation)
{
	//
	// Advance exploding charges
	//
	for(int ExplodingChargeIndex = 0; ExplodingChargeIndex < int(_ExplodingCharges.Length()); ExplodingChargeIndex++)
	{
		ExplodingCharge &CurExplodingCharge = _ExplodingCharges[ExplodingChargeIndex];
		CurExplodingCharge.CurrentFrame++;
		if(CurExplodingCharge.CurrentFrame == CurExplodingCharge.TotalFrames)
		{
			_ExplodingCharges.RemoveSwap(ExplodingChargeIndex);
			ExplodingChargeIndex--;
		}
	}

	//
	// Move charges
	//
	for(int PassIndex = 0; PassIndex < 2; PassIndex++)
	{
		for(int ChargeIndex = 0; ChargeIndex < int(_Charges.Length()); ChargeIndex++)
		{
			Charge &CurCharge = *_Charges[ChargeIndex];
			bool ShowDeathAnimation;
			Building *SourceBuilding = NULL, *DestBuilding = NULL;
			if(!CurCharge.Update(*this, PassIndex == 1, ShowDeathAnimation, SourceBuilding, DestBuilding, SelectedCircuitLocation))
			{
				if(ShowDeathAnimation)
				{
					_ExplodingCharges.PushEnd(ExplodingCharge(CurCharge._Source.Location, CurCharge._Source.Circuit, 10 + CurCharge.Level() * 5, CurCharge.Level()));
					if(SourceBuilding)
					{
						//SourceBuilding->LastChargeVisit() = CurrentTime();
					}
					if(DestBuilding)
					{
						DestBuilding->LastChargeVisit() = CurrentTime();
					}
                    bool PlaySound = DestBuilding != NULL &&
                                    (!DestBuilding->OnCircuitBoard() || DestBuilding->ParentCircuit() == SelectedCircuitLocation);
                    if(PlaySound && _Audio.PlaySounds)
                    {
					    _Audio.Audio.PlayWaveFile(_Audio.ChargeDeathSound);
                    }
				}
				_Charges.RemoveSwap(ChargeIndex);
				ChargeIndex--;
			}
		}
	}

	//
	// Update buildings
	//
	for(UINT BuildingIndex = 0; BuildingIndex < _Buildings.Length(); BuildingIndex++)
	{
		Building &CurBuilding = *_Buildings[BuildingIndex];
		if(_PowerSourcesActive &&
		   CurBuilding.IsPowerSource() &&
		   CurBuilding.TotalChargesRemaining() > 0) // && _CurrentTime - CurBuilding._LastChargeVisit >= ChargeTransitTime * 4 + 2
		{
			if(CurBuilding.TimeUntilEmission() > 0)
			{
				CurBuilding.TimeUntilEmission()--;
			}
			else
			{
				_Charges.PushEnd(new Charge(CurBuilding.Location(), CurBuilding.ParentCircuit(), CurBuilding.Location(), CurBuilding.ParentCircuit(), UINT(CurBuilding.Type()) - UINT(BuildingPowerSourceRed) + 1 ));
				CurBuilding.LastChargeVisit() = _CurrentTime;
				CurBuilding.TimeUntilEmission() = CurBuilding.Fields()[1] * GameFramesPerSecond;
				CurBuilding.TotalChargesRemaining()--;
			}
		}
		if(CurBuilding.IsMegaHold())
		{
			if(CurBuilding.TimeUntilDischarge() > 0)
			{
				CurBuilding.TimeUntilDischarge()--;
			}
			else
			{
				CurBuilding.TimeUntilDischarge() = CurBuilding.Fields()[1];
				CurBuilding.AbsorbedCharge() = Math::Max(0, int(CurBuilding.AbsorbedCharge()) - int(CurBuilding.Fields()[2]));
			}
		}
	}

	_CurrentTime++;
}

UINT Game::GetBuildingIndex(Building *B)
{
	for(UINT Index = 0; Index < _Buildings.Length(); Index++)
	{
		if(_Buildings[Index] == B)
		{
			return Index;
		}
	}
	SignalError("Building not found");
	return 0;
}

void Game::ResetBuildingDatabase(FileCollection &Data)
{
    Vector<String> Lines;
    FileCollectionFile *Result = Data.FindFile("Database.txt");
    Result->GetFileLines(Lines);
	UpdateBuildingDatabase(Lines, true, true);
}

void Game::ClearAll()
{
    StopPuzzle();
    _GameActionTaken = true;
	_AttemptingPuzzle = false;
	_PowerSourcesActive = false;
	_Buildings.FreeMemory();
	_Charges.FreeMemory();
	_ExplodingCharges.FreeMemory();
	_CurrentTime = 0;

    for(UINT Row = 0; Row < _Board.Cells().Rows(); Row++)
	{
		for(UINT Col = 0; Col < _Board.Cells().Cols(); Col++)
		{
			_Board.Cells().GetElement(Row, Col).Bldg = NULL;
		}
	}
}

void Game::SaveToPuzzleFile(const String &Filename, bool SaveAsPuzzle)
{
    StopPuzzle();
	ofstream File(Filename.CString());
	Assert(!File.fail(), "File save failed");

	File << _Buildings.Length() << "\t1" << endl;

    for(UINT Index = 0; Index < BuildingCount; Index++)
    {
        File << String(_BuildingInfo[Index].BuildableInPuzzle) << '\t';
    }
    File << endl;

	for(UINT BuildingIndex = 0; BuildingIndex < _Buildings.Length(); BuildingIndex++)
	{
		Building *CurBuilding = _Buildings[BuildingIndex];
		File << BuildingIndex << '\t';
		File << UINT(CurBuilding->Type()) << '\t';
		File << CurBuilding->Location().x << '\t';
		File << CurBuilding->Location().y << '\t';
		File << CurBuilding->ParentCircuit().x << '\t';
		File << CurBuilding->ParentCircuit().y << '\t';
		File << CurBuilding->ChargePreference() << '\t';
		for(UINT FieldIndex = 0; FieldIndex < MaxFieldIndex; FieldIndex++)
		{
			File << CurBuilding->Fields()[FieldIndex] << '\t';
		}
    if(SaveAsPuzzle)
    {
        File << UINT(BuildingPuzzlePiece) << '\t';
    }
    else
    {
        File << UINT(CurBuilding->PuzzlePiece()) << '\t';
    }
		File << endl;
		//File << CurBuilding->PuzzlePiece() << endl;

		if(CurBuilding->Type() == BuildingCircuit)
		{
			Grid<BoardCell> &Circuit = CurBuilding->CircuitBoard().Cells();
			for(UINT Row = 0; Row < Circuit.Rows(); Row++)
			{
				for(UINT Col = 0; Col < Circuit.Cols(); Col++)
				{
					Building *CurBuilding = Circuit.GetElement(Row, Col).Bldg;
					if(CurBuilding == NULL)
					{
						File << "NULL";
					}
					else
					{
						File << GetBuildingIndex(CurBuilding);
					}
					File << '\t';
				}
				File << endl;
			}
		}
	}
	
	for(UINT Row = 0; Row < _Board.Cells().Rows(); Row++)
	{
		for(UINT Col = 0; Col < _Board.Cells().Cols(); Col++)
		{
			Building *CurBuilding = _Board.Cells().GetElement(Row, Col).Bldg;
			if(CurBuilding == NULL)
			{
				File << "NULL";
			}
			else
			{
				File << GetBuildingIndex(CurBuilding);
			}
			File << '\t';
		}
		File << endl;
	}
}

void Game::LoadFromPuzzleFile(const Vector<String> &Lines, bool UseLocalBuildFile, FileCollection *Data)
{
    ClearAll();

	UINT CurLineIndex = 0;
    Vector<String> Words;
    Lines[CurLineIndex++].Partition('\t', Words);
    UINT CurBuildingCount = Words[0].ConvertToInteger();

    if(Words.Length() >= 2)
    {
        if(Words[1].ConvertToInteger() == 1)
        {
            const String &CurLine = Lines[CurLineIndex++];
            if(UseLocalBuildFile)
            {
                ResetBuildingDatabase(*Data);
            }
            CurLine.Partition('\t', Words);

            for(UINT Index = 0; Index < BuildingCount; Index++)
	        {
                if(UseLocalBuildFile)
                {
                    if(Words[Index].ConvertToBoolean())
	                {
		                _BuildingInfo[Index].Hotkey = _DefaultBuildingInfo[Index].Hotkey;
		                _BuildingInfo[Index].MenuCoordinate = _DefaultBuildingInfo[Index].MenuCoordinate;
	                }
	                else
	                {
		                _BuildingInfo[Index].Hotkey = 0;
		                _BuildingInfo[Index].MenuCoordinate.x = -1;
                        for(UINT BuildingScanIndex = 0; BuildingScanIndex < BuildingCount; BuildingScanIndex++)
                        {
                            BuildingInfo &CurScanInfo = _BuildingInfo[BuildingScanIndex];
                            for(int UpgradeScanIndex = 0; UpgradeScanIndex < int(CurScanInfo.Upgrades.Length()); UpgradeScanIndex++)
                            {
                                if(CurScanInfo.Upgrades[UpgradeScanIndex] == BuildingType(Index))
                                {
                                    CurScanInfo.Upgrades.RemoveSlow(UpgradeScanIndex);
                                    UpgradeScanIndex--;
                                }
                            }
                        }
	                }
                }
                else
                {
                    _BuildingInfo[Index].BuildableInPuzzle = Words[Index].ConvertToBoolean();
                }
	        }
        }
    }

    _BaseBuildingCount = CurBuildingCount;
	_Buildings.Allocate(CurBuildingCount);
	
	for(UINT BuildingIndex = 0; BuildingIndex < _Buildings.Length(); BuildingIndex++)
	{
		_Buildings[BuildingIndex] = new Building;
	}

	for(UINT BuildingIndex = 0; BuildingIndex < _Buildings.Length(); BuildingIndex++)
	{
		Building &CurBuilding = *_Buildings[BuildingIndex];
		const String &CurLine = Lines[CurLineIndex++];
		Vector<String> Words;
		CurLine.Partition('\t', Words);
		Assert(BuildingIndex == Words[0].ConvertToInteger(), "Invalid building index");
		
		BuildingType Type = BuildingType(Words[1].ConvertToInteger());
		Coordinate Location(Words[2].ConvertToInteger(), Words[3].ConvertToInteger());
		Coordinate ParentCircuit(Words[4].ConvertToInteger(), Words[5].ConvertToInteger());
		UINT ChargePreference = Words[6].ConvertToInteger();

        BuildingPuzzleType PuzzleType = BuildingPuzzlePiece;
        if(Words.Length() >= 16)
        {
          PuzzleType = BuildingPuzzleType(Words[15].ConvertToInteger());
        }

        CurBuilding = Building(Location, ParentCircuit, Type, ChargePreference, NULL, PuzzleType);

		for(UINT FieldIndex = 0; FieldIndex < MaxFieldIndex; FieldIndex++)
		{
			CurBuilding.Fields()[FieldIndex] = Words[7 + FieldIndex].ConvertToInteger();
		}

		if(CurBuilding.Type() == BuildingCircuit)
		{
			CurBuilding.InitCircuitBoard(*this, false);
			Grid<BoardCell> &Circuit = CurBuilding.CircuitBoard().Cells();
			for(UINT Row = 0; Row < Circuit.Rows(); Row++)
			{
				const String &CurLine = Lines[CurLineIndex++];
				Vector<String> Words;
				CurLine.Partition('\t', Words);
				for(UINT Col = 0; Col < Circuit.Cols(); Col++)
				{
					if(Words[Col] == "NULL")
					{
						Circuit.GetElement(Row, Col).Bldg = NULL;
					}
					else
					{
						Circuit.GetElement(Row, Col).Bldg = _Buildings[Words[Col].ConvertToInteger()];
					}
				}
			}
		}
	}
	
	for(UINT Row = 0; Row < _Board.Cells().Rows(); Row++)
	{
		const String &CurLine = Lines[CurLineIndex++];
		Vector<String> Words;
		CurLine.Partition('\t', Words);
		for(UINT Col = 0; Col < _Board.Cells().Cols(); Col++)
		{
			if(Words[Col] == "NULL")
			{
				_Board.Cells().GetElement(Row, Col).Bldg = NULL;
			}
			else
			{
				_Board.Cells().GetElement(Row, Col).Bldg = _Buildings[Words[Col].ConvertToInteger()];
			}
		}
	}
}

void Game::ResetAll()
{
    _GameActionTaken = true;
	_CurrentTime = 0;
	/*for(UINT ChargeIndex = 0; ChargeIndex < _Charges.Length(); ChargeIndex++)
	{
		delete _Charges[ChargeIndex];
	}*/
	_Charges.FreeMemory();
	for(UINT BuildingIndex = 0; BuildingIndex < _Buildings.Length(); BuildingIndex++)
	{
		Building &CurBuilding = *_Buildings[BuildingIndex];
		CurBuilding.ResetPuzzle();
		if(CurBuilding.IsPowerSource())
		{
			CurBuilding.TimeUntilEmission() = CurBuilding.Fields()[0] * GameFramesPerSecond;
			CurBuilding.TotalChargesRemaining() = CurBuilding.Fields()[2];
		}
	}
}

void Game::StartPuzzle()
{
  _GameActionTaken = true;
	ResetAll();
	_AttemptingPuzzle = true;
	_PowerSourcesActive = true;
}

void Game::StopPuzzle()
{
  _GameActionTaken = true;
	ResetAll();
	_AttemptingPuzzle = false;
	_PowerSourcesActive = false;
}

bool Game::PuzzleSolved()
{
	//if(_CurrentTime % 60 == 0) Console::WriteLine("Checking...");
	for(UINT BuildingIndex = 0; BuildingIndex < _Buildings.Length(); BuildingIndex++)
	{
		Building &CurBuilding = *_Buildings[BuildingIndex];
		if(CurBuilding.Type() == BuildingRedChargeGoal && CurBuilding.AbsorbedCharge() != 1)
		{
			//if(_CurrentTime % 60 == 0) Console::WriteLine("Building at " + CurBuilding.Location().CommaSperatedString() + " has absorbed charge " + String(CurBuilding.AbsorbedCharge()));
			return false;
		}
		if(CurBuilding.Type() == BuildingOrangeChargeGoal && CurBuilding.AbsorbedCharge() != 2)
		{
			//if(_CurrentTime % 60 == 0) Console::WriteLine("Building at " + CurBuilding.Location().CommaSperatedString() + " has absorbed charge " + String(CurBuilding.AbsorbedCharge()));
			return false;
		}
		if(CurBuilding.Type() == BuildingYellowChargeGoal && CurBuilding.AbsorbedCharge() != 3)
		{
			//if(_CurrentTime % 60 == 0) Console::WriteLine("Building at " + CurBuilding.Location().CommaSperatedString() + " has absorbed charge " + String(CurBuilding.AbsorbedCharge()));
			return false;
		}
		if(CurBuilding.Type() == BuildingGreenChargeGoal && CurBuilding.AbsorbedCharge() != 4)
		{
			//if(_CurrentTime % 60 == 0) Console::WriteLine("Building at " + CurBuilding.Location().CommaSperatedString() + " has absorbed charge " + String(CurBuilding.AbsorbedCharge()));
			return false;
		}
		if(CurBuilding.Type() == BuildingBlueChargeGoal && CurBuilding.AbsorbedCharge() != 5)
		{
			//if(_CurrentTime % 60 == 0) Console::WriteLine("Building at " + CurBuilding.Location().CommaSperatedString() + " has absorbed charge " + String(CurBuilding.AbsorbedCharge()));
			return false;
		}
		if(CurBuilding.IsMegaHold() && CurBuilding.AbsorbedCharge() < CurBuilding.Fields()[0])
		{
			//if(_CurrentTime % 60 == 0) Console::WriteLine("Mega hold at " + CurBuilding.Location().CommaSperatedString() + " has absorbed charge " + String(CurBuilding.AbsorbedCharge()));
			return false;
		}
	}
	//if(_CurrentTime % 60 == 0) Console::WriteLine("Passed Game::PuzzleSolved");
	return true;
}

UINT Game::CountChargesOfLevel(UINT ChargeLevel)
{
	UINT Count = 0;
	for(UINT ChargeIndex = 0; ChargeIndex < _Charges.Length(); ChargeIndex++)
	{
		if(_Charges[ChargeIndex]->Level() == ChargeLevel)
		{
			Count++;
		}
	}
	return Count;
}

bool Game::QueryAndResetGameActionTaken()
{
  bool Result = _GameActionTaken;
  _GameActionTaken = false;
  return Result;
}