#include "interpreter/interpreter.h"

vector<string> tokenize(const string &s)
{
    vector<string> tokens;
    string x = "";
    bool in_quotes = false;
    char quote_char = 0;
    size_t size = s.size();
    bool check = false;

    for (size_t i = 0; i < size; i++)
    {
        check = false;
        char c = s[i];

        if ((c == '\'') && !in_quotes)
        {
            for (size_t j = i + 1; j < size; j++)
            {
                for (size_t k = j + 2; k < size; k++)
                {
                    if ((j + 1 < size) && s[j] == '\\' && s[j + 1] == '\'' && s[k] == '\'')
                    {
                        check = true;
                        break;
                    }
                }
                if (s[j] == '\'')
                {
                    check = true;
                    break;
                }
            }
            if (!check)
            {
                goto only_1_quote;
            }
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

        if (in_quotes)
        {
            if (c == '\\' && (i + 1) < size)
            {
                char next = s[i + 1];
                if (next == quote_char || next == '\\')
                {
                    x += next;
                    i++;
                    continue;
                }
            }

            if (c == quote_char)
            {
                tokens.push_back(x);
                x = "";
                tokens.push_back(string(1, quote_char));
                in_quotes = false;
                continue;
            }

            x += c;
            continue;
        }
    only_1_quote:
        if (isspace(c))
        {
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
        }
        else if (c == ',' || c == '(' || c == ')' || c == ';' || c == '=' || c == '-')
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