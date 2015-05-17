
struct GameState
{
	void init();

    Board board;
    vector<Component> components;
    vector<Charge> charges;
    //vector<ExplodingCharge> explodingCharges;

    int gameStep;
};
