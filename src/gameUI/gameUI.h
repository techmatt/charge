
class GameUI
{
public:
    void init(RendererSDL &renderer);
	void render(RendererSDL &renderer, GameState &state);

private:
	void updateButtonList(GameState &state);

	void updateBackground(RendererSDL &renderer, GameState &state);
	void renderBuildingGrid(RendererSDL &renderer, GameState &state);
	void renderBackgroundBuildings(RendererSDL &renderer, GameState &state);
	void renderSelectionHighlight(RendererSDL &renderer);

    Texture background;
	vec2i windowDims;

	vector<GameButton> buttons;
};