
struct ChargeUpdateResult
{
    Component *source;
    Component *destination;
    bool destroyCharge;
    bool showDeathAnimation;
};

struct Charge
{
    Charge(const GameLocation &originLocation, ChargeType _level);

    ChargeUpdateResult update(GameState &state, bool secondPass);

    bool findNewDestination(GameState &state, Component &current, Component &previous);
    void setNewDestination(GameState &state, Component &newDestination);
    
    double computePreference(GameState &state, Component &targetComponent);

    ChargeType level;
    GameLocation source, destination;
    int timeInTransit, totalTransitTime;
    float randomRotationOffset;
};
