
struct UIRenderObject
{
    UIRenderObject(Texture &_tex, const rect2f &_rect, float _depth, const Component *_dynamicComponent = nullptr, float _rotation = 0.0f)
    {
        tex = &_tex;
        rect = _rect;
        depth = _depth;
        rotation = _rotation;
        dynamicComponent = _dynamicComponent;
    }
    float depth;
    Texture *tex;
    rect2f rect;
    float rotation;

    //
    // when component is not null, its texture is used instead of tex.
    //
    const Component *dynamicComponent;

};

inline bool operator < (const UIRenderObject &a, const UIRenderObject &b)
{
    return a.depth > b.depth;
}

class GameUI
{
public:
    GameUI(AppData &_app) : app(_app) {}
    void init();
	void render();
    
    void mouseDown(Uint8 button, int x, int y);
    void mouseMove(Uint32 buttonState, int x, int y);
    void keyDown(SDL_Keycode key);

private:

    AppData &app;

    void render(Texture &tex, const rect2f &destinationRect, float depth, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const CoordinateFrame &frame);

    void render(Texture &tex, const rect2f &destinationRect, float depth);
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation);

    void render(const UIRenderObject &o);

    void addBackgroundObject(Texture &tex, const rect2f &destinationRect, float depth, const Component *dynamicComponent = nullptr, float rotation = 0.0f)
    {
        backgroundObjects.push_back(UIRenderObject(tex, destinationRect, depth, dynamicComponent, rotation));
    }

	void updateButtonList();

	void updateBackgroundObjects();
	void renderBuildingGrid();
	
    void renderComponents();
    void renderComponent(const Component &component);
	void renderCircuitComponent(const Component &component);

    void renderSpokes(const Component &component);
    void renderSpokesCircuit(const Component &component);

    void renderCharge(const Charge &charge);
    void renderExplodingCharge(const ExplodingCharge &charge);

    void renderChargeCircuit(const Charge &charge);
    void renderExplodingChargeCircuit(const ExplodingCharge &charge);

    void renderButton(const GameButton &button, bool selected);
    void renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, const ComponentModifiers &modifiers, bool selected, bool isButton, bool isBackground, float depthOffset);

    void renderHoverComponent();
    void removeHoverComponent();

    void addHoverComponent();

    Component* activeCircuit() const;

    GameLocation hoverLocation(bool constructionOffset) const;

    vec2f canonicalDims;
	CoordinateFrame coordinateFrame; // the coordinate frame of rendered area inside of the window

    vec2i mouseHoverCoord;

	vector<GameButton> buttons;

    ComponentInfo *selectedMenuComponent;

    // TODO: this should be a vector
    GameLocation selectedGameLocation;

    bool backgroundDirty;

    vector<UIRenderObject> backgroundObjects;
};