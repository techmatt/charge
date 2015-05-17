
struct GameState
{
	void init();
    void addNewComponent(Component *component);

    Board board;
    vector<Component*> components;
    vector<Charge> charges;
    //vector<ExplodingCharge> explodingCharges;

    int gameStep;
};
