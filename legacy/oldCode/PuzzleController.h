struct PuzzleInfo
{
	PuzzleInfo() {}
	PuzzleInfo(const String &DatabaseLine);

	String Category;
	String Name;
	String PuzzleFile, PuzzleName;
	String SolutionFile, SolutionName;
	String DefinitionFile;
	String ReminderText;
	UINT StartTime, EndTime;
	bool AllowPreference;
	UINT MinChargeCount[MaxChargeLevel];
	UINT MaxChargeCount[MaxChargeLevel];
};

enum PuzzleType
{
	PuzzleDesign,
	PuzzleAttemptGame,
    PuzzleProvidedSolution,
};

void PuzzleMessageBox(const String &Message, const String &Title, Game &G, WindowManager &Window);

class PuzzleController
{
public:
	void Init(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options, InputManager &Input);
	void Step(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options, InputManager &Input, FileCollection &Data);
    void Exit(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	__forceinline bool VerifyingPuzzleList()
	{
		return _VerifyingPuzzleList;
	}
	String GetReminderText();

private:
    void CompilePuzzleList(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);

    void LoadPuzzleListFromDataFile(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	void UpdatePuzzleMenus(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	
	void LoadPuzzleBase(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	void LoadPuzzleProvidedSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	void LoadPuzzleYourSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	void UpdateCurrentPuzzle(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	
	void NextPuzzle(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	void PrevPuzzle(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	void SeeProvidedSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	void SeeYourSolution(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
    //void SaveCurrentProgress(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
    //void LoadSavedProgress(Game &G, GameUI &UI, WindowManager &Window, ChargeOptions &Options);
	
	PuzzleType _CurPuzzleType;
	UINT _CurPuzzleIndex;
	String _CurPuzzleFilename;
	String _CurPuzzleFullFilename;

    FileCollection _Collection;
	Vector<PuzzleInfo> _Puzzles;
	MenuInterface _Menu;
	bool _GodPresent, _VerifyingPuzzleList;
};