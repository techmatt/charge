
class GameUI
{
public:
    void init(RendererSDL &renderer);
	void render(RendererSDL &renderer, GameState &state);

private:
	void updateBackground(RendererSDL &renderer, GameState &state);
	void renderBuildingGrid(RendererSDL &renderer, GameState &state);
	void renderBackgroundBuildings(RendererSDL &renderer, GameState &state);

    Texture background;

	Texture *borderTex;
	
};