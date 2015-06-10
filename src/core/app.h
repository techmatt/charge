
struct AppData
{
    AppData() : ui(*this), canvas(*this), controller(*this)
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

    Component* activeCircuit()
    {
        Component *c = ui.selection.singleElementOrCircuit(state);

        if (c != nullptr && c->info->name == "Circuit")
        {
            return c;
        }
        return nullptr;
    }

    GameController controller;
    GameUI ui;
    GameCanvas canvas;
    GameState state;
    PuzzleManager puzzles;
    RendererOpenGL renderer;
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
