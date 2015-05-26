
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
};

class App
{
public:
    int run();

private:
    AppData data;
};
