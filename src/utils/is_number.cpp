#include "interpreter/interpreter.h"

bool is_number(const string &s)
{
    bool negative = true;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (i == 0 && s[i] == '-' && negative == true)
        {
            negative = false;
            continue;
        }

        if (!isdigit(s[i]))
        {
            return false;
        }
    }
    return true;
}