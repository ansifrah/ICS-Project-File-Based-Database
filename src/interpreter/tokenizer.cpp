#include "interpreter/interpreter.h"

vector<string> tokenize(const string& s)
{
    vector<string> tokens;
    string x = "";
    bool in_quotes = false;
    char quote_char = 0;

    for (size_t i = 0; i < s.size(); i++)
    {
        char c = s[i];

        if ((c == '\'') && !in_quotes)
        {
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
            tokens.push_back("'");
            in_quotes = true;
            quote_char = c;
            continue;
        }

        if (in_quotes && c == quote_char)
        {
            tokens.push_back(x);
            x = "";
            tokens.push_back("'");
            in_quotes = false;
            continue;
        }

        if (in_quotes)
        {
            x += c;
            continue;
        }

        if (isspace(c))
        {
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
        }
        else if (c == ',' || c == '(' || c == ')' || c == ';' || c == '='|| c=='-')
        {
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
            tokens.push_back(string(1, c));
        }
        else
        {
            x += c;
        }
    }

    if (!x.empty())
        tokens.push_back(x);

    return tokens;
}