
enum UIRenderType
{
    UIRenderStandard,
    UIRenderStoredCharge,
};

struct UIRenderObject
{
    UIRenderObject(Texture &_tex, const rect2f &_rect, float _depth, float _rotation)
    {
        tex = &_tex;
        rect = _rect;
        depth = _depth;
        rotation = _rotation;
        color = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
        dynamicComponent = nullptr;
    }

    UIRenderObject(Texture &_tex, const rect2f &_rect, float _depth, UIRenderType _type = UIRenderStandard, const vec4f &_color = vec4f(1.0f, 1.0f, 1.0f, 1.0f), const Component *_dynamicComponent = nullptr)
    {
        tex = &_tex;
        rect = _rect;
        depth = _depth;
        rotation = 0.0f;
        color = _color;
        type = _type;
        dynamicComponent = _dynamicComponent;
    }

    Texture *tex;
    UIRenderType type;
    float depth;
    rect2f rect;
    float rotation;
    vec4f color;
    
    //
    // when component is not null, its texture is used instead of tex.
    //
    const Component *dynamicComponent;

};

struct IconState
{
    IconState(const ComponentModifiers &_modifiers, bool _selected, bool _background = true)
        : modifiers(_modifiers)
    {
        selected = _selected;
        background = _background;
    }
    const ComponentModifiers &modifiers;
    bool selected;
    bool background;
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


	// TODO: this should be a vector
	GameLocation selectedGameLocation;

private:

    AppData &app;

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color, const CoordinateFrame &frame);

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    void renderText(Texture &tex, const vec2f &start, const float height, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    void render(const UIRenderObject &o);

    void addBackgroundObject(Texture &tex, const rect2f &rect, float depth, float rotation)
    {
        backgroundObjects.push_back(UIRenderObject(tex, rect, depth, rotation));
    }

    void addBackgroundObject(Texture &tex, const rect2f &rect, float depth, UIRenderType type = UIRenderStandard, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f), const Component *dynamicComponent = nullptr)
    {
        backgroundObjects.push_back(UIRenderObject(tex, rect, depth, type, color, dynamicComponent));
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

    void renderButtonBackground(const GameButton &button, bool selected);
    void renderButtonForeground(const GameButton &button, bool selected);
    void renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, float depthOffset, const IconState &icon);

    void renderHoverComponent();
    void removeHoverComponent();

    void addHoverComponent();

    Texture& getFontTexture(const string &text, float height, RGBColor color);

    Component* activeCircuit() const;

    GameLocation hoverLocation(bool constructionOffset) const;

    vec2f canonicalDims;
	CoordinateFrame coordinateFrame; // the coordinate frame of rendered area inside of the window

    vec2i mouseHoverCoord;

	vector<GameButton> buttons;

    ComponentInfo *selectedMenuComponent;
    ChargeType selectedMenuComponentColor;

    bool backgroundDirty;

    vector<UIRenderObject> backgroundObjects;

    map<string, Texture*> textCache;

	// Various sets of stored lists of components used for placement and 
	ComponentSelection selectedComponents;
	ComponentSet copyBuffer;
	ComponentSet activePlacementBuffer;
	bool currentlyMoving;
};