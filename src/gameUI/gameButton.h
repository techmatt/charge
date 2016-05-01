
enum class ButtonType
{
    Component,
    ChargeColor,
    WireSpeed,
    CircuitBoundary,
    GateState,
    TrapState,
    ChargePreference,
    PuzzleControlA,
    PuzzleControlB,
    PuzzleControlC,
    PuzzleControlD,
    PuzzleControlE,
	PuzzleControlF,
    ComponentAttribute,
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

        initTooltip();
    }
    GameButton(const string &_name, const vec2i &menuCoord, ButtonType _type, const string &_text)
    {
        component = nullptr;
        name = _name;
        type = _type;
        canonicalRect = getCanonicalRect(menuCoord, type);
        text = _text;

        initTooltip();
    }

    void leftClick(AppData &app, Component *selectedComponent) const;

	string tooltipHotkey() const
	{
		if (name == "CircuitCopy")
			return "C";
		if (name == "CircuitPaste")
			return "V";
		return hotkey;
	}

    const ComponentInfo *component;
    string name;
    ButtonType type;
    rect2i canonicalRect;
    ComponentModifiers modifiers;
    string text;

    const ComponentInfo *tooltip;
    string hotkey;
    SDL_Keycode hotkeyCode;

private:
    void initTooltip();

    static rect2i getCanonicalRect(const vec2i &menuCoord, ButtonType type)
    {
        const int sizeA = params().puzzleMenuCanonicalEntrySize;
        const int sizeB = params().componentMenuCanonicalEntrySize;
        if (type == ButtonType::PuzzleControlA)
        {
            const vec2i base = params().puzzleMenuACanonicalStart + menuCoord * sizeA;
            return rect2i(base, base + sizeA - 4);
        }
        else if (type == ButtonType::PuzzleControlB)
        {
            const vec2i base = params().puzzleMenuBCanonicalStart + vec2i(menuCoord.x * (sizeA - 4), 0);
            return rect2i(base, base + sizeA - 4);
        }
        else if (type == ButtonType::PuzzleControlC)
        {
            const vec2i base = params().puzzleMenuCCanonicalStart + menuCoord * sizeA;
            return rect2i(base, base + sizeA - 4);
        }
        else if (type == ButtonType::PuzzleControlD)
        {
            const vec2i base = params().puzzleMenuDCanonicalStart + vec2i(menuCoord.x * (sizeA - 4), 0);
            return rect2i(base, base + sizeA - 4);
        }
        else if (type == ButtonType::PuzzleControlE)
        {
            const vec2f buttonSize((sizeA - 4) * 2.0f, sizeA - 4.0f);
            const vec2f base = params().puzzleMenuECanonicalStart + vec2f(menuCoord.x * (buttonSize.x + 4.0f), menuCoord.y * (buttonSize.y + 4.0f));
            return rect2i(base, base + buttonSize);
        }
		else if (type == ButtonType::PuzzleControlF)
		{
			const vec2i base = params().puzzleMenuFCanonicalStart + menuCoord * sizeA;
			return rect2i(base, base + sizeA - 4);
		}
        else if (type == ButtonType::ComponentAttribute)
        {
            const vec2i base = params().attributeMenuCanonicalStart + vec2f::directProduct(menuCoord, params().attributeMenuCanonicalEntrySize);
            return rect2i(base, base + params().attributeMenuCanonicalEntrySize - 4);
        }
        else if (type == ButtonType::Component)
        {
            const vec2i base = params().componentMenuCanonicalStart + params().menuButtonOffset + vec2i::directProduct(menuCoord, vec2i(sizeB + 1, sizeB));
            return rect2i(base, base + params().canonicalCellSize * vec2i(2, 2));
        }
        else if (type == ButtonType::ChargePreference)
        {
            const vec2i base = params().affinityMenuCanonicalStart + params().menuButtonOffset + menuCoord * sizeB;
            return rect2i(base, base + params().canonicalCellSize * vec2i(2, 2));
        }
        else if (type == ButtonType::GateState || type == ButtonType::TrapState)
        {
            const vec2i base = params().doorMenuCanonicalStart + params().menuButtonOffset + menuCoord * sizeB;
            return rect2i(base, base + params().canonicalCellSize * vec2i(2, 2));
        }
        else if (type == ButtonType::WireSpeed || type == ButtonType::ChargeColor || type == ButtonType::CircuitBoundary)
        {
            const vec2i base = params().typeMenuCanonicalStart + params().menuButtonOffset + menuCoord * sizeB;
            return rect2i(base, base + params().canonicalCellSize * vec2i(2, 2));
        }
        return rect2i(params().componentMenuCanonicalStart, params().componentMenuCanonicalStart + params().canonicalCellSize * vec2i(2, 2));
    }
};
