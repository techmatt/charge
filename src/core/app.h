
struct AppData
{
    AppData() : ui(*this), canvas(*this), controller(*this)
    {
        
    }

    void playEffect(const string &name, const GameLocation &location)
    {
        //
        // play each sound at most once per tick.
        // TODO: consider playing at most N times per tick
        //
        if (soundsPlayedThisTick.count(name) > 0)
            return;

        //Component *selectedComponent = state.getComponent(ui.selectedGameLocation);
		Component* selectedComponent=ui.selection.singleElement();
		if (!location.inCircuit() || (selectedComponent != nullptr && selectedComponent->location.boardPos == location.boardPos) || name == "ChargeDeath")
        {
            soundsPlayedThisTick.insert(name);
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

    set<string> soundsPlayedThisTick;
};

class App
{
public:
    int run();

private:
    AppData data;
};
