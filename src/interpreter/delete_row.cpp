#include "interpreter/interpreter.h"
#include "engine/TableRead.h"
#include "engine/TableDelete.h"

void delete_row(vector<string> tokens)
{
    // find where FROM is in the token list since the table name comes right after it
    int from_pos = -1;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        string from = "FROM";
        if (strcasecmp(tokens[i].c_str(), from.c_str()) == 0)
        {
            from_pos = i;
            break;
        }
    }

    // grab the table name that's right after FROM
    string table_name = tokens[from_pos + 1];
    schema_t schema = get_schema_from_schema(table_name + "__schema_data.bin");

    // check if the table actually exists, if not throw error and list available tables
    if (schema.num_cols == 0)
    {
        logger("Cannot delete row: table does not exist!\n", LOG_ERROR);
        logger("Tables available are:\n", LOG_WARNING);
        display_tables();
        return;
    }

    // the token with our row numbers/ranges to delete. could be "1-4" or "1,2,3"
    string digits = tokens[2];

    // find where the hyphen is if there is one (for range deletion like 1-4)
    int hyphen_pos = 0;
    bool hyphen_prsnt = false;
    for (char c : digits)
    {
        if (c == '-')
        {
            hyphen_prsnt = true;
            break;
        }
        else
        {
            hyphen_pos++;
        }
    }

    if (hyphen_prsnt)
    {

        // add spaces around the hyphen so we can properly tokenize it
        digits.insert(hyphen_pos, 1, ' ');
        digits.insert(hyphen_pos + 2, 1, ' ');

        // split up the string so we can check if its a range or individual rows
        vector<string> digit(3);
        digit = tokenize(digits);

        // user wants to delete a range of rows like DELETE 1-4 FROM table_name
        int v1 = stoi(digit[0]);
        int v2 = stoi(digit[2]);

        // delete from highest to lowest so we don't mess up indices
        for (int i = v2; i >= v1; i--)
        {
            int row_to_delete = i;
            int result = delete_row_from_table(row_to_delete, schema);

            if (result == 0)
            {
                // success, it updated the schema file automatically
                cout << "Rows remaining: " << schema.num_rows << endl;
            }
            else
            {
                logger("Error deleting row (row(s) may not exist)\n", LOG_ERROR);
                break;
            }
        }
    }
    else
    {
        // user wants to delete specific rows like DELETE 1,2,5 FROM table_name
        vector<int> v;
        auto begin = tokens.begin() + 2;

        // go through tokens and grab all the row numbers, skip commas
        for (auto i = begin;; i++)
        {
            if (*i == ",")
                continue;
            if (*i == ")")
                break;
            v.push_back(stoi(*i));
        }

        // sort in descending order so we delete from highest to lowest and don't break indices
        sort(v.begin(), v.end(), greater<int>());

        // delete each row one by one
        for (auto i = v.begin(); i < v.end(); i++)
        {
            int row_to_delete = *i;
            int result = delete_row_from_table(row_to_delete, schema);

            if (result == 0)
            {
                // row got deleted, schema updated by the delete function
                cout << "Rows remaining: " << schema.num_rows << endl;
            }
             else
            {
                logger("Error deleting row (row may not exist)\n", LOG_ERROR);
                break;
            }
        }
    }
}