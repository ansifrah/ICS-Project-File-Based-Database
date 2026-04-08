#include "interpreter/interpreter.h"

// breaks down the input string into individual tokens
// handles quoted strings and special chars like commas, parens, etc
vector<string> tokenize(const string &s)
{
    vector<string> tokens;
    string x = "";  // buffer for building current token
    bool in_quotes = false;  // tracks if we're inside a quoted string
    char quote_char = 0;  // stores which quote char started the quoted string
    size_t size = s.size();
    bool check;

    for (size_t i = 0; i < size; i++)
    {
        check = false;
        char c = s[i];

        if ((c == '\'') && !in_quotes)
        {
            // check if there's actually a closing quote somewhere ahead
            for (size_t j = i + 1; j < size; j++)
            {
                if (s[j] == '\'')
                {
                    check = true;
                    break;
                }
            }
            // if no closing quote found, jump to error handling
            if (!check)
            {
                goto no_closing_quote;
            }
            // save whatever we built before hitting the quote
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
            // start the quoted string
            x += c;
            in_quotes = true;
            quote_char = c;
            continue;
        }

        // we're in quotes and found the closing quote
        if (in_quotes && c == quote_char)
        {
            x += c;
            tokens.push_back(x);
            x = "";
            in_quotes = false;
            continue;
        }

        // inside a quoted string, just add everything as-is
        if (in_quotes)
        {
            x += c;
            continue;
        }

    no_closing_quote:
        // whitespace = token separator, so flush the current buffer
        if (isspace(c))
        {
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
        }
        // special chars (comma, parens, semicolon, equals) are their own tokens
        else if (c == ',' || c == '(' || c == ')' || c == ';' || c == '=')
        {
            // save what we had before the special char
            if (!x.empty())
            {
                tokens.push_back(x);
                x = "";
            }
            // special char gets added as its own token
            tokens.push_back(string(1, c));
        }
        // regular chars just get added to the current token
        else
        {
            x += c;
        }
    }

    // pick up any leftover chars that weren't finished
    if (!x.empty())
        tokens.push_back(x);

    return tokens;
}