
class GameCanvas
{
public:
    GameCanvas(AppData &_app) : app(_app) {}
    void init();
    void render();

    bool backgroundDirty;

    vec2f canonicalDims;
    CoordinateFrame coordinateFrame; // the coordinate frame of rendered area inside of the window

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

    void addBackgroundObject(Texture &tex, const rect2f &rect, float depth, UIRenderType type = UIRenderType::Standard, const vec4f &color = vec4f(1.0f, 1.0f, 1.0f, 1.0f), const Component *dynamicComponent = nullptr)
    {
        backgroundObjects.push_back(UIRenderObject(tex, rect, depth, type, color, dynamicComponent));
    }

    void updateBackgroundObjects();
    void renderBuildingGrid();

    void renderComponents();
    void renderComponent(const Component &component);
    void renderCircuitComponent(const Component &component);

    void renderSpokes(const Component &component);
    void renderSpokesMiniCircuit(const Component &component);
    void renderSpokesCircuit(const Component &component);

    void renderCharge(const Charge &charge, bool trailRender);
    void renderExplodingCharge(const ExplodingCharge &charge);

    void renderMegaHold(const Component &component, bool trailRender);

    void renderChargeCircuit(const Charge &charge, bool trailRender);
    void renderExplodingChargeCircuit(const ExplodingCharge &charge);

    void renderButtonBackground(const GameButton &button, bool selected);
    void renderButtonForeground(const GameButton &button, bool selected);
    void renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, float depthOffset, const IconState &icon);

    void renderHoverComponent();

    void renderTooltip();
    void renderTooltip(const vec2f &canonicalStart, const ComponentInfo &info, const ComponentModifiers &modifiers, const Component *component);

    Texture& getFontTexture(const string &text, FontType font, int wrapWidth = 0);

    vector<UIRenderObject> backgroundObjects;

    map<string, Texture*> textCache;

    RenderTarget trailTexture0;
    RenderTarget trailTexture1;
};