#include "Main.h"

const UINT BaseRequiredTooltipHoverTime = 60;

void InputState::Load(InputManager &IM)
{
	MouseScreenPos = Coordinate(int(IM.MousePosInPixels().x), int(IM.MousePosInPixels().y));
	memcpy(KeyPressed, IM.KeyArray(), sizeof(bool) * MaxKeyCount);
	memcpy(MousePressed, IM.MouseButtonPressedArray(), sizeof(bool) * MaxMouseButtonCount);
}

String ClickableObject::GetTooltip(Game &G, Building *CurUpgradeBuilding)
{
	switch(Type)
	{
	case ClickableBuildingMenu:
		{
			BuildingInfo &CurInfo = G.GetBuildingInfo()[BuildingItem];
			String Result = CurInfo.Description;
			Result = Result.FindAndReplace("@", String("(Hotkey ") + String(char((CurInfo.Hotkey - KEY_A) + 'A')) + String(")"));
			Result = Result.FindAndReplace("|", String("\n"));
			return Result;
		}
		break;
	case ClickableUpgradeMenu:
		{
			if(CurUpgradeBuilding == NULL)
			{
				return String();
			}
			else
			{
				BuildingInfo &CurInfo = G.GetBuildingInfo()[CurUpgradeBuilding->Type()];
				if(Index >= CurInfo.Upgrades.Length())
				{
					return String();
				}
				else
				{
					String Result = G.GetBuildingInfo()[CurInfo.Upgrades[Index]].Description;
					String HotkeyReplacement = String("(Upgrade hotkey ") + String(char('1' + Index)) + String(")");
					Result = Result.FindAndReplace("@", HotkeyReplacement);
					Result = Result.FindAndReplace("|", String("\n"));
					return Result;
				}
			}
		}
		break;
	case ClickablePreferenceMenu:
		{
			if(CurUpgradeBuilding == NULL || !CurUpgradeBuilding->CanChangePreferenceLevel() || !G.AllowPreferences())
			{
				return String();
			}
			else
			{
				String Result;
				switch(Index)
				{
				case 4:
					Result = "Major Acceptor @|Making a component a Major Acceptor gives that component an Affinity level of +2.  Charges always prefer moving to components with higher Affinity.";
					break;
				case 3:
					Result = "Minor Acceptor @|Making a component a Minor Acceptor gives that component an Affinity level of +1.  Charges always prefer moving to components with higher Affinity.";
					break;
				case 2:
					Result = "Normal @|Making a component Normal gives that component an Affinity level of 0.  Charges always prefer moving to components with higher Affinity.";
					break;
				case 1:
					Result = "Minor Rejector @|Making a component a Minor Rejector gives that component an Affinity level of -1.  Charges always prefer moving to components with higher Affinity.";
					break;
				case 0:
					Result = "Major Rejector @|Making a component a Major Rejector gives that component an Affinity level of -2.  Charges always prefer moving to components with higher Affinity.";
					break;
				}
				char UpgradeChar = '7' + Index;
				if(Index == 3)
				{
					UpgradeChar = '0';
				}
				if(Index == 4)
				{
					UpgradeChar = '-';
				}
				String HotkeyReplacement = String("(Hotkey ") + String(UpgradeChar) + String(")");
				Result = Result.FindAndReplace("@", HotkeyReplacement);
				Result = Result.FindAndReplace("|", String("\n"));
				return Result;
			}
		}
		break;
	default:
		return "No tooltip info";
	}
}

void GameUI::Reset(Game &G)
{
	_CurState = GameUIStateIdle;
	_SelectedBuildingGridCoordinate = Coordinate::Invalid;
	_SelectedBuildingCircuitCoordinate = Coordinate::Invalid;
	_CopiedCircuitCoordinate = Coordinate::Invalid;
	_MouseInsideCircuit = false;
	_PuzzlePieceSelected = false;
	_ToolTipActive = false;
	
	_Clickables.FreeMemory();
	for(UINT BuildingIndex = 0; BuildingIndex < BuildingCount; BuildingIndex++)
	{
		BuildingInfo &CurInfo = G.GetBuildingInfo()[BuildingIndex];
		if(CurInfo.MenuCoordinate.x != -1)
		{
			_Clickables.PushEnd();
			_Clickables.Last().Type = ClickableBuildingMenu;
			_Clickables.Last().Rect.TopLeft = ThatWhichRendersGames::BuildingMenuToScreenCoordinate(CurInfo.MenuCoordinate);
			_Clickables.Last().Rect.BottomRight = _Clickables.Last().Rect.TopLeft + Coordinate(GameGridDimension * 2, GameGridDimension * 2);
			_Clickables.Last().BuildingItem = BuildingType(BuildingIndex);
		}
	}

	for(UINT UpgradeIndex = 0; UpgradeIndex < 6; UpgradeIndex++)
	{
		_Clickables.PushEnd();
		_Clickables.Last().Type = ClickableUpgradeMenu;
		_Clickables.Last().Rect.TopLeft = ThatWhichRendersGames::UpgradeMenuToScreenCoordinate(UpgradeIndex);
		_Clickables.Last().Rect.BottomRight = _Clickables.Last().Rect.TopLeft + Coordinate(GameGridDimension * 2, GameGridDimension * 2);
		_Clickables.Last().Index = UpgradeIndex;
	}

	for(UINT PreferenceIndex = 0; PreferenceIndex < 5; PreferenceIndex++)
	{
		_Clickables.PushEnd();
		_Clickables.Last().Type = ClickablePreferenceMenu;
		_Clickables.Last().Rect.TopLeft = ThatWhichRendersGames::PreferenceMenuToScreenCoordinate(PreferenceIndex);
		_Clickables.Last().Rect.BottomRight = _Clickables.Last().Rect.TopLeft + Coordinate(GameGridDimension * 2, GameGridDimension * 2);
		_Clickables.Last().Index = PreferenceIndex;
	}
}

Coordinate RemapCoordinateFromActualToExpected(const Coordinate &Coord, const Coordinate &WindowDimension, const Coordinate &ExpectedWindowDimension)
{
	Coordinate Result;
	Result.x = int(Math::LinearMap(0, double(WindowDimension.x), 0, double(ExpectedWindowDimension.x), double(Coord.x)));
	Result.y = int(Math::LinearMap(0, double(WindowDimension.y), 0, double(ExpectedWindowDimension.y), double(Coord.y)));
	return Result;
}

Coordinate RemapCoordinateFromExpectedToActual(const Coordinate &Coord, const Coordinate &WindowDimension, const Coordinate &ExpectedWindowDimension)
{
	Coordinate Result;
	Result.x = int(Math::LinearMap(0, double(ExpectedWindowDimension.x), 0, double(WindowDimension.x), double(Coord.x)));
	Result.y = int(Math::LinearMap(0, double(ExpectedWindowDimension.y), 0, double(WindowDimension.y), double(Coord.y)));
	return Result;
}

Rectangle2 RemapRectangle2FromExpectedToActual(const Rectangle2 &Rect, const Coordinate &WindowDimension, const Coordinate &ExpectedWindowDimension)
{
	Rectangle2 Result;
	Result.Min = RemapCoordinateFromExpectedToActual(Coordinate(Rect.Min), WindowDimension, ExpectedWindowDimension).AsVec2();
	Result.Max = RemapCoordinateFromExpectedToActual(Coordinate(Rect.Max), WindowDimension, ExpectedWindowDimension).AsVec2();
	return Result;
}

void GameUI::Update(Game &G, InputManager &IM, const Coordinate &WindowDimension, const Coordinate &ExpectedWindowDimension, const ChargeOptions &Options)
{
	InputState CurrentInputState;
	CurrentInputState.Load(IM);
	//if(CurrentInputState.MousePressed[LeftMouseButton] && !_PreviousInputState.MousePressed[LeftMouseButton])
    if(CurrentInputState.MousePressed[LeftMouseButton])
    {
        LeftClick(G, RemapCoordinateFromActualToExpected(CurrentInputState.MouseScreenPos, WindowDimension, ExpectedWindowDimension), !_PreviousInputState.MousePressed[LeftMouseButton]);
    }
	//if(CurrentInputState.MousePressed[RightMouseButton] && !_PreviousInputState.MousePressed[RightMouseButton])
    if(CurrentInputState.MousePressed[RightMouseButton])
	{
		RightClick(G, RemapCoordinateFromActualToExpected(CurrentInputState.MouseScreenPos, WindowDimension, ExpectedWindowDimension), !_PreviousInputState.MousePressed[RightMouseButton]);
	}
	for(UINT KeyIndex = 0; KeyIndex < MaxKeyCount; KeyIndex++)
	{
		if(CurrentInputState.KeyPressed[KeyIndex] && !_PreviousInputState.KeyPressed[KeyIndex])
		{
			KeyPress(G, KeyIndex);
		}
	}
	MouseMove(G, RemapCoordinateFromActualToExpected(CurrentInputState.MouseScreenPos, WindowDimension, ExpectedWindowDimension), Options);

	if(!_ToolTipActive && Options.ToolTip != ToolTipBehaviorNone)
	{
		Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
		if(SelectedUpgradeBuilding != NULL &&
		   _PuzzlePieceSelected)
		{
      if(SelectedUpgradeBuilding->OnCircuitBoard() && SelectedUpgradeBuilding->Type() == BuildingMegaHoldAll)
      {
        SelectedUpgradeBuilding = G.CircuitFromLocation(SelectedUpgradeBuilding->ParentCircuit())->CircuitBoard().Cells().GetElement(2, 2).Bldg;
        Assert(SelectedUpgradeBuilding->Type() == BuildingMegaHoldAll, "Mega-hold not found at circuit (2, 2)");
      }
			_ToolTipActive = true;
			_ToolTipBackdrop = false;
			_ToolTipStart = PreferenceMenuTopLeft;

			BuildingInfo &CurInfo = G.GetBuildingInfo()[SelectedUpgradeBuilding->Type()];
			String Result = SelectedUpgradeBuilding->Describe(CurInfo.Description);
			String EmptyString;
			Result = Result.FindAndReplace("@", EmptyString);
			Result = Result.FindAndReplace("|", String("\n"));
			_ToolTipText = Result;
		}
	}

	_PreviousInputState = CurrentInputState;
}

Building* GameUI::GetCopiedCircuitBuilding(Game &G)
{
	if(_CopiedCircuitCoordinate == Coordinate::Invalid)
	{
		return NULL;
	}
	Building *Result = G.BuildingFromLocation(_CopiedCircuitCoordinate);
	if(Result != NULL && Result->Type() == BuildingCircuit)
	{
		return Result;
	}
	else
	{
		return NULL;
	}
}

Building* GameUI::GetSelectedGridBuilding(Game &G)
{
	if(_SelectedBuildingGridCoordinate == Coordinate::Invalid)
	{
		return NULL;
	}
	const Grid<BoardCell> &Cells = G.GetBoard().Cells();
	if(Cells.ValidCoordinates(_SelectedBuildingGridCoordinate.y, _SelectedBuildingGridCoordinate.x))
	{
		const BoardCell &CurCell = Cells.GetElement(_SelectedBuildingGridCoordinate.y, _SelectedBuildingGridCoordinate.x);
    if(G.GetGameMode() == GameModePuzzle && CurCell.Bldg != NULL && CurCell.Bldg->Type() == BuildingBlocker)
    {
      return NULL;
    }
		return CurCell.Bldg;
	}
	return NULL;
}

Building* GameUI::GetSelectedCircuitBuilding(Game &G)
{
	if(_SelectedBuildingGridCoordinate == Coordinate::Invalid)
	{
		return NULL;
	}
	Building *CircuitBuilding = GetSelectedGridBuilding(G);
	if(CircuitBuilding == NULL)
	{
		return NULL;
	}
	if(CircuitBuilding->Type() != BuildingCircuit)
	{
		return NULL;
	}
	const Grid<BoardCell> &Cells = CircuitBuilding->CircuitBoard().Cells();
	if(Cells.ValidCoordinates(_SelectedBuildingCircuitCoordinate.y, _SelectedBuildingCircuitCoordinate.x))
	{
		const BoardCell &CurCell = Cells.GetElement(_SelectedBuildingCircuitCoordinate.y, _SelectedBuildingCircuitCoordinate.x);
		return CurCell.Bldg;
	}
	return NULL;
}

Building* GameUI::GetSelectedUpgradeBuilding(Game &G)
{
	Building *SelectedGridBuilding = GetSelectedGridBuilding(G);
	Building *SelectedCircuitBuilding = GetSelectedCircuitBuilding(G);
	if(SelectedCircuitBuilding != NULL)
	{
		return SelectedCircuitBuilding;
	}
	else
	{
		return SelectedGridBuilding;
	}
}

int GameUI::GetClickableIndex(const Coordinate &ScreenPos)
{
	for(UINT ClickableIndex = 0; ClickableIndex < _Clickables.Length(); ClickableIndex++)
	{
		const ClickableObject &CurClickable = _Clickables[ClickableIndex];
		if(CurClickable.Rect.CoordinateInside(ScreenPos))
		{
			return ClickableIndex;
		}
	}
	return -1;
}

void GameUI::LeftClick(Game &G, const Coordinate &ScreenPos, bool IsTransitionClick)
{
    G.SetGameActionTaken();
	int ClickableIndex = GetClickableIndex(ScreenPos);
	if(ClickableIndex != -1 && IsTransitionClick)
	{
		const ClickableObject &CurClickable = _Clickables[ClickableIndex];
		if(CurClickable.Type == ClickableBuildingMenu)
		{
			_CurState = GameUIStateBuildingMask;
			_CandidateBuilding = CurClickable.BuildingItem;
			return;
		}
		else if(CurClickable.Type == ClickableUpgradeMenu)
		{
			Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
			if(SelectedUpgradeBuilding != NULL && !_PuzzlePieceSelected)
			{
				BuildingInfo &CurInfo = G.GetBuildingInfo()[SelectedUpgradeBuilding->Type()];
				if(CurClickable.Index < CurInfo.Upgrades.Length())
				{
					BuildingType UpgradeTo = CurInfo.Upgrades[CurClickable.Index];
					if(UpgradeTo == BuildingCircuitCopy)
					{
						_CopiedCircuitCoordinate = SelectedUpgradeBuilding->Location();
					}
					else if(UpgradeTo == BuildingCircuitPaste)
					{
						_CurState = GameUIStateBuildingMask;
						_CandidateBuilding = BuildingCircuitPaste;
					}
					else
					{
						G.UpgradeBuilding(*SelectedUpgradeBuilding, UpgradeTo);
					}
				}
			}
			return;
		}
		else if(CurClickable.Type == ClickablePreferenceMenu)
		{
			Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
			if(SelectedUpgradeBuilding != NULL && !_PuzzlePieceSelected)
			{
				BuildingInfo &CurInfo = G.GetBuildingInfo()[SelectedUpgradeBuilding->Type()];
				if(G.AllowPreferences())
				{
					if(G.AttemptingPuzzle())
					{
						G.StopPuzzle();
					}
					SelectedUpgradeBuilding->SetChargePreference(CurClickable.Index);
				}
			}
			return;
		}
	}

	if((_CurState == GameUIStateIdle || _CurState == GameUIStateBuildingMask) && IsTransitionClick)
	{
		if(_MouseInsideCircuit)
		{
			Building *CircuitBuilding = G.CircuitFromLocation(_SelectedBuildingGridCoordinate);
			if(CircuitBuilding->CircuitBoard().Cells().ValidCoordinates(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x))
			{
				const BoardCell &CurCell = CircuitBuilding->CircuitBoard().Cells().GetElement(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x);
				//if(CurCell.Bldg != NULL && CurCell.Bldg->Type() != BuildingCircuitBoundary)
				if(CurCell.Bldg != NULL)
				{
					_CurState = GameUIStateIdle;
					_SelectedBuildingCircuitCoordinate = CurCell.Bldg->Location();
					_PuzzlePieceSelected = CurCell.Bldg->PuzzlePiece() == BuildingPuzzlePiece && G.GetGameMode() == GameModePuzzle;
				}
				else
				{
					_SelectedBuildingCircuitCoordinate = Coordinate::Invalid;
				}
			}
		}
		else
		{
			if(G.GetBoard().Cells().ValidCoordinates(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x))
			{
				const BoardCell &CurCell = G.GetBoard().Cells().GetElement(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x);
				_SelectedBuildingCircuitCoordinate = Coordinate::Invalid;
				if(CurCell.Bldg != NULL)
				{
					_CurState = GameUIStateIdle;
					_SelectedBuildingGridCoordinate = CurCell.Bldg->Location();
					_PuzzlePieceSelected = CurCell.Bldg->PuzzlePiece() == BuildingPuzzlePiece && G.GetGameMode() == GameModePuzzle;
				}
				else
				{
					_SelectedBuildingGridCoordinate = Coordinate::Invalid;
				}
			}
		}
	}
	if(_CurState == GameUIStateBuildingMask)
	{
		if(_MouseInsideCircuit)
		{
			Building *CircuitBuilding = G.CircuitFromLocation(_SelectedBuildingGridCoordinate);
			bool CanBuildingInCircuit = CircuitBuilding->PuzzlePiece() == BuildingUser;

			if(CircuitBuilding->CircuitBoard().BuildingPlacementValid(_CurCoordinateForBuilding) &&
				(G.GetGameMode() == GameModeGod || CanBuildingInCircuit))
			{
				G.AddBuilding(_CurCoordinateForBuilding, _SelectedBuildingGridCoordinate, _CandidateBuilding, 2, NULL, BuildingUser);
				//_CurState = GameUIStateIdle;
			}
		}
		else
		{
			if(G.GetBoard().BuildingPlacementValid(_CurCoordinateForBuilding))
			{
				G.AddBuilding(_CurCoordinateForBuilding, Coordinate::Invalid, _CandidateBuilding, 2, NULL, BuildingUser, _CopiedCircuitCoordinate);
				//_CurState = GameUIStateIdle;
			}
		}
	}
}

void GameUI::DeleteCurrentPiece(Game &G)
{
	_CurState = GameUIStateIdle;
	Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
	if(SelectedUpgradeBuilding != NULL &&
	   !_PuzzlePieceSelected &&
	   !SelectedUpgradeBuilding->IsCircuitBoundary())
	{
		G.RemoveBuilding(SelectedUpgradeBuilding);
		if(_SelectedBuildingCircuitCoordinate != Coordinate::Invalid)
		{
			_SelectedBuildingCircuitCoordinate = Coordinate::Invalid;
		}
		else
		{
			_SelectedBuildingGridCoordinate = Coordinate::Invalid;
		}
	}
}

void GameUI::RightClick(Game &G, const Coordinate &ScreenPos, bool IsTransitionClick)
{
    G.SetGameActionTaken();
    int ClickableIndex = GetClickableIndex(ScreenPos);
    if(ClickableIndex != -1 && G.GetGameMode() == GameModeGod)
	{
		const ClickableObject &CurClickable = _Clickables[ClickableIndex];
        BuildingType Type = BuildingCount;
		if(CurClickable.Type == ClickableBuildingMenu)
		{
            Type = CurClickable.BuildingItem;
		}
		else if(CurClickable.Type == ClickableUpgradeMenu)
		{
			Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
			if(SelectedUpgradeBuilding != NULL && !_PuzzlePieceSelected)
			{
				BuildingInfo &CurInfo = G.GetBuildingInfo()[SelectedUpgradeBuilding->Type()];
				if(CurClickable.Index < CurInfo.Upgrades.Length())
				{
					Type = CurInfo.Upgrades[CurClickable.Index];
				}
			}
		}
		else if(CurClickable.Type == ClickablePreferenceMenu)
		{
			Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
			if(SelectedUpgradeBuilding != NULL && !_PuzzlePieceSelected)
			{
				Type = SelectedUpgradeBuilding->Type();
			}
		}
        if(Type != BuildingCount && Type != BuildingBlocker)
        {
            if(IsTransitionClick)
            {
                G.GetBuildingInfo()[Type].BuildableInPuzzle = !G.GetBuildingInfo()[Type].BuildableInPuzzle;
            }
            return;
        }
	}
	if(_CurState == GameUIStateBuildingMask)
	{
		_CurState = GameUIStateIdle;
	}
	else if(_CurState == GameUIStateIdle)
	{
		if(_SelectedBuildingCircuitCoordinate != Coordinate::Invalid)
		{
			_SelectedBuildingCircuitCoordinate = Coordinate::Invalid;
		}
		else if(!_MouseInsideCircuit)
		{
			_SelectedBuildingGridCoordinate = Coordinate::Invalid;
		}

		if(_MouseInsideCircuit)
		{
			Building *CircuitBuilding = G.CircuitFromLocation(_SelectedBuildingGridCoordinate);
			if(CircuitBuilding->CircuitBoard().Cells().ValidCoordinates(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x))
			{
				const BoardCell &CurCell = CircuitBuilding->CircuitBoard().Cells().GetElement(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x);
				if(CurCell.Bldg != NULL)
				{
					bool IsPuzzlePiece = CurCell.Bldg->PuzzlePiece() == BuildingPuzzlePiece && G.GetGameMode() == GameModePuzzle;
					if(!IsPuzzlePiece)
					{
						_SelectedBuildingCircuitCoordinate = CurCell.Bldg->Location();
						_PuzzlePieceSelected = false;
						DeleteCurrentPiece(G);
					}
				}
			}
		}
		else
		{
			if(G.GetBoard().Cells().ValidCoordinates(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x))
			{
				const BoardCell &CurCell = G.GetBoard().Cells().GetElement(_CurCoordinateForSelection.y, _CurCoordinateForSelection.x);
				_SelectedBuildingCircuitCoordinate = Coordinate::Invalid;
				if(CurCell.Bldg != NULL)
				{
					bool IsPuzzlePiece = CurCell.Bldg->PuzzlePiece() == BuildingPuzzlePiece && G.GetGameMode() == GameModePuzzle;
					if(!IsPuzzlePiece)
					{
						_SelectedBuildingGridCoordinate = CurCell.Bldg->Location();
						_PuzzlePieceSelected = false;
						DeleteCurrentPiece(G);
					}
				}
			}
		}
	}
}

void GameUI::MouseMove(Game &G, const Coordinate &ScreenPos, const ChargeOptions &Options)
{
	Building *CircuitBuilding = G.BuildingFromLocation(_SelectedBuildingGridCoordinate);
	_MouseInsideCircuit = (ScreenPos.x > CircuitTopLeft.x && ScreenPos.y > CircuitTopLeft.y && CircuitBuilding != NULL && CircuitBuilding->Type() == BuildingCircuit);
	if(_MouseInsideCircuit)
	{
		_CurCoordinateForSelection = ThatWhichRendersGames::ScreenCoordinateToCircuitCoordinate(ScreenPos);
		_CurCoordinateForBuilding = ThatWhichRendersGames::ScreenCoordinateToCircuitCoordinate(ScreenPos - Coordinate(GameGridDimension / 2, GameGridDimension / 2));
	}
	else
	{
		_CurCoordinateForSelection = ThatWhichRendersGames::ScreenCoordinateToGridCoordinate(ScreenPos);
		_CurCoordinateForBuilding = ThatWhichRendersGames::ScreenCoordinateToGridCoordinate(ScreenPos - Coordinate(GameGridDimension / 2, GameGridDimension / 2));
	}

	int ClickableIndex = GetClickableIndex(ScreenPos);
	_ToolTipActive = false;
	if(ClickableIndex == -1 || Options.ToolTip == ToolTipBehaviorNone)
	{
		_MouseHoverClickableIndex = -1;
	}
	else
	{
		ClickableObject &CurClickable = _Clickables[ClickableIndex];
		if(_MouseHoverClickableIndex == ClickableIndex)
		{
			UINT RequiredTooltipHoverTime = BaseRequiredTooltipHoverTime;
			if(Options.ToolTip == ToolTipBehaviorImmediate)
			{
				RequiredTooltipHoverTime = 0;
			}
			if(G.CurrentTime() >= _MouseHoverStart + RequiredTooltipHoverTime)
			{
				if(_PuzzlePieceSelected &&
				   (CurClickable.Type == ClickableUpgradeMenu || CurClickable.Type == ClickablePreferenceMenu))
				{

				}
				else
				{
					_ToolTipActive = true;
					_ToolTipBackdrop = true;
					_ToolTipStart = Coordinate(781, CurClickable.Rect.BottomRight.y + 10);
					Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
					_ToolTipText = CurClickable.GetTooltip(G, SelectedUpgradeBuilding);
				}
			}
		}
		else
		{
			_MouseHoverClickableIndex = ClickableIndex;
			_MouseHoverStart = G.CurrentTime();
		}
	}
}

void GameUI::KeyPress(Game &G, UINT Key)
{
  G.SetGameActionTaken();
	PersistentAssert(Key != 0, "Invalid keypress");
	Building *SelectedUpgradeBuilding = GetSelectedUpgradeBuilding(G);
	if(Key == KEY_C)
	{
		Building *SelectedCircuitBuilding = GetSelectedGridBuilding(G);
		if(SelectedCircuitBuilding != NULL)
		{
			_CopiedCircuitCoordinate = SelectedCircuitBuilding->Location();
		}
	}
	//if(Key == KEY_Q || Key == KEY_W || Key == KEY_E || Key == KEY_R || Key == KEY_T)
	if(Key >= KEY_1 && Key <= KEY_6)
	{
		if(SelectedUpgradeBuilding != NULL && !_PuzzlePieceSelected)
		{
			UINT UpgradeIndex = 0;
			if(Key == KEY_2) UpgradeIndex = 1;
			if(Key == KEY_3) UpgradeIndex = 2;
			if(Key == KEY_4) UpgradeIndex = 3;
			if(Key == KEY_5) UpgradeIndex = 4;
			if(Key == KEY_6) UpgradeIndex = 5;
			BuildingInfo &CurInfo = G.GetBuildingInfo()[SelectedUpgradeBuilding->Type()];
			if(UpgradeIndex < CurInfo.Upgrades.Length())
			{
				BuildingType UpgradeTo = CurInfo.Upgrades[UpgradeIndex];
				if(UpgradeTo == BuildingCircuitCopy)
				{
					_CopiedCircuitCoordinate = SelectedUpgradeBuilding->Location();
				}
				else if(UpgradeTo == BuildingCircuitPaste)
				{
					_CurState = GameUIStateBuildingMask;
					_CandidateBuilding = BuildingCircuitPaste;
				}
				else
				{
					G.UpgradeBuilding(*SelectedUpgradeBuilding, UpgradeTo);
				}
			}
		}
	}
	Console::WriteLine(String(Key));
	if(Key == KEY_7 || Key == KEY_8 || Key == KEY_9 || Key == KEY_0 || Key == KEY_SUBTACT)
	{
		if(SelectedUpgradeBuilding != NULL && !_PuzzlePieceSelected)
		{
			UINT PreferenceIndex = 0;
			if(Key == KEY_8) PreferenceIndex = 1;
			if(Key == KEY_9) PreferenceIndex = 2;
			if(Key == KEY_0) PreferenceIndex = 3;
			if(Key == KEY_SUBTACT) PreferenceIndex = 4;
            if(G.AllowPreferences() && SelectedUpgradeBuilding->Type() != BuildingBlocker)
			{
                if(G.AttemptingPuzzle())
				{
					G.StopPuzzle();
				}
				SelectedUpgradeBuilding->SetChargePreference(PreferenceIndex);
			}
		}
	}
	if(_CurState == GameUIStateIdle || _CurState == GameUIStateBuildingMask)
	{
		for(UINT BuildingInfoIndex = 0; BuildingInfoIndex < BuildingCount; BuildingInfoIndex++)
		{
			BuildingInfo &CurInfo = G.GetBuildingInfo()[BuildingInfoIndex];
			if(CurInfo.MaxBuildable > 0 && Key == CurInfo.Hotkey)
			{
				_CurState = GameUIStateBuildingMask;
				_CandidateBuilding = BuildingType(BuildingInfoIndex);
			}
		}
	}
	if(_SelectedBuildingGridCoordinate != Coordinate::Invalid)
	{
		if(Key == KEY_BACKSPACE || Key == KEY_DELETE)
		{
			DeleteCurrentPiece(G);
		}
	}
}
