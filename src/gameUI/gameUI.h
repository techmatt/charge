
struct AppData;

enum GameMode
{
    ModeDesign,
    ModeExecuting,
    ModePaused,
};

enum GameSpeed
{
    Speed1x,
    Speed2x,
    Speed10x,
    Speed50x,
};

inline int ticksFromSpeed(GameSpeed speed)
{
    switch (speed)
    {
    case Speed1x: return 1;
    case Speed2x: return 2;
    case Speed10x: return 10;
    case Speed50x: return 50;
    default: return 0;
    }
}

class GameUI
{
public:
    GameUI(AppData &_app) : app(_app) {}
    void init();
	void render();
    void step();
    
    void mouseDown(Uint8 button, int x, int y);
    void mouseMove(Uint32 buttonState, int x, int y);
    void keyDown(SDL_Keycode key);

private:
    AppData &app;

	void updateButtonList();

	void updateBackground();
	void renderBuildingGrid();
	
    void renderComponents(bool background);
    void renderComponent(const Component &component);

    void renderSpokes(const Component &component);
    void renderSpokesCircuit(const Component &component);

    void renderCharge(const Charge &charge);
    void renderExplodingCharge(const ExplodingCharge &charge);

    void renderChargeCircuit(const Charge &charge);
    void renderExplodingChargeCircuit(const ExplodingCharge &charge);

    void renderButton(const GameButton &button, bool selected);
    void renderLocalizedComponent(const string &name, const rect2f &screenRect, const ComponentModifiers &modifiers, bool selected, bool isButton);

    void renderHoverComponent();
    void removeHoverComponent();

    void addHoverComponent();

    Component* activeCircuit() const;

    GameLocation hoverLocation(bool constructionOffset) const;

    GameMode mode;
    GameSpeed speed;

    Texture background;
	vec2i windowDims;
	CoordinateFrame coordinateFrame; // the coordinate frame of rendered area inside of the window

    vec2i mouseHoverCoord;

	vector<GameButton> buttons;

    ComponentInfo *selectedMenuComponent;

    // TODO: this should be a vector
    GameLocation selectedGameLocation;

    bool designActionTaken;
};