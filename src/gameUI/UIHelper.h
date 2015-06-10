
namespace depthLayers
{
    const float selection = 0.9f;
    const float charge = 0.01f;
    const float font = 0.02f;
    const float tooltip = 0.03f;
    const float hoverComponent = 0.1f;
    const float hoverCircuitComponent = 0.09f;
    const float hoverComponentGrid = 0.05f;
    const float background = 1.0f;
    const float spokes = 0.95f;
    const float component = 0.9f;
    const float miniCircuitOffset = 0.2f;
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
