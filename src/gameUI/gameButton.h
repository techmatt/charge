
enum ButtonType
{
    ButtonComponent,
    ButtonChargeColor,
    ButtonWireSpeed,
    ButtonCircuitBoundary,
    ButtonChargePreference,
    ButtonPuzzleControl,
    ButtonComponentAttribute,
};

struct GameButton
{
    GameButton() {}
    GameButton(const string &_name, const vec2i &menuCoord, ButtonType _type, const ComponentModifiers &_modifiers)
    {
        component = nullptr;
        name = _name;
        type = _type;
        modifiers = _modifiers;
        canonicalRect = getCanonicalRect(menuCoord, type);

        if (database().components.count(name) > 0)
            component = database().components.at(name);
    }
    GameButton(const string &_name, const vec2i &menuCoord, ButtonType _type, const string &_text)
    {
        component = nullptr;
        name = _name;
        type = _type;
        canonicalRect = getCanonicalRect(menuCoord, type);
        text = _text;
    }

    ComponentInfo *component;
    string name;
    ButtonType type;
    rect2i canonicalRect;
    ComponentModifiers modifiers;
    string text;

private:
    static rect2i getCanonicalRect(const vec2i &menuCoord, ButtonType type)
    {
        if (type == ButtonPuzzleControl)
        {
            const vec2i base = params().puzzleMenuCanonicalStart + menuCoord * params().puzzleMenuCanonicalEntrySize;
            return rect2i(base, base + params().puzzleMenuCanonicalEntrySize - 4);
        }
        else if (type == ButtonComponentAttribute)
        {
            const vec2i base = params().attributeMenuCanonicalStart + vec2f::directProduct(menuCoord, params().attributeMenuCanonicalEntrySize);
            return rect2i(base, base + params().attributeMenuCanonicalEntrySize - 4);
        }
        else
        {
            const vec2i base = params().componentMenuCanonicalStart + menuCoord * params().componentMenuCanonicalEntrySize;
            return rect2i(base, base + params().canonicalCellSize * vec2i(2, 2));
        }
    }
};
