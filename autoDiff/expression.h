
struct Binding
{
    map<string, double> values;
};

struct Expression
{
    virtual void print() = 0;
    virtual double evaluate(const Binding &binding) = 0;
};

struct Variable : public Expression
{
    string name;
};

struct Vec2 : public Expression
{
    string name;
    
    string getVariable(int index)
    {
        string lowerName = name;
        for (auto &c : lowerName) c = tolower(c);
        return lowerName + to_string(index);
    }
};

struct Mat2 : public Expression
{
    string name;

    string getVariable(int x, int y)
    {
        string lowerName = name;
        for (auto &c : lowerName) c = tolower(c);
        return lowerName + to_string(x) + "_" + to_string(y);
    }
};

struct Sum : public Expression
{
    vector<Expression*> children;

    void print()
    {

    }

    double evaluate(const Binding &binding)
    {

    }
};
