struct BuildingInfo
{
	BuildingInfo();
	BuildingInfo(const String &DatabaseLine);

	String Name, Description;
	UINT MaxBuildable, MaxMP;
	Coordinate MenuCoordinate;
	UINT Hotkey;
	bool Circuit;
	Vector<BuildingType> Upgrades;
    bool BuildableInPuzzle;
};

struct ExplodingCharge
{
	ExplodingCharge() {}
	ExplodingCharge(const Coordinate &_Location, const Coordinate &_ParentCircuit, UINT _TotalFrames, UINT _ChargeLevel)
	{
		Location = _Location;
		CurrentFrame = 0;
		TotalFrames = _TotalFrames;
		ChargeLevel = _ChargeLevel;
		ParentCircuit = _ParentCircuit;
	}

	Coordinate Location;
	Coordinate ParentCircuit;
	UINT CurrentFrame, TotalFrames;
	UINT ChargeLevel;
};

enum ToolTipBehaviorType
{
	ToolTipBehaviorImmediate,
	ToolTipBehavior1s,
	ToolTipBehaviorNone,
};

enum ChargeLevelState
{
    ChargeLevelSolved,
    ChargeLevelUnSolved,
};

struct ChargeLevelInfo
{
    ChargeLevelState CurState;
    UINT PiecesUsed;
    UINT UpgradesUsed;
    UINT SolveTime;
};

struct ChargeOptions
{
	void SaveToFile();
	void LoadFromFile();
    void RegisterSkippedPuzzle(UINT PuzzleIndex);
    void RegisterPuzzleSolved(UINT PuzzleIndex);
    bool PuzzleSkipped(UINT PuzzleIndex);
    bool PuzzleSolutionViewable(UINT PuzzleIndex);
    bool PuzzleAttemptable(UINT PuzzleIndex);
    UINT PuzzlesSkipped();

    static const int MaxSkippedLevels = 3;
    static const int MaxLevels = 1000;

	int HighestPuzzleIndexSolved;
    int SkippedLevels[MaxSkippedLevels];
    //ChargeLevelInfo LevelInfo[MaxLevels];

	ToolTipBehaviorType ToolTip;
	UINT AutoAdvanceLevels;
    UINT ShowFPS;
    UINT PlaySounds;
    UINT PlayMusic;
	bool Paused;
};

void GetThickLineVertices(const Vec3 &Pt1, float Radius1, const Vec3 &Pt2, float Radius2, Vec3 &V0, Vec3 &V1, Vec3 &V2, Vec3 &V3);