
class SplashScreen : public EventHandler, public RenderHandler
{
public:
    SplashScreen(AppData &_app) : app(_app) {}

    void init();

    void transferToPuzzleMode(int slotIndex);

    void mouseUp(Uint8 button, int x, int y, bool shift, bool ctrl) override;
    void mouseMove(Uint32 buttonState, int x, int y) override;
    void keyDown(SDL_Keycode key, bool shift, bool ctrl) override;

    void render() override;

private:
    void bloom();

    vec3f focusColorA, focusColorB;

    vec3f focusColorATarget;

    AppData &app;

    RenderTarget bloomTexture0;
    RenderTarget bloomTexture1;
};