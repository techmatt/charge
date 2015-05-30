
struct DialogButton
{
    DialogButton() {}
    DialogButton(const string &_name, const rect2f &_dialogDims)
    {
        name = _name;
        dialogDims = _dialogDims;
    }

    string name;

    // TODO: this can be a CoordinateFrame
    rect2f dialogDims;
};

class Dialog
{
public:
    void addButton(const string &_name)
    {
        const rect2f rect(50, 50, 250, 75);
        const float spacing = 40.0f;


    }

    // TODO: this can be a CoordinateFrame
    rect2f canonicalRect;

    vector<DialogButton> buttons;
};

class DialogMainMenu : public Dialog
{
public:
    DialogMainMenu()
    {
        //buttons.push_back(DialogButton("Save Puzzle", rect2f(rect.min() + vec2f(0.0f, spacing * buttons.size()), rect.max() + vec2f(0.0f, spacing * buttons.size()));
        //buttons.push_back(DialogButton("Quit"));
    }
};