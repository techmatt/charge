
struct AppData;

class GameUI
{
public:
    GameUI(AppData &_app) : app(_app)
    {

    }
    void init();
	void render();
    
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

	void renderSelectionHighlight();

    void renderHoverComponent();

    void addHoverComponent();

    vec2i hoverBoardLocation();

    Texture background;
	vec2i windowDims;

    vec2i mouseHoverCoord;

	vector<GameButton> buttons;

    ComponentInfo *selectedMenuComponent;
};