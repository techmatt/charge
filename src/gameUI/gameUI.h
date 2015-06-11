
class GameUI
{
public:
    GameUI(AppData &_app) : app(_app) {}
    void init();
    
    void mouseDown(Uint8 button, int x, int y);
	void mouseUp(Uint8 button, int x, int y);
	void mouseMove(Uint32 buttonState, int x, int y);
    void keyDown(SDL_Keycode key);

    GameLocation hoverLocation(bool constructionOffset, const vec2f mouseOffsetFromHover = vec2f(-1.0f, -1.0f)) const;

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

    // TODO: consider moving this to Board
    bool canNotBuildAtPosition(const Board &b, const ComponentDefiningProperties &cdp, const vec2i &coord) const
    {
        //TODO: check whether the component is actually buildable in this puzzle with the corresponding properties
        const BoardCell &cell = b.cells(coord);
        return (cell.c != nullptr || cell.blocked) ||
            (b.isCircuitBoard() && min(coord.x, constants::circuitBoardSize - 1 - coord.x) <= 1 && min(coord.y, constants::circuitBoardSize - 1 - coord.y) <= 1) ||
            (b.isCircuitBoard() && (cdp.baseInfo->name == "Circuit" || cdp.baseInfo->name == "FixedCircuit"));
    }

private:

    AppData &app;

    void removeHoverComponent();
    void addHoverComponent();
};