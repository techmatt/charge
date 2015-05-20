
enum ButtonType
{
    ButtonComponent,
    ButtonChargeColor,
    ButtonWireSpeed,
    ButtonChargePreference,
    ButtonPuzzleControl,
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

    ComponentInfo *component;
    string name;
    ComponentModifiers modifiers;
    ButtonType type;
    rect2i canonicalRect;

private:
    static rect2i getCanonicalRect(const vec2i &menuCoord, ButtonType type)
    {
        if (type == ButtonPuzzleControl)
        {
            const vec2i base = params().puzzleMenuCanonicalStart + menuCoord * params().puzzleMenuCanonicalEntrySize;
            return rect2i(base, base + params().puzzleMenuCanonicalEntrySize - 4);
        }
        else
        {
            const vec2i base = params().componentMenuCanonicalStart + menuCoord * params().componentMenuCanonicalEntrySize;
            return rect2i(base, base + params().boardCanonicalCellSize * vec2i(2, 2));
        }
    }
};
