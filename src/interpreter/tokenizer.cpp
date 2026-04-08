#include "interpreter/interpreter.h"

vector<string> tokenize(const string &s)
{
    vector<string> tokens;
    string x = "";
    bool in_quotes = false;
    char quote_char = 0;
    size_t size = s.size();
    bool check;

    for (size_t i = 0; i < size; i++)
    {
        check = false;
        char c = s[i];

        if ((c == '\'') && !in_quotes)
        {
            for (size_t j = i + 1; j < size; j++)
            {
                if (s[j] == '\'')
                {
                    check = true;
                    break;
                }
            }
            if (!check)
            {
                goto no_closing_quote;
            }
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
            x += c;
            in_quotes = true;
            quote_char = c;
            continue;
        }

        if (in_quotes && c == quote_char)
        {
            x += c;
            tokens.push_back(x);
            x = "";
            in_quotes = false;
            continue;
        }

        if (in_quotes)
        {
            x += c;
            continue;
        }

    no_closing_quote:
        if (isspace(c))
        {
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
        }
        else if (c == ',' || c == '(' || c == ')' || c == ';' || c == '=')
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