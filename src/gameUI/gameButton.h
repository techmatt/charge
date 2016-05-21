
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
    LevelSelect,
    ComponentAttribute,
};

enum class ModifierKey
{
	None,
	Alt,
	Shift,
	Ctrl
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
    GameButton(const string &_name, const vec2i &menuCoord, ButtonType _type, const int _levelIndex)
    {
        component = nullptr;
        name = _name;
        type = _type;
        canonicalRect = getCanonicalRect(menuCoord, type);
        levelIndex = _levelIndex;

        initTooltip();
    }

    void leftClick(AppData &app, const vector<Component*> &selectedComponents) const;

	string tooltipHotkey() const
	{
		if (name == "CircuitCopy")
			return "C";
		if (name == "CircuitPaste")
			return "V";
		if (name == "CircuitCut")
			return "X";
		if (name == "Undo")
			return "Ctrl+Z";
		if (name == "Pause")
			return "Space";
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
	ModifierKey modifierKey;
    int levelIndex;

private:
    void initTooltip();

    static rect2i getCanonicalRect(const vec2i &menuCoord, ButtonType type)
    {
        const int sizeA = params().puzzleMenuCanonicalEntrySize;
        const int sizeB = params().componentMenuCanonicalEntrySize;
        const int sizeC = params().levelSelectMenuCanonicalEntrySize;
        if (type == ButtonType::PuzzleControlA)
        {
            const vec2i base = params().puzzleMenuACanonicalStart + menuCoord * sizeA;
            return rect2i(base, base + sizeA - 4);
        }
        else if (type == ButtonType::PuzzleControlB)
        {
            const vec2i base = params().puzzleMenuBCanonicalStart + vec2i(menuCoord.x * (sizeA - 8), 0);
            return rect2i(base, base + sizeA - 8);
        }
        else if (type == ButtonType::PuzzleControlC)
        {
            const vec2i base = params().puzzleMenuCCanonicalStart + menuCoord * sizeA;
            return rect2i(base, base + sizeA - 4);
        }
        else if (type == ButtonType::PuzzleControlD)
        {
			const vec2i base = params().puzzleMenuDCanonicalStart + vec2i(menuCoord.x * (sizeA - 2), 0);
			return rect2i(base, base + sizeA - 4);
        }
        else if (type == ButtonType::PuzzleControlE)
        {
			if (menuCoord.y == 0)
			{
				const vec2f buttonSize((sizeA - 4) * 2.0f + 1.0f, sizeA - 4.0f);
				const vec2f base = params().puzzleMenuECanonicalStart + vec2f(menuCoord.x * (buttonSize.x + 4.0f), menuCoord.y * (buttonSize.y + 4.0f));
				return rect2i(base, base + buttonSize);
			}
			else
			{
				const int w = 30, h = sizeA;
				const vec2i base = params().puzzleMenuECanonicalStart + vec2i(menuCoord.x * w, menuCoord.y * h);
				return rect2i(base, base + vec2i(w - 4, h - 4));
			}
        }
		else if (type == ButtonType::PuzzleControlF)
		{
			//const vec2i base = params().puzzleMenuFCanonicalStart + menuCoord * sizeA;
			//return rect2i(base, base + sizeA - 4);
			const vec2i base = params().puzzleMenuDCanonicalStart + vec2i(menuCoord.x * (sizeA - 2), 0);
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
        else if (type == ButtonType::LevelSelect)
        {
            const vec2i base = params().componentMenuCanonicalStart + params().menuButtonOffset + menuCoord * sizeC;
            return rect2i(base, base + (sizeC - 4) * vec2i(1, 1));
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
