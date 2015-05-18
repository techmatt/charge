
struct ChargeUpdateResult
{
    ChargeUpdateResult()
    {
        destroyCharge = false;
        showDeathAnimation = false;
    }
    bool destroyCharge;
    bool showDeathAnimation;
};

struct Charge
{
    Charge(const GameLocation &originLocation, ChargeType _level);

    //
    // advance the charge towards its destination
    //
    void advance(GameState &state);

    //
    // if the charge has reached its destination, process the component
    //
    ChargeUpdateResult interactWithDestination(GameState &state);

    //
    // if the charge has reached its destination, find a new component or destroy the charge if none can be found
    //
    ChargeUpdateResult updateDestination(GameState &state);

    bool findBestDestination(GameState &state);
    void setNewDestination(GameState &state, Component &newDestination);
    
    double computePreference(GameState &state, Component &targetComponent);

    ChargeType level;
    GameLocation source, destination;
    int timeInTransit, totalTransitTime;
    float randomRotationOffset;
};
