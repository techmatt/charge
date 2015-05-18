
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
    void renderLocalizedComponent(const ComponentInfo &info, ChargeType charge, const rect2f &screenRect);

    void renderSpokes(const Component &component);

    void renderCharge(const Charge &charge);

	void renderSelectionHighlight();

    void renderHoverComponent();

    void addHoverComponent();

    vec2i hoverBoardLocation();

    GameMode mode;
    GameSpeed speed;

    Texture background;
	vec2i windowDims;

    vec2i mouseHoverCoord;

	vector<GameButton> buttons;

    ComponentInfo *selectedMenuComponent;

    Texture* chargeTextures[constants::maxChargeLevel];
};