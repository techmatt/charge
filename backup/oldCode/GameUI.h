enum GameUIStateType
{
	GameUIStateIdle,
	GameUIStateBuildingMask,
};

enum ClickableType
{
	ClickableBuildingMenu,
	ClickableUpgradeMenu,
	ClickablePreferenceMenu,
};

struct ClickableObject
{
	String GetTooltip(Game &G, Building *CurUpgradeBuilding);
	
	ClickableType Type;
	Rectangle2i Rect;

	//
	// Valid for:
	// ClickableBuildingMenu
	//
	BuildingType BuildingItem;

	//
	// Valid for:
	// ClickableUpgradeMenu
	// ClickablePreferenceMenu
	//
	UINT Index;
};

struct InputState
{
	InputState()
	{
		for(UINT Index = 0; Index < MaxMouseButtonCount; Index++)
		{
			MousePressed[Index] = false;
		}
		for(UINT Index = 0; Index < MaxKeyCount; Index++)
		{
			KeyPressed[Index] = false;
		}
	}
	void Load(InputManager &IM);

	Coordinate MouseScreenPos;
	bool MousePressed[MaxMouseButtonCount];
	bool KeyPressed[MaxKeyCount];
};

Rectangle2 RemapRectangle2FromExpectedToActual(const Rectangle2 &Rect, const Coordinate &WindowDimension, const Coordinate &ExpectedWindowDimension);

class GameUI
{
public:
	friend class ThatWhichRendersGames;

	void Reset(Game &G);

	void Update(Game &G, InputManager &IM, const Coordinate &WindowDimension, const Coordinate &ExpectedWindowDimension, const ChargeOptions &Options);
	Building *GetSelectedGridBuilding(Game &G);
	Building *GetCopiedCircuitBuilding(Game &G);
	Building *GetSelectedCircuitBuilding(Game &G);
	Building *GetSelectedUpgradeBuilding(Game &G);

private:
	void DeleteCurrentPiece(Game &G);
	void LeftClick(Game &G, const Coordinate &ScreenPos, bool IsTransitionClick);
	void RightClick(Game &G, const Coordinate &ScreenPos, bool IsTransitionClick);
	void MouseMove(Game &G, const Coordinate &ScreenPos, const ChargeOptions &Options);
	void KeyPress(Game &G, UINT Key);
	int GetClickableIndex(const Coordinate &ScreenPos);
	
	Vector<ClickableObject> _Clickables;

	InputState _PreviousInputState;

	GameUIStateType _CurState;
	Coordinate _CurCoordinateForBuilding;
	Coordinate _CurCoordinateForSelection;
	bool _MouseInsideCircuit;
	bool _PuzzlePieceSelected;

	Coordinate _CopiedCircuitCoordinate;

	Coordinate _SelectedBuildingGridCoordinate;
	Coordinate _SelectedBuildingCircuitCoordinate;

	BuildingType _CandidateBuilding;

	UINT _MouseHoverStart;
	int _MouseHoverClickableIndex;
	bool _ToolTipActive;
	Coordinate _ToolTipStart;
	String _ToolTipText;
	bool _ToolTipBackdrop;
};

