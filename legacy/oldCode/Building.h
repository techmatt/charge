class Game;
class Charge;
class Board;

const UINT CircuitSize = 14;
const UINT MaxFieldIndex = 8;
const int BuildingFieldArrowIndex = 3;
const int BuildingArrowDirections = 12;

enum BuildingPuzzleType
{
	BuildingUser,
	BuildingPuzzlePiece,
	BuildingCopiedCircuit,
};

class Building
{
public:
	Building();
	Building(const Coordinate &Location, const Coordinate &ParentCircuit, BuildingType Type, UINT ChargePreference, UINT *Fields, BuildingPuzzleType PuzzlePiece);
	~Building();

	void ResetPuzzle();
	void FreeMemory();
	void InitCircuitBoard(Game &G, bool AddBoundaries);

	__forceinline const Coordinate& Location()
	{
		return _Location;
	}
	__forceinline const Coordinate& ParentCircuit()
	{
		return _ParentCircuit;
	}
	__forceinline BuildingType Type()
	{
		return _Type;
	}
	__forceinline UINT ChargePreference()
	{
		return _ChargePreference;
	}
	__forceinline bool OnCircuitBoard()
	{
		return (_ParentCircuit != Coordinate::Invalid);
	}
	__forceinline Board& CircuitBoard()
	{
		Assert(_Type == BuildingCircuit, "CircuitBoard only valid for circuit");
		return *_CircuitBoard;
	}
	__forceinline int& LastChargeVisit()
	{
		//Assert(_Type != BuildingCircuit, "LastChargeVisit not valid for circuit");
		return _LastChargeVisit;
	}
	__forceinline UINT& AbsorbedCharge()
	{
		//Assert(IsAmplifierTower() || IsHoldClass(), "AbsorbedCharge call invalid");
		return _AbsorbedCharge;
	}
  __forceinline bool IsForegroundLayer()
  {
    return (IsAmplifierTower() || IsHoldClass() || IsTrapOpen() || IsTrapSprung() ||
            IsOpenGate() || IsClosedGate());
  }
	__forceinline bool IsPowerSource()
	{
		return (_Type == BuildingPowerSourceRed    ||
				_Type == BuildingPowerSourceOrange ||
				_Type == BuildingPowerSourceYellow ||
				_Type == BuildingPowerSourceGreen  ||
				_Type == BuildingPowerSourceBlue);
	}
	__forceinline bool IsOpenGate()
	{
		return (_Type == BuildingGateOpenRed    ||
				_Type == BuildingGateOpenOrange ||
				_Type == BuildingGateOpenYellow ||
				_Type == BuildingGateOpenGreen  ||
				_Type == BuildingGateOpenBlue	||
				_Type == BuildingGateOpenGray);
	}
	__forceinline bool IsClosedGate()
	{
		return (_Type == BuildingGateClosedRed    ||
				_Type == BuildingGateClosedOrange ||
				_Type == BuildingGateClosedYellow ||
				_Type == BuildingGateClosedGreen  ||
				_Type == BuildingGateClosedBlue	  ||
				_Type == BuildingGateClosedGray);
	}
	__forceinline bool IsGateSwitch()
	{
		return (_Type == BuildingGateSwitchRed    ||
				_Type == BuildingGateSwitchOrange ||
				_Type == BuildingGateSwitchYellow ||
				_Type == BuildingGateSwitchGreen  ||
				_Type == BuildingGateSwitchBlue   ||
				_Type == BuildingGateSwitchGray);
	}
	__forceinline bool IsAmplifierTower()
	{
		return (_Type == BuildingAmplifier   ||
				_Type == BuildingAmplifierL1 ||
				_Type == BuildingAmplifierL2 ||
				_Type == BuildingAmplifierL3 ||
				_Type == BuildingAmplifierL4);
	}
	__forceinline bool IsTeleportDestination()
	{
		return (_Type == BuildingTeleportRedDestination    ||
				_Type == BuildingTeleportOrangeDestination ||
				_Type == BuildingTeleportYellowDestination ||
				_Type == BuildingTeleportGreenDestination  ||
				_Type == BuildingTeleportBlueDestination);
	}
	__forceinline bool IsTeleportSource()
	{
		return (_Type == BuildingTeleportRedSource    ||
				_Type == BuildingTeleportOrangeSource ||
				_Type == BuildingTeleportYellowSource ||
				_Type == BuildingTeleportGreenSource  ||
				_Type == BuildingTeleportBlueSource);
	}
	__forceinline bool IsCircuitBoundary()
	{
		return (_Type == BuildingCircuitBoundaryBase     ||
				_Type == BuildingCircuitBoundaryBlocked);
	}

	__forceinline bool IsTrapSprung()
	{
		return (_Type == BuildingTrapSprungRed    ||
				_Type == BuildingTrapSprungOrange ||
				_Type == BuildingTrapSprungYellow ||
				_Type == BuildingTrapSprungGreen  ||
				_Type == BuildingTrapSprungBlue	  ||
				_Type == BuildingTrapSprungGray);
	}
	__forceinline bool IsTrapOpen()
	{
		return (_Type == BuildingTrapOpenRed    ||
				_Type == BuildingTrapOpenOrange ||
				_Type == BuildingTrapOpenYellow ||
				_Type == BuildingTrapOpenGreen  ||
				_Type == BuildingTrapOpenBlue	||
				_Type == BuildingTrapOpenGray);
	}
	__forceinline bool IsTrapReset()
	{
		return (_Type == BuildingTrapResetRed    ||
				_Type == BuildingTrapResetOrange ||
				_Type == BuildingTrapResetYellow ||
				_Type == BuildingTrapResetGreen  ||
				_Type == BuildingTrapResetBlue	||
				_Type == BuildingTrapResetGray);
	}

	__forceinline bool CanChangePreferenceLevel()
	{
		return (!IsPowerSource() && _Type != BuildingCircuit && _Type != BuildingBlocker);
	}
	__forceinline BuildingPuzzleType PuzzlePiece()
	{
		return _PuzzlePiece;
	}
	__forceinline bool IsHoldClass()
	{
		return (_Type == BuildingHold			  ||
				_Type == BuildingRedChargeGoal    ||
				_Type == BuildingOrangeChargeGoal ||
				_Type == BuildingYellowChargeGoal ||
				_Type == BuildingGreenChargeGoal  ||
				_Type == BuildingBlueChargeGoal);
	}
	__forceinline bool IsHoldGoal()
	{
		return (_Type == BuildingRedChargeGoal    ||
				_Type == BuildingOrangeChargeGoal ||
				_Type == BuildingYellowChargeGoal ||
				_Type == BuildingGreenChargeGoal  ||
				_Type == BuildingBlueChargeGoal);
	}
	__forceinline bool IsMegaHold()
	{
			return( _Type == BuildingMegaHoldRed ||
					_Type == BuildingMegaHoldOrange ||
					_Type == BuildingMegaHoldYellow ||
					_Type == BuildingMegaHoldGreen ||
					_Type == BuildingMegaHoldBlue ||
					_Type == BuildingMegaHoldAll );

	}

	__forceinline UINT* Fields()
	{
		return _Fields;
	}
	__forceinline UINT& TimeUntilEmission()
	{
		return _TimeUntilEmission;
	}
	__forceinline UINT& TotalChargesRemaining()
	{
		return _TotalChargesRemaining;
	}
	__forceinline UINT& TimeUntilDischarge()
	{
		return _TimeUntilDischarge;
	}

	Vec2 GetWorldLocation();
	void Discharge();
	void Move(const Coordinate &NewLocation);
	void SetChargePreference(UINT NewPreference);
	void SetPuzzlePiece(BuildingPuzzleType NewPuzzlePiece);

	UINT GetFieldCount();
	String GetFieldName(UINT Index);
	String Describe(const String &BaseDescription);

	void Upgrade(BuildingType NewType);
	bool EatCharge(Game &G, const Charge &C, bool PlaySounds);
	bool WillAcceptCharge(Game &G, const Charge &C, Building *CurrentBuilding, bool IgnoreLastChargeVisit);
	Building *FindTeleportDestination(Game &G);
	Building *FindGrayGateMatch(Game &G);
	Building *FindGrayTrapMatch(Game &G);
	Building *GetCircuitBoundaryFromGrid(Game &G, Building *GridBuilding);
	Building *GetCircuitToCircuitProjection(Game &G, Building *SourceBuilding);
	Building *GetCircuitBoundaryDestination(Game &G);
	//BuildingType GetCircuitBoundaryProxyType(Game &G);

	static Coordinate OffsetCoordinateFromArrowIndex(UINT Index);

private:

	//
	// Used by:
	// All
	//
	BuildingType _Type;
	BuildingType _InitialType;
	Coordinate _Location; // The top-left cell of this building
	Coordinate _ParentCircuit;
	int _LastChargeVisit; // Invalid on circuit
	UINT _ChargePreference;
	BuildingPuzzleType _PuzzlePiece;

	//
	// Used by:
	// BuildingCircuit
	//
	Board *_CircuitBoard;

	//
	// Used by:
	// BuildingAmplifier
	//
	UINT _AbsorbedCharge;

	//
	// Used by:
	// HoldGoal
	// MegaHold
	//
	UINT _TimeUntilDischarge;

	//
	// Used by:
	// Power sources
	//
	UINT _TimeUntilEmission;
	UINT _TotalChargesRemaining;
	
	//
	// Used by:
	// Power sources
	// HoldGoal
	//
	UINT _Fields[MaxFieldIndex];
};
