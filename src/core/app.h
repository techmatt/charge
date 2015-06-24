
struct AppData
{
    AppData() : ui(*this), canvas(*this), controller(*this), splash(*this)
    {
        
    }

    void playEffect(const string &name, const GameLocation &locationA, GameLocation locationB = GameLocation())
    {
        if (!locationB.valid())
            locationB = locationA;

        //
        // play each sound at most once per tick.
        //
        if (soundsPlayedThisTick.count(name) > 0)
            return;

        //Component *selectedComponent = state.getComponent(ui.selectedGameLocation);
		Component* selectedComponent=ui.selection.singleElement();
        if (!locationA.inCircuit() || !locationB.inCircuit() || (!soundPlayedThisSecond && activeCircuit() == nullptr) ||
            (selectedComponent != nullptr && (selectedComponent->location.boardPos == locationA.boardPos || selectedComponent->location.boardPos == locationB.boardPos)) || name == "ChargeDeath" || name == "GoalHold")
        {
            soundPlayedThisSecond = true;
            soundsPlayedThisTick.insert(name);
            audio.playEffect(name, locationA.inCircuit());
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

    EventHandler *activeEventHandler;
    RenderHandler *activeRenderHandler;

    UserSession session;

    SplashScreen splash;

    GameController controller;
    GameUI ui;
    GameCanvas canvas;
    GameState state;
    
    //RendererOpenGL renderer;
    RendererSDL renderer;
    AudioDevice audio;
	
	UndoBuffer undoBuffer;

    FrameTimer frameTimer;

    set<string> soundsPlayedThisTick;
    int soundCountTicks;
    bool soundPlayedThisSecond;
};

class App
{
public:
    int run();

private:
    AppData data;
};
