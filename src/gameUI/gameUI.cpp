
#include "main.h"

void GameUI::init(RendererSDL &renderer)
{

}

void GameUI::render(RendererSDL &renderer, GameState &state)
{
	updateBackground(renderer, state);

	renderer.setDefaultRenderTarget();

	renderer.render(background, 0, 0);
}

void GameUI::updateButtonList(GameState &state)
{
	buttons.clear();
	for (auto &p : database().components)
	{
        const ComponentInfo &info = *p.second;
        if (info.menuCoordinate.x != -1)
		{
            if (info.colorUpgrades)
            {
                buttons.push_back(GameButton(info.name + "Red", info.menuCoordinate, ButtonType::ButtonComponent));
            }
            else
            {
                buttons.push_back(GameButton(info.name, info.menuCoordinate, ButtonType::ButtonComponent));
            }
		}
	}
}

void GameUI::updateBackground(RendererSDL &renderer, GameState &state)
{
	//
	// resize the background if needed
	//
	windowDims = renderer.getWindowSize();
	if (vec2i((int)background.bmp().width(), (int)background.bmp().height()) != windowDims)
    {
		cout << "Resizing render target: " << windowDims << endl;
		background.loadRenderTarget(windowDims.x, windowDims.y);
    }

	updateButtonList(state);

	renderer.setRenderTarget(background);

	renderer.render(database().getTexture(renderer, "Background"), rect2i(0, 0, windowDims.x, windowDims.y));

	renderBuildingGrid(renderer, state);

	for (auto &button : buttons)
	{
		button.render(renderer, false);
	}
    
	renderer.setDefaultRenderTarget();
}

void GameUI::renderBuildingGrid(RendererSDL &renderer, GameState &state)
{
	Texture &border = database().getTexture(renderer, "Border");
	for (auto &cell : state.board.cells)
	{
		if (cell.value.c == nullptr && !cell.value.blocked)
		{
			const vec2i canonicalBase = params().boardCanonicalStart + vec2i(cell.x, cell.y) * params().boardCanonicalCellSize;
			const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + vec2i(params().boardCanonicalCellSize, params().boardCanonicalCellSize));
			const rect2f screenRect = GameUtil::canonicalToWindow(windowDims, canonicalRect);
			renderer.render(border, screenRect);
		}
	}
}
