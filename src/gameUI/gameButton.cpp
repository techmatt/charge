
#include "main.h"

void GameButton::initTooltip()
{
    tooltip = component;
    if (tooltip == nullptr)
        return;
    hotkey = component->hotkey;
    if (hotkey.size() == 0)
        hotkey = "!";

    if (type == ButtonType::ChargePreference)
    {
        tooltip = &database().getComponent("Preference" + to_string((int)modifiers.chargePreference));
        switch ((int)modifiers.chargePreference)
        {
        case 0: hotkey = "7"; break;
        case 1: hotkey = "8"; break;
        case 2: hotkey = "9"; break;
        case 3: hotkey = "0"; break;
        case 4: hotkey = "-"; break;
        }
    }

    if (type == ButtonType::WireSpeed)
    {
        tooltip = &database().getComponent(GameUtil::speedToTextureName(modifiers.speed));
        switch ((int)modifiers.speed)
        {
        case 0: hotkey = "1"; break;
        case 1: hotkey = "2"; break;
        case 2: hotkey = "3"; break;
        case 3: hotkey = "4"; break;
        case 4: hotkey = "5"; break;
        }
    }

    if (type == ButtonType::ChargeColor)
        hotkey[0] = '0' + (int)modifiers.color;

    if (type == ButtonType::Component && modifiers.color == ChargeType::Gray)
        hotkey = database().getComponent(name + "GrayProxy").hotkey;

    if (modifiers.color == ChargeType::Gray && (component->name == "GateSwitch" || component->name == "TrapReset" || component->name == "MegaHold"))
        tooltip = &database().getComponent(component->name + "GrayProxy");

    if (type == ButtonType::TrapState || type == ButtonType::GateState || type == ButtonType::CircuitBoundary)
    {

    }
}
