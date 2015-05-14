class Game;

//#define DEBUG_CHARGE_PATH

const int ChargeRequiredTimeDifference = 11;
const int ChargeTransitTime = 9;
const UINT MaxChargeLevel = 5;

class Charge
{
public:
	friend class ThatWhichRendersGames;
	friend class Game;
	friend class Building;

	Charge() {}
	Charge(const Coordinate &SourceLocation, const Coordinate &SourceCircuit, const Coordinate &CameFromLocation, const Coordinate &CameFromCircuit, UINT Level);
	bool Update(Game &G, bool SecondPass, bool &ShowDeathAnimation, Building* &SourceBuilding, Building* &DestBuilding, const Coordinate &SelectedCircuitLocation);
	__forceinline UINT Level() const
	{
		return _Level;
	}
    __forceinline float RandomValue() const
    {
        return _RandomValue;
    }

	static RGBColor ColorFromLevel(UINT Level);

private:
	double ComputePreference(Game &G, Building &TargetBuilding);
	bool FindNewDestination(Game &G, bool &ShowDeathAnimation, Building *CurrentBuilding, Building *PreviousBuilding);
	void SetNewDestination(Game &G, Building *NewDestination);

	WorldLocation _Source;
	WorldLocation _Destination;
	UINT _TimeInTransit, _TotalTransitTime, _Level;
  float _RandomValue;
};
