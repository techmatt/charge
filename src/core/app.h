
struct AppData
{
    AppData() : ui(*this)
    {

    }
    GameUI ui;
    GameState state;
    //RendererOpenGL renderer;
    RendererSDL renderer;
};

class App
{
public:
    int run();

private:
    AppData data;
};
