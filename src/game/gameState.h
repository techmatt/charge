
struct GameState
{
	void init();

    Board board;
    vector<Component> component;
    vector<Charge> charges;
    //vector<ExplodingCharge> explodingCharges;

    int gameStep;
};
