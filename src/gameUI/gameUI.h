
class GameUI
{
public:
    GameUI(AppData &_app) : app(_app) {}
    void init();
	void render();
    
    void mouseDown(Uint8 button, int x, int y);
	void mouseUp(Uint8 button, int x, int y);
	void mouseMove(Uint32 buttonState, int x, int y);
    void keyDown(SDL_Keycode key);


	// the list of selected components
	ComponentSelection selection;

    bool backgroundDirty;

private:

    AppData &app;

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color, const CoordinateFrame &frame);

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    void renderText(Texture &tex, const vec2f &start, const float height, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    void render(const UIRenderObject &o);

    void renderTrails();

    void addBackgroundObject(Texture &tex, const rect2f &rect, float depth, float rotation)
    {
        backgroundObjects.push_back(UIRenderObject(tex, rect, depth, rotation));
    }

    void addBackgroundObject(Texture &tex, const rect2f &rect, float depth, UIRenderType type = UIRenderStandard, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f), const Component *dynamicComponent = nullptr)
    {
        backgroundObjects.push_back(UIRenderObject(tex, rect, depth, type, color, dynamicComponent));
    }

	void updateBackgroundObjects();
	void renderBuildingGrid();
	
    void renderComponents();
    void renderComponent(const Component &component);
	void renderCircuitComponent(const Component &component);

    void renderSpokes(const Component &component);
    void renderSpokesCircuit(const Component &component);

    void renderCharge(const Charge &charge, bool trailRender);
    void renderExplodingCharge(const ExplodingCharge &charge);

    void renderChargeCircuit(const Charge &charge, bool trailRender);
    void renderExplodingChargeCircuit(const ExplodingCharge &charge);

    void renderButtonBackground(const GameButton &button, bool selected);
    void renderButtonForeground(const GameButton &button, bool selected);
    void renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, float depthOffset, const IconState &icon);

    void renderHoverComponent();
    void removeHoverComponent();

    void renderTooltip();
    void renderTooltip(const vec2f &canonicalStart, const ComponentInfo &info, const ComponentIntrinsics &intrinsics);

    void addHoverComponent();

    Texture& getFontTexture(const string &text, FontType font);

    Component* activeCircuit() const;

	GameLocation hoverLocation(bool constructionOffset, vec2f mouseOffsetFromHover = vec2f(-1.0f, -1.0f)) const;

    vec2f canonicalDims;
	CoordinateFrame coordinateFrame; // the coordinate frame of rendered area inside of the window

    vec2i mouseHoverCoord;

    const ComponentInfo *selectedMenuComponent;
    ChargeType selectedMenuComponentColor;

    vector<UIRenderObject> backgroundObjects;

    map<string, Texture*> textCache;

    RenderTarget trailTexture0;
    RenderTarget trailTexture1;

	// Various sets of stored lists of components used for placement and 
	ComponentSelection selectedComponents;
	ComponentSet copyBuffer;
	ComponentSet activePlacementBuffer;
	bool currentlyMoving;

	// the location that the person clicked.  Used for clicking and dragging.
	GameLocation clickLocation;
	vec2f clickScreenLocation;

    // TODO: consider moving this to Board
	bool canNotBuildAtPosition(const Board &b, const ComponentDefiningProperties &cdp, const vec2i &coord) const
	{
		//TODO: check whether the component is actually buildable in this puzzle with the corresponding properties
		const BoardCell &cell = b.cells(coord);
		return (cell.c != nullptr || cell.blocked) ||
			(b.isCircuitBoard() && min(coord.x, constants::circuitBoardSize - 1 - coord.x) <= 1 && min(coord.y, constants::circuitBoardSize - 1 - coord.y) <= 1) ||
			(b.isCircuitBoard() && (cdp.baseInfo->name == "Circuit" || cdp.baseInfo->name == "FixedCircuit"));
	}
};