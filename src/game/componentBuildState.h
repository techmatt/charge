
struct ComponentBuildState
{
    ComponentBuildState()
    {

    }

    ComponentBuildState(const ParameterTable &table)
    {
        for (auto &p : table.params)
        {
            components.insert(p.first);
        }
    }

    void loadDefaults()
    {
        components.clear();

        components.insert("Wire");
        components.insert("Splitter");
        components.insert("Amplifier");

        components.insert("FilteredAmplifierOrange");
        components.insert("FilteredAmplifierYellow");
        components.insert("FilteredAmplifierGreen");
        components.insert("FilteredAmplifierBlue");

        recordColoredComponent("TeleportSource", false);
        recordColoredComponent("TeleportDestination", false);
        recordColoredComponent("ChargeFilter", false);
        recordColoredComponent("GateClosed", true);
        recordColoredComponent("GateSwitch", true);
        recordColoredComponent("TrapOpen", true);
        recordColoredComponent("TrapReset", true);


        components.insert("Circuit");
        components.insert("Hold");
    }

    bool canBuild(const string &componentName, const ComponentModifiers &modifiers)
    {
        return components.count(annotateComponent(componentName, modifiers)) > 0;
    }

    void setBuild(const string &componentName, const ComponentModifiers &modifiers, bool newState)
    {
        const string annotation = annotateComponent(componentName, modifiers);
        components.erase(annotation);
        if (newState)
            components.insert(annotation);
    }

    ParameterTable toTable() const
    {
        ParameterTable result("ComponentBuildState");

        for (const string &s : components)
            result.setString(s, "1");
        
        return result;
    }
    
    set<string> components;

private:
    void recordColoredComponent(const string &componentName, bool gray)
    {
        for (int chargeLevel = (int)ChargeRed; chargeLevel <= (int)ChargeBlue; chargeLevel++)
            components.insert(componentName + GameUtil::suffixFromCharge((ChargeType)chargeLevel));
        if (gray)
            components.insert(componentName + GameUtil::suffixFromCharge(ChargeGray));
    }

    string annotateComponent(const string &componentName, const ComponentModifiers &modifiers)
    {
        return componentName + GameUtil::suffixFromCharge(modifiers.color);
    }
};
