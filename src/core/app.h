
struct AppData
{
    AppData() : ui(*this), controller(*this)
    {
        
    }

    void playEffect(const string &name, const GameLocation &location)
    {
        //Component *selectedComponent = state.getComponent(ui.selectedGameLocation);
		Component* selectedComponent=ui.selection.singleElement();
		if (!location.inCircuit() || (selectedComponent != nullptr && selectedComponent->location.boardPos == location.boardPos) || name == "ChargeDeath")
        {
            audio.playEffect(name, location.inCircuit());
        }
    }

    GameController controller;
    GameUI ui;
    GameState state;
    PuzzleManager puzzles;
    RendererOpenGL renderer;
    //RendererSDL renderer;
    AudioDevice audio;
	
	UndoBuffer undoBuffer;
};

class App
{
public:
    int run();

private:
    AppData data;
};
