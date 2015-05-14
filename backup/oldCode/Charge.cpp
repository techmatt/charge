#include "Main.h"

Charge::Charge(const Coordinate &SourceLocation, const Coordinate &SourceCircuit, const Coordinate &CameFromLocation, const Coordinate &CameFromCircuit, UINT Level)
{
	_Source.Location = CameFromLocation;
	_Source.Circuit = CameFromCircuit;
	_Destination.Location = SourceLocation;
	_Destination.Circuit = SourceCircuit;
	_TimeInTransit = 0;
	_TotalTransitTime = 0;
	_Level = Level;
    _RandomValue = rnd();
}

RGBColor Charge::ColorFromLevel(UINT Level)
{
	switch(Level)
	{
	case 0:
		return RGBColor::Gray;
	case 1:
		return RGBColor::Red;
	case 2:
		return RGBColor(255, 128, 0);
	case 3:
		return RGBColor::Yellow;
	case 4:
		return RGBColor::Green;
	case 5:
		return RGBColor::Blue;
	case 6:
		return RGBColor(149, 38, 252);
	}
	return RGBColor::Black;
}

double Charge::ComputePreference(Game &G, Building &TargetBuilding)
{
	double Preference = 0.0;
	Preference = TargetBuilding.ChargePreference() * 1000000.0;
	
	int CurrentTimeDifference = Math::Min(int(G.CurrentTime()) - int(TargetBuilding.LastChargeVisit()), 900000);
	//Assert(CurrentTimeDifference >= ChargeRequiredTimeDifference, "Tower discharge test too late");
	Preference += CurrentTimeDifference;

	return Preference;
}

void Charge::SetNewDestination(Game &G, Building *NewDestination)
{
	_Source = _Destination;
	_Destination.Location = NewDestination->Location();
	_Destination.Circuit = NewDestination->ParentCircuit();
	if(!NewDestination->IsMegaHold())
	{
		NewDestination->LastChargeVisit() = G.CurrentTime();
	}
	_TimeInTransit = 0;
	_TotalTransitTime = ChargeTransitTime;
	if(NewDestination->Type() == BuildingWireMinorDelay)
	{
		_TotalTransitTime *= 2;
	}
	if(NewDestination->Type() == BuildingWireMajorDelay)
	{
		_TotalTransitTime *= 4;
	}
	/*if(NewDestination->IsHoldClass())
	{
		_TotalTransitTime += 2;
	}*/
	if(NewDestination->Type() == BuildingWireMinorAccelerator)
	{
		_TotalTransitTime = _TotalTransitTime * 2 / 3;
	}
	if(NewDestination->Type() == BuildingWireMajorAccelerator)
	{
		_TotalTransitTime /= 2;
	}
}

bool Charge::FindNewDestination(Game &G, bool &ShowDeathAnimation, Building *CurrentBuilding, Building *PreviousBuilding)
{
	UINT AdjacentCount;
	Building *AdjacentBuildings[6];
	if(_Destination.Circuit == Coordinate::Invalid)
	{
		G.GetBoard().FindAdjacentBuildings(_Destination.Location, AdjacentBuildings, AdjacentCount);
	}
	else
	{
		Building *CircuitBuilding = G.CircuitFromLocation(_Destination.Circuit);
		CircuitBuilding->CircuitBoard().FindAdjacentBuildings(_Destination.Location, AdjacentBuildings, AdjacentCount);
	}

	double LargestPreference = 0.0;
	Building *BestBuilding = NULL;

#ifdef DEBUG_CHARGE_PATH
	Console::WriteLine(String("Charge level ") + String(_Level) + String(" looking for new destination, ") + String(AdjacentCount) + " adjacent buildings");
#endif

	for(UINT AdjacentIndex = 0; AdjacentIndex < AdjacentCount; AdjacentIndex++)
	{
		Building *CandidateBuilding = AdjacentBuildings[AdjacentIndex];
		bool ImpassableCircuitWall = false;

		if(CandidateBuilding->Type() == BuildingCircuit)
		{
			CandidateBuilding = CandidateBuilding->GetCircuitBoundaryFromGrid(G, CurrentBuilding);
			if(CandidateBuilding->Type() == BuildingCircuitBoundaryBlocked)
			{
				ImpassableCircuitWall = true;
			}
		}
		else if(CandidateBuilding->IsCircuitBoundary())
		{
			Building *Proxy = CandidateBuilding->GetCircuitBoundaryDestination(G);
			if(Proxy == NULL || CandidateBuilding->Type() == BuildingCircuitBoundaryBlocked)
			{
				ImpassableCircuitWall = true;
			}
			if(CurrentBuilding->IsCircuitBoundary())
			{
				ImpassableCircuitWall = true;
			}
		}

		bool BuildingWillAccept = CandidateBuilding->WillAcceptCharge(G, *this, CurrentBuilding, false);
		bool IsPreviousBuilding = PreviousBuilding == CandidateBuilding || PreviousBuilding == AdjacentBuildings[AdjacentIndex];
#ifdef DEBUG_CHARGE_PATH
		Console::WriteLine(String("  Building ") + String(AdjacentIndex) + String(": ") + GetBuildingTypeString(CandidateBuilding->Type()) + 
						   String(", CWall = ") + ImpassableCircuitWall +
						   String(", Accept = ") + BuildingWillAccept +
						   String(", Previous = ") + IsPreviousBuilding);
#endif

		if(!IsPreviousBuilding &&
		   !ImpassableCircuitWall &&
		   BuildingWillAccept)
		{
			double CurrentPreference = ComputePreference(G, *CandidateBuilding);
#ifdef DEBUG_CHARGE_PATH
			Console::WriteLine(String("  Preference = ") + CurrentPreference);
#endif
			if(CurrentPreference > LargestPreference)
			{
				LargestPreference = CurrentPreference;
				BestBuilding = CandidateBuilding;
			}
		}
	}
	if(BestBuilding != NULL)
	{
		SetNewDestination(G, BestBuilding);
		return true;
	}
	return false;
}

bool Charge::Update(Game &G, bool SecondPass, bool &ShowDeathAnimation, Building* &SourceBuilding, Building* &DestBuilding, const Coordinate &SelectedCircuitLocation)
{
	ShowDeathAnimation = true;

	Building *SourceCircuitBuilding, *DestCircuitBuilding;
	
	if(_Destination.Circuit == Coordinate::Invalid)
	{
		DestCircuitBuilding = NULL;
		DestBuilding = G.GetBoard().Cells().GetElement(_Destination.Location.y, _Destination.Location.x).Bldg;
	}
	else
	{
		DestCircuitBuilding = G.CircuitFromLocation(_Destination.Circuit);
		DestBuilding = DestCircuitBuilding->CircuitBoard().Cells().GetElement(_Destination.Location.y, _Destination.Location.x).Bldg;
	}

	if(SecondPass && (DestBuilding == NULL || !DestBuilding->IsHoldClass()))
	{
		return true;
	}

	if(DestBuilding == NULL)
	{
		return false;
	}
	
	if(_Source.Circuit == Coordinate::Invalid)
	{
		SourceCircuitBuilding = NULL;
		if(_Source.Location == Coordinate::Invalid)
		{
			SourceBuilding = NULL;
		}
		else
		{
			SourceBuilding = G.GetBoard().Cells().GetElement(_Source.Location.y, _Source.Location.x).Bldg;
		}
	}
	else
	{
		SourceCircuitBuilding = G.CircuitFromLocation(_Source.Circuit);
		if(_Source.Location == Coordinate::Invalid)
		{
			SourceBuilding = NULL;
		}
		else
		{
			SourceBuilding = SourceCircuitBuilding->CircuitBoard().Cells().GetElement(_Source.Location.y, _Source.Location.x).Bldg;
		}
	}

    bool PlaySounds = (SourceCircuitBuilding == NULL || DestCircuitBuilding == NULL ||
                       SourceCircuitBuilding->Location() == SelectedCircuitLocation || DestCircuitBuilding->Location() == SelectedCircuitLocation);

	if(_TimeInTransit >= _TotalTransitTime)
	{
		Assert(DestBuilding->Type() != BuildingCircuit && (SourceBuilding == NULL || SourceBuilding->Type() != BuildingCircuit), "Cannot move towards circuits");

		if(!DestBuilding->IsMegaHold())
		{
			//DestBuilding->LastChargeVisit() = G.CurrentTime();
		}

		if(DestBuilding->IsHoldClass())
		{
			DestBuilding->LastChargeVisit() = G.CurrentTime() - ChargeRequiredTimeDifference + 2;
		}

		//
		// Handle transfer from circuit to grid
		//
		if(DestBuilding->IsCircuitBoundary() && SourceCircuitBuilding == DestCircuitBuilding)
		{
			Building *Proxy = DestBuilding->GetCircuitBoundaryDestination(G);
			if(Proxy == NULL)
			{
				_Source = _Destination;
				return false;
			}
			
			if(Proxy->WillAcceptCharge(G, *this, NULL, false))
			{
				SetNewDestination(G, Proxy);
				return true;
			}
			else
			{
				_Source = _Destination;
				Proxy->WillAcceptCharge(G, *this, NULL, false);
				return false;
			}
		}
		
		//
		// Handle towers that consume charge.  Do not allow amplifiers or splitters to consume the charge
		// they just produced.
		//
		if(DestBuilding->IsHoldClass())
		{
			if(DestBuilding->AbsorbedCharge() == 0)
			{
				DestBuilding->EatCharge(G, *this, PlaySounds);
			}
			else
			{
				// can happen if we get rotated
				//Assert(DestBuilding->AbsorbedCharge() == _Level, "Hold tower not charged properly");
				//DestBuilding->Discharge();
				//return false;
			}
		}
		else if(_TotalTransitTime != 0 && DestBuilding->EatCharge(G, *this, PlaySounds))
		{
			ShowDeathAnimation = false;
			return false;
		}

		//
		// Handle teleporters
		//
		if(DestBuilding->IsTeleportSource())
		{
			Building *NewDestination = DestBuilding->FindTeleportDestination(G);
			if(NewDestination == NULL)
			{
                _Source = _Destination;
				return false;
			}
			else
			{
				if(NewDestination->WillAcceptCharge(G, *this, NULL, false))
				{
					SetNewDestination(G, NewDestination);
					return true;
				}
				else
				{
					_Source = _Destination;
					return false;
				}
			}
		}

		//
		// Find a new target in adjacent squares
		//
		if(FindNewDestination(G, ShowDeathAnimation, DestBuilding, SourceBuilding))
		{
			if(DestBuilding->IsHoldClass())
			{
				DestBuilding->Discharge();
			}
			return true;
		}
		else
		{
			if(DestBuilding->IsHoldClass())
			{
				_TimeInTransit = _TotalTransitTime;

				if(SourceBuilding == NULL || !SourceBuilding->WillAcceptCharge(G, *this, NULL, true))
				{
					_Source.Circuit = Coordinate::Invalid;
					_Source.Location = Coordinate::Invalid;
				}

				if(DestBuilding->IsHoldGoal())
				{
					if(DestBuilding->TimeUntilDischarge() == 0)
					{
						DestBuilding->Discharge();
						_Source = _Destination;
						return false;
					}
					DestBuilding->TimeUntilDischarge()--;
				}

				return true;
			}

			_Source = _Destination;
			return false;
		}
	}
	else
	{
		if(!SecondPass)
		{
			_TimeInTransit++;
		}
		return true;
	}
}