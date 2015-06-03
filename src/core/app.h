
struct AppData
{
    AppData() : ui(*this), controller(*this)
    {
        
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
