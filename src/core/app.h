
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
	
	ComponentSet copyBuffer;
	BackBuffer backBuffer = BackBuffer();
};

class App
{
public:
    int run();

private:
    AppData data;
};
