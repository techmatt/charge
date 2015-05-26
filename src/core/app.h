
struct AppData
{
    AppData() : ui(*this)
    {

    }
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
