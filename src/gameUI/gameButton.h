
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
	GameButton(const string &textureNameBase, const string &textureNameSuffix, const vec2i &menuCoord, ButtonType _type)
	{
        component = nullptr;
        if (database().components.count(textureNameBase) > 0)
        {
            component = database().components.at(textureNameBase);
        }
        textureName = textureNameBase + textureNameSuffix;
		type = _type;
		vec2i base = params().menuCanonicalStart + menuCoord * params().menuCanonicalEntrySize;
		canonicalRect = rect2i(base, base + params().boardCanonicalCellSize * vec2i(2, 2));
	}

	void render(RendererSDL &renderer, bool selected);

    ComponentInfo *component;
	string textureName;
	ButtonType type;
	rect2i canonicalRect;
};
