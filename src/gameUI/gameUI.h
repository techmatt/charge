
enum class GameSpeed
{
	Quarter,
	x1,
	x3,
	x5,
	x10,
	x20,
};

class GameUI : public EventHandler
{
public:
    GameUI(AppData &_app) : app(_app) {}
    void init();
    
    void mouseDown(Uint8 button, int x, int y, int clicks, bool shift, bool ctrl) override;
    void mouseUp(Uint8 button, int x, int y, int clicks, bool shift, bool ctrl) override;
    void mouseMove(Uint32 buttonState, int x, int y, bool shift, bool ctrl) override;
    void mouseWheel(int x, int y, bool shift, bool ctrl) override;
    void keyDown(SDL_Keycode key, bool shift, bool ctrl, bool alt) override;
    void keyUp(SDL_Keycode key) override;

    GameLocation hoverLocation(bool constructionOffset, const vec2f mouseOffsetFromHover = vec2f(-1.0f, -1.0f)) const;

	bool canNotBuildAtPosition(const Board &b, const ComponentDefiningProperties &cdp, const vec2i &coord) const
	{
		//TODO: check whether the component is actually buildable in this puzzle with the corresponding properties
		const BoardCell &cell = b.cells(coord);
		return (cell.c != nullptr || cell.blocked) ||
			(b.isCircuitBoard() && min(coord.x, constants::circuitBoardSize - 1 - coord.x) <= 1 && min(coord.y, constants::circuitBoardSize - 1 - coord.y) <= 1) ||
			(b.isCircuitBoard() && (cdp.baseInfo->name == "Circuit" || cdp.baseInfo->name == "FixedCircuit"));
	}

	void addHoverComponent(const GameLocation &location);

	void cycleButtonSelection(ButtonType type, int direction, bool wrap);

    void clearSelection();
	void deleteSelection();
	void cut();
	void copy();
	void paste();

	// the list of selected components
	ComponentSelection selection;

    const ComponentInfo *selectedMenuComponent;
    ChargeType selectedMenuComponentColor;

    // Various sets of stored lists of components used for placement and 
    ComponentSet copyBuffer;
    ComponentSet activePlacementBuffer;
    bool currentlyMoving;

    vec2i mouseHoverCoord;

    // the location that the person clicked.  Used for clicking and dragging.
    GameLocation clickLocation;
    vec2f clickScreenLocation;

    int hoverButtonIndex;

private:

    AppData &app;

    void removeHoverComponent();

    // true if the space key has been observed to be up. used to avoid space repeat.
    bool tabUp;
	bool leftClickUp;
	int leftClickCounter;
	bool rightClickUp;
	bool rightClickUpRequired;
};