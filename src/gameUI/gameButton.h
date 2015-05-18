
enum ButtonType
{
    ButtonComponent,
    ButtonUpgradeColor,
    ButtonChangeDelay,
    ButtonCircuitModifier,
    ButtonPuzzleControl,
};

struct GameButton
{
    GameButton() {}
    GameButton(const string &_name, const vec2i &menuCoord, ButtonType _type, ChargeType _chargePrimary = ChargeNone, ChargeType _chargeSecondary = ChargeNone)
    {
        component = nullptr;
        name = _name;
        chargePrimary = _chargePrimary;
        chargeSecondary = _chargeSecondary;
        type = _type;
        canonicalRect = getCanonicalRect(menuCoord, type);

        if (database().components.count(name) > 0)
            component = database().components.at(name);
    }

    void render(RendererSDL &renderer, bool selected);

    ComponentInfo *component;
    string name;
    ChargeType chargePrimary, chargeSecondary;
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
