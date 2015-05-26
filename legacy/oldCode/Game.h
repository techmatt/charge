enum GameMode
{
	GameModePuzzle,
	GameModeGod,
};

//const int PowerEmissionPeriod = 120;
const int GameFramesPerSecond = 60;

class Game
{
public:
	//
	// Init
	//
	void Init(FileCollection &Data);

	//
	// Accessors
	//
	__forceinline Board& GetBoard()
	{
		return _Board;
	}
	__forceinline Vector<Building*>& Buildings()
	{
		return _Buildings;
	}
	__forceinline Vector<Charge*>& Charges()
	{
		return _Charges;
	}
	__forceinline Vector<ExplodingCharge>& ExplodingCharges()
	{
		return _ExplodingCharges;
	}
	__forceinline UINT CurrentTime()
	{
		return _CurrentTime;
	}
	__forceinline BuildingInfo* GetBuildingInfo()
	{
		return _BuildingInfo;
	}
	__forceinline GameMode GetGameMode()
	{
		return _CurMode;
	}
	__forceinline bool AttemptingPuzzle()
	{
		return _AttemptingPuzzle;
	}
	__forceinline bool AllowPreferences()
	{
		return _AllowPreferences;
	}
	__forceinline GameAudio& Audio()
	{
		return _Audio;
	}
    bool PuzzleModified()
    {
        return _Buildings.Length() != _BaseBuildingCount;
    }

	//
	// Puzzle
	//
	void ResetAll();
	void StartPuzzle();
	void StopPuzzle();
	bool PuzzleSolved();
	void SaveToPuzzleFile(const String &Filename, bool SaveAsPuzzle);
	void LoadFromPuzzleFile(const Vector<String> &Lines, bool UseLocalBuildFile, FileCollection *Data);
    void UpdateBuildingDatabase(const Vector<String> &Lines, bool InitialLoad, bool NewAllowPreferences);
    void ResetBuildingDatabase(FileCollection &Data);
	void SetPuzzleMode();
	void SetGodMode();
    void ClearAll();

	Building *BuildingFromLocation(const Coordinate &GridCoordinate);
	bool CircuitAtLocation(const Coordinate &GridCoordinate);
	Building *CircuitFromLocation(const Coordinate &GridCoordinate);
	void AddBuilding(const Coordinate &Location, const Coordinate &ParentCircuit, BuildingType Type, UINT PreferenceLevel, UINT *Fields, BuildingPuzzleType PuzzlePiece, const Coordinate &CircuitCopyLocation = Coordinate::Invalid);
	void UpgradeBuilding(Building &Bldg, BuildingType NewType);
	void RemoveBuilding(Building *Bldg);
	void Step(const Coordinate &SelectedCircuitLocation);
	__forceinline void AddCharge(const Charge &NewCharge)
	{
		_Charges.PushEnd(new Charge(NewCharge));
	}
	UINT CountChargesOfLevel(UINT ChargeLevel);
    bool QueryAndResetGameActionTaken();
    __forceinline void SetGameActionTaken()
    {
        _GameActionTaken = true;
    }

private:
	void LoadBuildingDatabase(FileCollection &Data);
	//void DischargeBuilding(Building &Bldg);
	UINT GetBuildingIndex(Building *B);

    bool _GameActionTaken;
	bool _AllowPreferences;
	bool _AttemptingPuzzle;
	GameMode _CurMode;
	bool _PowerSourcesActive;
	Board _Board;
	Vector<Building*> _Buildings;
	Vector<Charge*> _Charges;
	Vector<ExplodingCharge> _ExplodingCharges;
	UINT _CurrentTime, _BaseBuildingCount;
	BuildingInfo _DefaultBuildingInfo[BuildingCount];
	BuildingInfo _BuildingInfo[BuildingCount];
	GameAudio _Audio;
};
