
class SplashScreen : public EventHandler, public RenderHandler
{
public:
    SplashScreen(AppData &_app) : app(_app) {}

    void init();

    void mouseUp(Uint8 button, int x, int y, bool shift, bool ctrl) override;
    void keyDown(SDL_Keycode key, bool shift, bool ctrl) override;

    void render() override;

private:
    AppData &app;
};