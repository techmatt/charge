
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
	
	BackBuffer backBuffer;// = BackBuffer();
};

class App
{
public:
    int run();

private:
    AppData data;
};
