
#include "main.h"

void GameUI::init(RendererSDL &renderer)
{
	borderTex = &database().getTexture(renderer, "Border");
}

void GameUI::render(RendererSDL &renderer, GameState &state)
{
	updateBackground(renderer, state);

	renderer.setDefaultRenderTarget();

	renderer.render(background, 0, 0);
}

void GameUI::updateBackground(RendererSDL &renderer, GameState &state)
{
	//
	// resize the background if needed
	//
	vec2i dims = renderer.getWindowSize();
    if (vec2i((int)background.bmp().width(), (int)background.bmp().height()) != dims)
    {
        cout << "Resizing render target: " << dims << endl;
        background.loadRenderTarget(dims.x, dims.y);
    }

	renderer.setRenderTarget(background);

	renderer.render(database().getTexture(renderer, "Background"), rect2i(0, 0, dims.x, dims.y));

	renderBuildingGrid(renderer, state);
    
	renderer.setDefaultRenderTarget();
}

void GameUI::renderBuildingGrid(RendererSDL &renderer, GameState &state)
{
	for (auto &cell : state.board.cells)
	{
		if (cell.value.c == nullptr && !cell.value.blocked)
		{
			vec2i base = params().boardCanonicalStart + vec2i(cell.x, cell.y) * params().boardCanonicalCellSize;
			renderer.render(*borderTex, rect2i(base, base + vec2i(params().boardCanonicalCellSize, params().boardCanonicalCellSize)));
		}
	}
}
