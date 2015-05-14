#include "Main.h"

Building::Building()
{
	_Type = BuildingCount;
	_CircuitBoard = NULL;
	_ParentCircuit = Coordinate::Invalid;
}

Building::Building(const Coordinate &Location, const Coordinate &ParentCircuit, BuildingType Type, UINT ChargePreference, UINT *Fields, BuildingPuzzleType PuzzlePiece)
{
	_Location = Location;
	_Type = Type;
	_InitialType = Type;
	_LastChargeVisit = -1000;
	_AbsorbedCharge = 0;
	_CircuitBoard = NULL;
	_ParentCircuit = ParentCircuit;
	_ChargePreference = ChargePreference;
	_PuzzlePiece = PuzzlePiece;
	_TimeUntilDischarge = 0;
	
	if(Fields == NULL)
	{
		for(UINT FieldIndex = 0; FieldIndex < MaxFieldIndex; FieldIndex++)
		{
			_Fields[FieldIndex] = 0;
		}

		if(IsPowerSource())
		{
			_Fields[0] = 0;
			_Fields[1] = 1;
			_Fields[2] = 1000;

			_TimeUntilEmission = _Fields[1] * GameFramesPerSecond;
			_TotalChargesRemaining = _Fields[2];
		}

		if(IsHoldGoal())
		{
			_Fields[0] = 100000;
		}

		if(IsMegaHold())
		{
			_Fields[0] = 0;
			_Fields[1] = 5;
			_Fields[2] = 1;
		}
	}
	else
	{
		for(UINT FieldIndex = 0; FieldIndex < MaxFieldIndex; FieldIndex++)
		{
			_Fields[FieldIndex] = Fields[FieldIndex];
		}
	}
}

Building::~Building()
{
	FreeMemory();
}

void Building::FreeMemory()
{
	if(_CircuitBoard)
	{
		delete _CircuitBoard;
		_CircuitBoard = NULL;
	}
}

void Building::ResetPuzzle()
{
	Discharge();
	_Type = _InitialType;
	_LastChargeVisit = -1000;
}

UINT Building::GetFieldCount()
{
	//return 4;
	if(IsPowerSource())
	{
		return 3;
	}
	if(IsHoldGoal())
	{
		return 1;
	}
	if(IsMegaHold())
	{
		return 3;
	}
	return 0;
}

String Building::GetFieldName(UINT Index)
{
	if(Index == BuildingFieldArrowIndex)
	{
		return "Arrow direction";
	}
	if(IsPowerSource())
	{
		switch(Index)
		{
		case 0:
			return "Start time (in seconds)";
		case 1:
			return "Period (in seconds)";
		case 2:
			return "Charge count";
		}
	}
	if(IsHoldGoal())
	{
		switch(Index)
		{
		case 0:
			return "Discharge time (in seconds)";
		}
	}
	if(IsMegaHold())
	{
		switch(Index)
		{
		case 0:
			return "Required charge level";
		case 1:
			return "Frames between discharge";
		case 2:
			return "Charges per discharge";
		}
	}
	return "Invalid";
}

String Building::Describe(const String &BaseDescription)
{
	String ExtraDescription;
	if(IsMegaHold())
	{
		ExtraDescription += "  Current it has absorbed " + String(_AbsorbedCharge) + " units of charge and requires " + String(_Fields[0]) + " units.  It loses " +
							String(Math::Round(float(_Fields[2]) / float(_Fields[1]) / SecondsPerGameFrame)) + " charge each second.";
	}
	if(IsHoldGoal() && _Fields[0] < 10000)
	{
		ExtraDescription += "  The held charge will discharge after " + String(_Fields[0]) + " seconds.";
	}
	if(IsPowerSource())
	{
		ExtraDescription += "  This Power Source will start emitting after " + String(_Fields[0]) + " seconds, and will emit a charge every " + String(_Fields[1]) +
							" seconds, stopping after " + String(_Fields[2]) + " charges.";
	}
	return BaseDescription + ExtraDescription;
}

void Building::Move(const Coordinate &NewLocation)
{
	_Location = NewLocation;
}

void Building::SetPuzzlePiece(BuildingPuzzleType NewPuzzlePiece)
{
	_PuzzlePiece = NewPuzzlePiece;
}

void Building::SetChargePreference(UINT NewPreference)
{
	if(CanChangePreferenceLevel())
	{
		_ChargePreference = NewPreference;
	}
}

Building* Building::GetCircuitBoundaryFromGrid(Game &G, Building *GridBuilding)
{
	Coordinate GridOffset = GridBuilding->_Location - _Location;
	Coordinate CircuitCoordinates = GridOffset;// = Coordinate((GridOffset.x + 2) * 4, (GridOffset.y + 2) * 4);
	for(UINT CoordinateIndex = 0; CoordinateIndex < 2; CoordinateIndex++)
	{
		int &CurValue = Utility::Choose(CircuitCoordinates.x, CircuitCoordinates.y, CoordinateIndex);
		switch(CurValue)
		{
		case -2:
			CurValue = 0;
			break;
		case -1:
			CurValue = 2;
			break;
		case 0:
			CurValue = 6;
			break;
		case 1:
			CurValue = 10;
			break;
		case 2:
			CurValue = 12;
			break;
		default:
			SignalError("Unexpected offset");
		}
	}
	Building *Result = CircuitBoard().Cells().GetElement(CircuitCoordinates.y, CircuitCoordinates.x).Bldg;
	Assert(Result != NULL && Result->IsCircuitBoundary(), "Invalid projection");
	return Result;
}

Building* Building::GetCircuitToCircuitProjection(Game &G, Building *SourceBuilding)
{
	Building *OtherCircuit = G.CircuitFromLocation(SourceBuilding->ParentCircuit());
	Assert(OtherCircuit != this, "GetCircuitToCircuitProjection called on building in same circuit");
	Assert(_CircuitBoard != NULL, "GetCircuitToCircuitProjection called on non-circuit");
	Coordinate Offset = _Location - OtherCircuit->_Location;
	if(Offset.x != 0 && Offset.y != 0)
	{
		return GetCircuitBoundaryFromGrid(G, OtherCircuit);
	}
	Building *Result = NULL;
	if(Offset.x == -2)
	{
		Result = _CircuitBoard->Cells().GetElement(SourceBuilding->Location().y, CircuitSize - 2).Bldg;
	}
	else if(Offset.x == 2)
	{
		Result = _CircuitBoard->Cells().GetElement(SourceBuilding->Location().y, 0).Bldg;
	}
	else if(Offset.y == -2)
	{
		Result = _CircuitBoard->Cells().GetElement(CircuitSize - 2, SourceBuilding->Location().x).Bldg;
	}
	else if(Offset.y == 2)
	{
		Result = _CircuitBoard->Cells().GetElement(0, SourceBuilding->Location().x).Bldg;
	}
	Assert(Result != NULL, "Invalid result for GetCircuitToCircuitProjection");
	return Result;
}

Building* Building::GetCircuitBoundaryDestination(Game &G)
{
	Assert(IsCircuitBoundary(), "GetCircuitBoundaryDestination called on invalid building");
	Grid<BoardCell> &Cells = G.GetBoard().Cells();
	Coordinate SmallerGrid(_Location.x / 2, _Location.y / 2);
	Coordinate GridPos = Coordinate::Invalid;

	if((SmallerGrid.x == 0 && SmallerGrid.y == 0) ||
	   (SmallerGrid.x == CircuitSize / 2 - 1 && SmallerGrid.y == 0) ||
	   (SmallerGrid.x == 0 && SmallerGrid.y == CircuitSize / 2 - 1) ||
	   (SmallerGrid.x == CircuitSize / 2 - 1 && SmallerGrid.y == CircuitSize / 2 - 1))
	{
		return NULL;
	}

	Coordinate WorldOffset;

	if(SmallerGrid.y == 0 && G.CircuitAtLocation(_ParentCircuit + Coordinate(0, -2)))
	{
		return G.CircuitFromLocation(_ParentCircuit + Coordinate(0, -2))->GetCircuitToCircuitProjection(G, this);
	}
	if(SmallerGrid.y == CircuitSize / 2 - 1 && G.CircuitAtLocation(_ParentCircuit + Coordinate(0, 2)))
	{
		return G.CircuitFromLocation(_ParentCircuit + Coordinate(0, 2))->GetCircuitToCircuitProjection(G, this);
	}
	if(SmallerGrid.x == 0 && G.CircuitAtLocation(_ParentCircuit + Coordinate(-2, 0)))
	{
		return G.CircuitFromLocation(_ParentCircuit + Coordinate(-2, 0))->GetCircuitToCircuitProjection(G, this);
	}
	if(SmallerGrid.x == CircuitSize / 2 - 1 && G.CircuitAtLocation(_ParentCircuit + Coordinate(2, 0)))
	{
		return G.CircuitFromLocation(_ParentCircuit + Coordinate(2, 0))->GetCircuitToCircuitProjection(G, this);
	}
	
	//
	// Top wall
	//
	if(SmallerGrid.x == 1 && SmallerGrid.y == 0)
	{
		GridPos = Coordinate(-1, -2);
	}
	else if(SmallerGrid.x == 3 && SmallerGrid.y == 0)
	{
		GridPos = Coordinate(0, -2);
	}
	else if(SmallerGrid.x == 5 && SmallerGrid.y == 0)
	{
		GridPos = Coordinate(1, -2);
	}

	//
	// Bottom wall
	//
	if(SmallerGrid.x == 1 && SmallerGrid.y == 6)
	{
		GridPos = Coordinate(-1, 2);
	}
	else if(SmallerGrid.x == 3 && SmallerGrid.y == 6)
	{
		GridPos = Coordinate(0, 2);
	}
	else if(SmallerGrid.x == 5 && SmallerGrid.y == 6)
	{
		GridPos = Coordinate(1, 2);
	}

	//
	// Left wall
	//
	if(SmallerGrid.x == 0 && SmallerGrid.y == 1)
	{
		GridPos = Coordinate(-2, -1);
	}
	else if(SmallerGrid.x == 0 && SmallerGrid.y == 3)
	{
		GridPos = Coordinate(-2, 0);
	}
	else if(SmallerGrid.x == 0 && SmallerGrid.y == 5)
	{
		GridPos = Coordinate(-2, 1);
	}

	//
	// Right wall
	//
	if(SmallerGrid.x == 6 && SmallerGrid.y == 1)
	{
		GridPos = Coordinate(2, -1);
	}
	else if(SmallerGrid.x == 6 && SmallerGrid.y == 3)
	{
		GridPos = Coordinate(2, 0);
	}
	else if(SmallerGrid.x == 6 && SmallerGrid.y == 5)
	{
		GridPos = Coordinate(2, 1);
	}

	if(GridPos == Coordinate::Invalid)
	{
		return NULL;
	}

	GridPos = GridPos + _ParentCircuit;

	if(Cells.ValidCoordinates(GridPos.y, GridPos.x))
	{
		Building *SourceBuilding = Cells.GetElement(GridPos.y, GridPos.x).Bldg;
		if(SourceBuilding != NULL && SourceBuilding->Location() == GridPos)
		{
			if(SourceBuilding->Type() == BuildingCircuit)
			{
				return SourceBuilding->GetCircuitBoundaryFromGrid(G, G.CircuitFromLocation(_ParentCircuit));
			}
			return SourceBuilding;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void Building::Discharge()
{
	_AbsorbedCharge = 0;
}

Vec2 Building::GetWorldLocation()
{
	if(_ParentCircuit == Coordinate::Invalid)
	{
		return Vec2(float(_Location.x + 0.5f), float(_Location.y + 0.5f));
	}
	else
	{
		return Vec2(float(_ParentCircuit.x), float(_ParentCircuit.y)) + Vec2(float(_Location.x + 0.5f) / float(CircuitSize), float(_Location.y + 0.5f) / float(CircuitSize));
	}
}

Building* Building::FindGrayGateMatch(Game &G)
{
	float MaxRadius = 100.0f;
	float MaxRadiusSq = MaxRadius * MaxRadius;

	Building *BestBuilding = NULL;
	float BestValue = MaxRadiusSq;

	for(UINT BuildingIndex = 0; BuildingIndex < G.Buildings().Length(); BuildingIndex++)
	{
		Building *CurBuilding = G.Buildings()[BuildingIndex];
		if( CurBuilding->_Type == BuildingGateOpenGray ||
			CurBuilding->_Type == BuildingGateClosedGray )
		{
			float CurDistSq = Vec2::DistSq(CurBuilding->GetWorldLocation(), GetWorldLocation());
			if(CurBuilding->ParentCircuit() == _ParentCircuit)
			{
				CurDistSq -= 10000.0f;
			}
			if(CurDistSq <= BestValue)
			{
				BestValue = CurDistSq;
				BestBuilding = CurBuilding;
			}
		}
	}

	return BestBuilding;
}

Building* Building::FindGrayTrapMatch(Game &G)
{
	float MaxRadius = 100.0f;
	float MaxRadiusSq = MaxRadius * MaxRadius;

	Building *BestBuilding = NULL;
	float BestValue = MaxRadiusSq;

	for(UINT BuildingIndex = 0; BuildingIndex < G.Buildings().Length(); BuildingIndex++)
	{
		Building *CurBuilding = G.Buildings()[BuildingIndex];
		if( CurBuilding->_Type == BuildingTrapOpenGray ||
			CurBuilding->_Type == BuildingTrapSprungGray )
		{
			float CurDistSq = Vec2::DistSq(CurBuilding->GetWorldLocation(), GetWorldLocation());
			if(CurBuilding->ParentCircuit() == _ParentCircuit)
			{
				CurDistSq -= 10000.0f;
			}
			if(CurDistSq <= BestValue)
			{
				BestValue = CurDistSq;
				BestBuilding = CurBuilding;
			}
		}
	}

	return BestBuilding;
}

Building* Building::FindTeleportDestination(Game &G)
{
	float MaxRadius = 100.0f;
	float MaxRadiusSq = MaxRadius * MaxRadius;

	Building *BestBuilding = NULL;
	float BestValue = MaxRadiusSq;

	for(UINT BuildingIndex = 0; BuildingIndex < G.Buildings().Length(); BuildingIndex++)
	{
		Building *CurBuilding = G.Buildings()[BuildingIndex];
		if( (_Type == BuildingTeleportRedSource		&& CurBuilding->Type() == BuildingTeleportRedDestination)    ||
			(_Type == BuildingTeleportOrangeSource  && CurBuilding->Type() == BuildingTeleportOrangeDestination) ||
			(_Type == BuildingTeleportYellowSource  && CurBuilding->Type() == BuildingTeleportYellowDestination) ||
			(_Type == BuildingTeleportGreenSource	&& CurBuilding->Type() == BuildingTeleportGreenDestination)  ||
			(_Type == BuildingTeleportBlueSource	&& CurBuilding->Type() == BuildingTeleportBlueDestination) )
		{
			float CurDistSq = Vec2::DistSq(CurBuilding->GetWorldLocation(), GetWorldLocation());
			if(CurBuilding->ParentCircuit() == _ParentCircuit)
			{
				CurDistSq -= 10000.0f;
			}
			if(CurDistSq <= BestValue)
			{
				BestValue = CurDistSq;
				BestBuilding = CurBuilding;
			}
		}
	}

	return BestBuilding;
}

Coordinate Building::OffsetCoordinateFromArrowIndex(UINT Index)
{
	switch(Index)
	{
	case 1:
		return Coordinate(-1, -2);
	case 2:
		return Coordinate(0, -2);
	case 3:
		return Coordinate(1, -2);
	case 4:
		return Coordinate(2, -1);
	case 5:
		return Coordinate(2, 0);
	case 6:
		return Coordinate(2, 1);
	case 7:
		return Coordinate(1, 2);
	case 8:
		return Coordinate(0, 2);
	case 9:
		return Coordinate(-1, 2);
	case 10:
		return Coordinate(-2, 1);
	case 11:
		return Coordinate(-2, 0);
	case 12:
		return Coordinate(-2, -1);
	}
	//SignalError("Invalid arrow index");
	return Coordinate(-1, -2);
}

bool Building::WillAcceptCharge(Game &G, const Charge &C, Building *CurrentBuilding, bool IgnoreLastChargeVisit)
{
	if(IsPowerSource() || IsClosedGate() || IsTrapSprung())
	{
		return false;
	}
	
	if(CurrentBuilding != NULL)
	{
		if(CurrentBuilding->Fields()[BuildingFieldArrowIndex] != 0)
		{
			Coordinate Offset = _Location - CurrentBuilding->Location();
			return (Offset == OffsetCoordinateFromArrowIndex(CurrentBuilding->Fields()[BuildingFieldArrowIndex]));
		}
	}

	if(!IgnoreLastChargeVisit)
	{
		int CurrentTimeDifference = int(G.CurrentTime()) - int(_LastChargeVisit);
		int RequiredTimeDifference = ChargeRequiredTimeDifference;
		/*if(IsHoldClass())
		{
			RequiredTimeDifference += 2;
		}*/
		/*if(_Type == BuildingWireMinorDelay)
		{
			RequiredTimeDifference *= 2;
		}
		if(_Type == BuildingWireMajorDelay)
		{
			RequiredTimeDifference *= 4;
		}*/
#ifdef DEBUG_CHARGE_PATH
		Console::WriteLine(String("    Building ") + GetBuildingTypeString(_Type) + String(" cooldown check, ") + String(CurrentTimeDifference) + String(" / ") + String(RequiredTimeDifference));
#endif
		if(CurrentTimeDifference < RequiredTimeDifference)
		{
			return false;
		}
	}
	switch(_Type)
	{
	case BuildingHold:
	case BuildingRedChargeGoal:
	case BuildingOrangeChargeGoal:
	case BuildingYellowChargeGoal:
	case BuildingGreenChargeGoal:
	case BuildingBlueChargeGoal:
		return (_AbsorbedCharge == 0);
	/*case BuildingRedChargeGoal:
		return (_AbsorbedCharge == 0 && C.Level() == 1);
	case BuildingOrangeChargeGoal:
		return (_AbsorbedCharge == 0 && C.Level() == 2);
	case BuildingYellowChargeGoal:
		return (_AbsorbedCharge == 0 && C.Level() == 3);
	case BuildingGreenChargeGoal:
		return (_AbsorbedCharge == 0 && C.Level() == 4);
	case BuildingBlueChargeGoal:
		return (_AbsorbedCharge == 0 && C.Level() == 5);*/
	case BuildingStreamer:
		return (_AbsorbedCharge == 0);
	case BuildingAmplifierL2:
		return (_AbsorbedCharge == 0 || C.Level() >= 2 || _AbsorbedCharge == 2);
	case BuildingAmplifierL3:
		return (_AbsorbedCharge == 0 || C.Level() >= 3 || _AbsorbedCharge == 3);
	case BuildingAmplifierL4:
		return (_AbsorbedCharge == 0 || C.Level() >= 4 || _AbsorbedCharge == 4);
	case BuildingCircuitBoundaryBlocked:
		return false;

	case BuildingRedChargeFilter:
		return (C.Level() == 1);
	case BuildingOrangeChargeFilter:
		return (C.Level() == 2);
	case BuildingYellowChargeFilter:
		return (C.Level() == 3);
	case BuildingGreenChargeFilter:
		return (C.Level() == 4);
	case BuildingBlueChargeFilter:
		return (C.Level() == 5);

	case BuildingMegaHoldRed:
		return (C.Level() == 1);
	case BuildingMegaHoldOrange:
		return (C.Level() == 2);
	case BuildingMegaHoldYellow:
		return (C.Level() == 3);
	case BuildingMegaHoldGreen:
		return (C.Level() == 4);
	case BuildingMegaHoldBlue:
		return (C.Level() == 5);

	case BuildingBlocker:
		return false;
	}
	return true;
}

bool Building::EatCharge(Game &G, const Charge &C, bool PlaySounds)
{
	if(IsHoldClass())
	{
		Assert(_AbsorbedCharge == 0, "Accepting before eating");
		_AbsorbedCharge = C.Level();
		if(IsHoldGoal())
		{
			_TimeUntilDischarge = _Fields[0] * GameFramesPerSecond;
		}
		return true;
	}
	if(IsMegaHold())
	{
		if(_ParentCircuit != Coordinate::Invalid)
		{
			Building *Circuit = G.CircuitFromLocation(_ParentCircuit);
			Building *BaseMegaHold = Circuit->CircuitBoard().Cells().GetElement(2, 2).Bldg;
			Assert(BaseMegaHold != NULL && BaseMegaHold->IsMegaHold(), "Mega hold in circuit must have element at (2, 2)");
			BaseMegaHold->_AbsorbedCharge += C.Level();
		}
		else
		{
			_AbsorbedCharge += C.Level();
		}
		return true;
	}
	if(IsGateSwitch())
	{
		UINT MyColorIndex = UINT(_Type) - UINT(BuildingGateSwitchRed);
		if(_Type == BuildingGateSwitchGray)
		{
			Building *Match = FindGrayGateMatch(G);
			if(Match != NULL)
			{
				if(Match->IsOpenGate())
				{
					Match->_Type = BuildingGateClosedGray;
				}
				else
				{
					Match->_Type = BuildingGateOpenGray;
				}
			}
		}
		else
		{
			for(UINT Index = 0; Index < G.Buildings().Length(); Index++)
			{
				Building &CurBuilding = *G.Buildings()[Index];
				if(CurBuilding.IsOpenGate())
				{
					if(UINT(CurBuilding.Type()) - UINT(BuildingGateOpenRed) == MyColorIndex)
					{
						CurBuilding._Type = BuildingType(BuildingGateClosedRed + MyColorIndex);
					}
				}
				else if(CurBuilding.IsClosedGate())
				{
					if(UINT(CurBuilding.Type()) - UINT(BuildingGateClosedRed) == MyColorIndex)
					{
						CurBuilding._Type = BuildingType(BuildingGateOpenRed + MyColorIndex);
					}
				}
			}
		}
	}
	if(IsTrapReset())
	{
		UINT MyColorIndex = UINT(_Type) - UINT(BuildingTrapResetRed);
		if(_Type == BuildingTrapResetGray)
		{
			Building *Match = FindGrayTrapMatch(G);
			if(Match != NULL)
			{
				Match->_Type = BuildingTrapOpenGray;
			}
		}
		else
		{
			for(UINT Index = 0; Index < G.Buildings().Length(); Index++)
			{
				Building &CurBuilding = *G.Buildings()[Index];
				if(CurBuilding.IsTrapSprung())
				{
					if(UINT(CurBuilding.Type()) - UINT(BuildingTrapSprungRed) == MyColorIndex)
					{
						CurBuilding._Type = BuildingType(BuildingTrapOpenRed + MyColorIndex);
					}
				}
			}
		}
	}
	if(IsTrapOpen())
	{
		if(_Type == BuildingTrapOpenGray)
		{
			_Type = BuildingTrapSprungGray;
		}
		else
		{
			UINT MyColorIndex = UINT(_Type) - UINT(BuildingTrapOpenRed);
			_Type = BuildingType(BuildingTrapSprungRed + MyColorIndex);
		}
	}
	switch(_Type)
	{
	case BuildingSplitter:
		{
			int SplitA = 0, SplitB = 0, SplitC = 0;
			const bool UseConservativeSplitterRule = true;
			if(UseConservativeSplitterRule)
			{
				switch(C.Level())
				{
				case 1:
					return false;
				case 2:
					SplitA = 1;
					SplitB = 1;
					break;
				case 3:
					SplitA = 1;
					SplitB = 1;
					SplitC = 1;
					break;
				case 4:
					SplitA = 2;
					SplitB = 2;
					break;
				case 5:
					SplitA = 3;
					SplitB = 3;
					break;
				default:
					SignalError("Undefined split");
					break;
				}
			}
			else
			{
				switch(C.Level())
				{
				case 1:
					return false;
				case 2:
					SplitA = 1;
					SplitB = 1;
					break;
				case 3:
					SplitA = 2;
					SplitB = 2;
					break;
				case 4:
					SplitA = 3;
					SplitB = 3;
					break;
				case 5:
					SplitA = 4;
					SplitB = 4;
					break;
				default:
					SignalError("Undefined split");
					break;
				}
			}
			//const Coordinate &SourceLocation, const Coordinate &SourceCircuit, const Coordinate &CameFromLocation, const Coordinate &CameFromCircuit, UINT Level
			G.AddCharge(Charge(_Location, _ParentCircuit, C._Source.Location, C._Source.Circuit, SplitA));
			G.AddCharge(Charge(_Location, _ParentCircuit, C._Source.Location, C._Source.Circuit, SplitB));
			if(SplitC > 0)
			{
				G.AddCharge(Charge(_Location, _ParentCircuit, C._Source.Location, C._Source.Circuit, SplitC));
			}
            if(PlaySounds && G.Audio().PlaySounds)
            {
			    G.Audio().Audio.PlayWaveFile(G.Audio().SplitterSound);
            }
			return true;
		}
		break;
	case BuildingAmplifier:
	case BuildingAmplifierL1:
	case BuildingAmplifierL2:
	case BuildingAmplifierL3:
	case BuildingAmplifierL4:
		{
			if(_AbsorbedCharge == 0)
			{
				_AbsorbedCharge = C.Level();
                if(PlaySounds && G.Audio().PlaySounds)
                {
				    G.Audio().Audio.PlayWaveFile(G.Audio().ChargeSound);
                }
			}
			else
			{
				UINT ChargeLevel;
				if(_Type == BuildingAmplifier)
				{
					ChargeLevel = Math::Min(Math::Max(_AbsorbedCharge, C.Level()) + 1, MaxChargeLevel);
				}
				else
				{
					ChargeLevel = UINT(_Type) - UINT(BuildingAmplifierL1) + 2;
				}
				G.AddCharge(Charge(_Location, _ParentCircuit, C._Source.Location, C._Source.Circuit, ChargeLevel));
				_AbsorbedCharge = 0;
                if(PlaySounds && G.Audio().PlaySounds)
                {
				    G.Audio().Audio.PlayWaveFile(G.Audio().DischargeSound);
                }
			}
			return true;
		}
		break;
	default:
		return false;
	}
}

void Building::Upgrade(BuildingType NewType)
{
	if(_Type == BuildingCircuit)
	{
		switch(NewType)
		{
		case BuildingCircuitRotate90:
			_CircuitBoard->Rotate90();
			break;
		case BuildingCircuitRotateN90:
			_CircuitBoard->RotateN90();
			break;
		case BuildingCircuitFlipHorizontal:
			_CircuitBoard->FlipHorizontal();
			break;
		case BuildingCircuitFlipVertical:
			_CircuitBoard->FlipVertical();
			break;
		case BuildingCircuitCopy:
			//_CircuitBoard->Rotate90();
			break;
		default:
			SignalError("Invalid circuit upgrade");
		}
		
	}
	else
	{
		_Type = NewType;
		_InitialType = NewType;
	}
}

void Building::InitCircuitBoard(Game &G, bool AddBoundaries)
{
	_CircuitBoard = new Board;

	CircuitBoard().Init(CircuitSize, CircuitSize);
	
	if(AddBoundaries)
	{
		for(UINT Index = 1; Index < 6; Index++)
		{
			G.AddBuilding(Coordinate(Index * 2, 0),					_Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
			G.AddBuilding(Coordinate(0, Index * 2),					_Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
			G.AddBuilding(Coordinate(Index * 2, CircuitSize - 2),	_Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
			G.AddBuilding(Coordinate(CircuitSize - 2, Index * 2),	_Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
		}
		G.AddBuilding(Coordinate(0, 0),								_Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
		G.AddBuilding(Coordinate(0, CircuitSize - 2),				_Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
		G.AddBuilding(Coordinate(CircuitSize - 2, 0),				_Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
		G.AddBuilding(Coordinate(CircuitSize - 2, CircuitSize - 2), _Location, BuildingCircuitBoundaryBlocked, 2, NULL, BuildingUser);
	}

	/*UINT AdjacentCount;
	Building *AdjacentBuildings[6];
	G.GetBoard().FindAdjacentBuildings(_Location, AdjacentBuildings, AdjacentCount);

	for(UINT AdjacentIndex = 0; AdjacentIndex < AdjacentCount; AdjacentIndex++)
	{
		Coordinate Offset = _Location - AdjacentBuildings[AdjacentIndex]->_Location;
		Building *WallBuilding = CircuitBoard().Cells().GetElement((Offset.y + 2) * 2, (Offset.x + 2) * 2).Bldg;
		Assert(WallBuilding != NULL, "Circuit boundary not found");
		Assert(WallBuilding->Type() == BuildingCircuitBoundary, "Circuit boundary invalid");
		WallBuilding->_AppearsAs = AdjacentBuildings[AdjacentIndex]->Type;
	}*/
}
