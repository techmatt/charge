
enum ButtonType
{
	ButtonComponent,
	ButtonUpgradeColor,
	ButtonChangeDelay,
	ButtonCircuitModifier,
	ButtonGameSpeedControl,
};

struct GameButton
{
	GameButton() {}
  GameButton(const GameButton &g) {}
	GameButton(const string &_textureName, ChargeType _chargePrimary, ChargeType _chargeSecondary, const vec2i &menuCoord, ButtonType _type)
	{
        component = nullptr;
        textureName = _textureName;
        chargePrimary = _chargePrimary;
        chargeSecondary = _chargeSecondary;
        type = _type;
		    vec2i base = params().menuCanonicalStart + menuCoord * params().menuCanonicalEntrySize;
		    canonicalRect = rect2i(base, base + params().boardCanonicalCellSize * vec2i(2, 2));

        if (database().components.count(textureName) > 0)
        {
            component = database().components.at(textureName);
        }
	}

	void render(RendererSDL &renderer, bool selected);

    ComponentInfo *component;
	string textureName;
    ChargeType chargePrimary, chargeSecondary;
	ButtonType type;
	rect2i canonicalRect;
};
