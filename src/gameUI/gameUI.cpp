
#include "main.h"

void GameUI::init()
{
    selectedMenuComponent = nullptr;
}

void GameUI::render()
{
	updateBackground();

    app.renderer.setDefaultRenderTarget();

    app.renderer.render(background, 0, 0);
}

void GameUI::updateButtonList()
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

void GameUI::updateBackground()
{
	//
	// resize the background if needed
	//
    windowDims = app.renderer.getWindowSize();
	if (vec2i((int)background.bmp().width(), (int)background.bmp().height()) != windowDims)
    {
		cout << "Resizing render target: " << windowDims << endl;
		background.loadRenderTarget(windowDims.x, windowDims.y);
    }

	updateButtonList();

    app.renderer.setRenderTarget(background);

    app.renderer.render(database().getTexture(app.renderer, "Background"), rect2i(0, 0, windowDims.x, windowDims.y));

	renderBuildingGrid();

    renderComponents(true);

	for (auto &button : buttons)
	{
        button.render(app.renderer, false);
	}
    
    app.renderer.setDefaultRenderTarget();
}

void GameUI::renderBuildingGrid()
{
    Texture &border = database().getTexture(app.renderer, "Border");
    for (auto &cell : app.state.board.cells)
	{
		if (cell.value.c == nullptr && !cell.value.blocked)
		{
			const vec2i canonicalBase = params().boardCanonicalStart + vec2i(cell.x, cell.y) * params().boardCanonicalCellSize;
			const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + vec2i(params().boardCanonicalCellSize, params().boardCanonicalCellSize));
			const rect2f screenRect = GameUtil::canonicalToWindow(windowDims, canonicalRect);
            app.renderer.render(border, screenRect);
		}
	}
}

void GameUI::renderComponent(const Component &component)
{
    Texture &baseTex = database().getTexture(app.renderer, "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, component.info->name, component.charge);

    if (!component.location.inCircuit())
    {
        const vec2i canonicalBase = params().boardCanonicalStart + component.location.pos * params().boardCanonicalCellSize;
        const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + 2 * vec2i(params().boardCanonicalCellSize, params().boardCanonicalCellSize));
        const rect2f screenRect = GameUtil::canonicalToWindow(windowDims, canonicalRect);
        app.renderer.render(baseTex, screenRect);
        app.renderer.render(componentTex, screenRect);
    }
}

void GameUI::renderComponents(bool background)
{
    for (auto &component : app.state.components)
    {
        if (component.info->background == background)
            renderComponent(component);
    }
}

void GameUI::mouseDown(Uint8 button, int x, int y)
{
    for (const auto &button : buttons)
    {
        //const rect2f screenRect = GameUtil::canonicalToWindow(app.renderer.getWindowSize(), canonicalRect);
    }
}

void GameUI::mouseMove(Uint32 buttonState, int x, int y)
{

}

void GameUI::keyDown(SDL_Keycode key)
{

}