
struct CanvasConnection
{
    CanvasConnection() {}
    CanvasConnection(int _type, vec2i _offset, int _connectorIndex, Texture *_tex)
    {
        type = _type; offset = _offset; connectorIndex = _connectorIndex; tex = _tex;
    }
    int type;
    vec2i offset;
    int connectorIndex;
    Texture *tex;
};

struct MenuTextEntry
{
    string text;
    vec2f coord;
};

class GameCanvas : public RenderHandler
{
public:
    GameCanvas(AppData &_app) : app(_app) {}
    void init();
    void render() override;

    bool backgroundDirty;
    bool errorResetBuffer;

    vec2f canonicalDims;
    CoordinateFrame coordinateFrame; // the coordinate frame of rendered area inside of the window

private:

    AppData &app;

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color, const CoordinateFrame &frame);
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color, const CoordinateFrame &frame);

    void render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    void render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    void renderText(Texture &tex, const vec2f &start, const float height, float depth = depthLayers::fontStandard);

    void render(const UIRenderObject &o);

    void renderTrails();

    void addBackgroundObject(Texture &tex, const rect2f &rect, float depth, float rotation)
    {
        backgroundObjects.push_back(UIRenderObject(tex, rect, depth, rotation));
    }

    void addBackgroundObject(Texture &tex, const rect2f &rect, float depth, UIRenderType type = UIRenderType::Standard, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f), const Component *dynamicComponent = nullptr)
    {
        backgroundObjects.push_back(UIRenderObject(tex, rect, depth, type, color, dynamicComponent));
    }

    void updateBackgroundObjects();
    void renderMenuBackground(const string &menuName, const string &menuTextureSuffix, const vec2f &canonicalStartCoord, const vec2i &gridDimensions, vec2f extension = vec2f(0.0f, 0.0f));
    void renderMenuBackground(const string &menuName, const string &menuTextureSuffix, const vec2f &canonicalStartCoord, const rect2f &rect);
    void renderBuildingGrid();

    void renderComponents();
    void renderComponent(const Component &component);
    void renderCircuitComponent(const Component &component);

    void renderSpokes(const Component &component);
    void renderSpokesMiniCircuit(const Component &component);
    void renderSpokesCircuit(const Component &component);

    void renderCharge(const Charge &charge, bool trailRender);
    void renderChargeCircuit(const Charge &charge, bool trailRender);

    void renderExplodingCharge(const ExplodingCharge &charge);
    void renderExplodingChargeCircuit(const ExplodingCharge &charge);

    void renderMegaHold(const Component &component, bool trailRender);
    void renderMegaHoldCircuit(const Component &component, bool trailRender);

    void renderSuperMegaHold(const Component &component);
	void renderTutorial();

    void renderButtonBackground(const GameButton &button, bool selected);
    void renderButtonForeground(const GameButton &button, bool selected);
    void renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, float depthOffset, const IconState &icon);
    void renderLocalizedComponentHover(const string &name, const rect2f &screenRect, float depth, const IconState &icon);

    void renderHoverComponent();

    void renderTooltip();
    void renderTooltip(const vec2f &canonicalStart, const string &title, const string &description, const ComponentModifiers &modifiers, const string &hotkey, const Component *component, bool error, bool transparent);
    void renderVictoryPanel();

    Texture& getFontTexture(const string &text, FontType font, int wrapWidth = 0);

    vector<UIRenderObject> backgroundObjects;
    vector<MenuTextEntry> menuText;

    map<string, Texture*> textCache[(int)FontType::Count];

    RenderTarget trailTexture0;
    RenderTarget trailTexture1;

    CanvasConnection connectionClasses[constants::wireConnectorClasses];
};