
struct AppData
{
    AppData() : ui(*this)
    {

    }
    GameUI ui;
    GameState state;
    RendererSDL renderer;
};

class App
{
public:
    int run();

private:
    AppData data;
};
