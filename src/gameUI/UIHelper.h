
class EventHandler
{
public:
    virtual void mouseDown(Uint8 button, int x, int y, bool shift, bool ctrl) {}
    virtual void mouseUp(Uint8 button, int x, int y, bool shift, bool ctrl) {}
    virtual void mouseMove(Uint32 buttonState, int x, int y) {}
    virtual void keyDown(SDL_Keycode key, bool shift, bool ctrl) {}
    virtual void keyUp(SDL_Keycode key) {}
};

class RenderHandler
{
public:
    virtual void render() = 0;
};

namespace depthLayers
{
    const float selection = 0.9f;
    const float charge = 0.01f;
    const float font = 0.003f;
    const float tooltip = 0.005f;
    const float hoverComponent = 0.1f;
    const float hoverCircuitComponent = 0.06f;
    const float hoverComponentGrid = 0.05f;
    const float hoverMiniCircuitOffset = 0.005f;
    const float background = 1.0f;

    const float superMegaHold = 0.5f;
    
	const float spokes = 0.95f;
    const float component = 0.9f;
    const float miniCircuitOffsetStandard = 0.2f;
}

enum class UIRenderType
{
    Standard,
    StoredCharge,
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

    UIRenderObject(Texture &_tex, const rect2f &_rect, float _depth, UIRenderType _type = UIRenderType::Standard, const vec4f &_color = vec4f(1.0f, 1.0f, 1.0f, 1.0f), const Component *_dynamicComponent = nullptr)
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
    IconState(const ComponentModifiers &_modifiers, bool _selected, bool _background = true, bool _faded = false)
        : modifiers(_modifiers)
    {
        selected = _selected;
        faded = _faded;
        background = _background;
    }
    const ComponentModifiers &modifiers;
    bool selected;
    bool background;
    bool faded;
};

inline bool operator < (const UIRenderObject &a, const UIRenderObject &b)
{
    return a.depth > b.depth;
}
