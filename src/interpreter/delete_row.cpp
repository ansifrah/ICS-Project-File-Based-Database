#include "interpreter/interpreter.h"
#include "engine/TableRead.h"
#include "engine/TableDelete.h"

void delete_row(vector<string> tokens)
{
    // FROM position
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
    // just after FROM we have table name, so extracting that
    string table_name = tokens[from_pos + 1];
    schema_t schema = get_schema_from_schema(table_name + "__schema_data.bin");
    if (schema.num_cols == 0)
    {
        logger("Cannot delete row: table does not exist!\n", LOG_ERROR);
        logger("Tables availaible are:\n", LOG_WARNING);
        display_tables();
        return;
    }
    // delete range of rows 1-4

    // digits is like 1-4
    string digits = tokens[2];
    // hyphen position
    int hyphen_pos = 0;
    for (char c : digits)
    {
        if (c == '-')
        {
            break;
        }
        else
        {
            hyphen_pos++;
        }
    }
    // adding space before and after hyphen
    digits.insert(hyphen_pos, 1, ' ');
    digits.insert(hyphen_pos + 2, 1, ' ');
    // tokenizing 1-4
    vector<string> digit(3);
    digit = tokenize(digits);
    if (digit[1] == "-")
    {
        int v1 = stoi(digit[0]);
        int v2 = stoi(digit[2]);
        for (int i = v2; i >= v1; i--)
        {
            int row_to_delete = i;
            int result = delete_row_from_table(row_to_delete, schema);

            if (result == 0)
            {
                // Row deleted successfully; schema file is updated by delete function
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
        // delete 0,1,2......n rows
        vector<int> v;
        auto begin = tokens.begin() + 2;
        for (auto i = begin;; i++)
        {
            if (*i == ",")
                continue;
            if (*i == ")")
                break;
            v.push_back(stoi(*i));
        }
        sort(v.begin(), v.end(), greater<int>());
        for (auto i = v.begin(); i < v.end(); i++)
        {
            int row_to_delete = *i;
            int result = delete_row_from_table(row_to_delete, schema);

            if (result == 0)
            {
                // Row deleted successfully; schema file is updated by delete function
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