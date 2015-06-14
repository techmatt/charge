
class SplashScreen : public EventHandler, public RenderHandler
{
public:
    SplashScreen(AppData &_app) : app(_app) {}

    void init();
    void render();

private:
    AppData &app;
};