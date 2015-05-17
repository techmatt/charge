
#include "main.h"

void GameUI::init()
{
    selectedMenuComponent = nullptr;
}

void GameUI::keyDown(SDL_Keycode key)
{
    if (key == SDLK_ESCAPE)
    {
        selectedMenuComponent = nullptr;
    }
}

void GameUI::mouseDown(Uint8 button, int x, int y)
{
    if (button == SDL_BUTTON_RIGHT)
    {
        selectedMenuComponent = nullptr;
    }

    if (button == SDL_BUTTON_LEFT)
    {
        addHoverComponent();
    }

    for (const auto &button : buttons)
    {
        const rect2f screenRect = GameUtil::canonicalToWindow(app.renderer.getWindowSize(), button.canonicalRect);
        if (screenRect.intersects(vec2f((float)x, (float)y)))
        {
            if (button.type == ButtonComponent)
            {
                selectedMenuComponent = button.component;
            }
        }
    }
}

void GameUI::mouseMove(Uint32 buttonState, int x, int y)
{
    mouseHoverCoord = vec2i(x, y);

    if (buttonState & SDL_BUTTON_LMASK)
    {
        addHoverComponent();
    }
}

void GameUI::addHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    const vec2i boardLocation = hoverBoardLocation();
    if (boardLocation == constants::invalidCoord)
        return;

    if (app.state.board.coordValidForNewComponent(boardLocation))
    {
        Component *newComponent = new Component(selectedMenuComponent->name, ChargeRed, GameLocation(boardLocation));
        app.state.addNewComponent(newComponent);
    }
}

void GameUI::render()
{
	updateBackground();

    app.renderer.setDefaultRenderTarget();

    app.renderer.render(background, 0, 0);

    renderHoverComponent();
}

vec2i GameUI::hoverBoardLocation()
{
    const vec2f boardCoordf = GameUtil::windowToBoard(windowDims, mouseHoverCoord);
    const vec2i boardCoordi(math::round(boardCoordf) - vec2i(1, 1));

    if (!math::between(boardCoordi.x, 0, 22) || !math::between(boardCoordi.y, 0, 22))
        return constants::invalidCoord;

    return boardCoordi;
}
void GameUI::renderHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    const vec2i boardLocation = hoverBoardLocation();
    if (boardLocation == constants::invalidCoord)
        return;
    
    const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, boardLocation, 2);
    renderLocalizedComponent(*selectedMenuComponent, ChargeRed, screenRect);
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
                buttons.push_back(GameButton(info.name, "Red", info.menuCoordinate, ButtonType::ButtonComponent));
            }
            else
            {
                buttons.push_back(GameButton(info.name, "", info.menuCoordinate, ButtonType::ButtonComponent));
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
        button.render(app.renderer, (button.component == selectedMenuComponent));
	}
    
    app.renderer.setDefaultRenderTarget();
}

void GameUI::renderBuildingGrid()
{
    Texture &border = database().getTexture(app.renderer, "Border");
    for (auto &cell : app.state.board.cells)
	{
		//if (cell.value.c == nullptr && !cell.value.blocked)
        if (!cell.value.blocked)
		{
			const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, vec2i(cell.x, cell.y), 1);
            app.renderer.render(border, screenRect);
		}
	}
}

void GameUI::renderLocalizedComponent(const ComponentInfo &info, ChargeType charge, const rect2f &screenRect)
{
    Texture &baseTex = database().getTexture(app.renderer, "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, info.name, charge);

    app.renderer.render(baseTex, screenRect);
    app.renderer.render(componentTex, screenRect);
}

void GameUI::renderComponent(const Component &component)
{
    if (!component.location.inCircuit())
    {
        const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, component.location.boardPos, 2);
        renderLocalizedComponent(*component.info, component.charge, screenRect);
    }
}

void GameUI::renderComponents(bool background)
{
    for (auto &component : app.state.components)
    {
        if (component->info->background == background)
            renderComponent(*component);
    }
}
