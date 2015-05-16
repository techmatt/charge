
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
	GameButton(const string &_textureName, const vec2i &menuCoord, ButtonType _type)
	{
		textureName = _textureName;
		type = _type;
		vec2i base = params().menuCanonicalStart + menuCoord * params().menuCanonicalEntrySize;
		canonicalRect = rect2i(base, base + params().boardCanonicalCellSize * vec2i(2, 2));
	}

	void render(RendererSDL &renderer, bool selected);

	string textureName;
	ButtonType type;
	rect2i canonicalRect;
};
