
#include "main.h"

void warningFunctionMLIB(const std::string &description)
{
    cout << description << endl;
    //DEBUG_BREAK;
}

void errorFunctionMLIB(const std::string &description)
{
    cout << description << endl;
    DEBUG_BREAK;
}

void assertFunctionMLIB(bool statement, const std::string &description)
{
    if (!statement)
    {
        cout << description << endl;
#ifdef _DEBUG
        DEBUG_BREAK;
#endif
    }
}
