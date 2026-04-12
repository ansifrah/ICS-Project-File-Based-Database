#include "interpreter/interpreter.h"
#include "engine/TableCreate.h"
#include "engine/TableWrite.h"
#include "engine/TableRead.h"
#include <iomanip>

void create_table(vector<string> tokens)
{
    // keeping track of which column number we're on
    unsigned char column_id = 0;
    // stores all the column info we parse from the tokens
    vector<col_item_t> column_data;

    // map all the string representations of data types to their actual enum values
    unordered_map<string, col_data_type_t> type_map = {
        {"int", INT},
        {"unsigned_int", UNSIGNED_INT},
        {"string", STRING},
        {"bool", BOOLEAN},
        {"float", FLOATING_POINT},
        {"double", DOUBLE_FLOATING_POINT},
        {"unsigned_char", UNSIGNED_CHAR}};

    // go through every token from the parsed SQL command
    for (size_t i = 0; i < tokens.size(); i++)
    {
        // skip over the syntax stuff like commas and parentheses, also skip numbers
        if (tokens[i] == "," || tokens[i] == "(" || tokens[i] == ")" || is_number(tokens[i]))
            continue;
        int check = -1;
        // check if current token is a valid data type
        if (type_map.count(tokens[i]))
        {
            col_item_t col{};

            // assign the column id and get the data type from the map
            col.col_id = column_id;
            col.data_type = type_map[tokens[i]];
            // the column name is the token right before the type
            col.col_name = tokens[i - 1];
            col.is_string = (col.data_type == STRING);
            col.is_primary_key = false;

            // check if this column is marked as PRIMARY KEY in the statement
            bool is_primary_key;
            string primary = "PRIMARY";
            string key = "KEY";
            // for non-string types, PRIMARY KEY comes right after the type
            if (!col.is_string)
            {
                is_primary_key = ((i + 2 < tokens.size()) && (strcasecmp(tokens[i + 1].c_str(), primary.c_str()) == 0) && (strcasecmp(tokens[i + 2].c_str(), key.c_str()) == 0));
            }
            // for strings, there's a length parameter in between, so we check further ahead
            else
            {
                is_primary_key = ((i + 5 < tokens.size()) && (strcasecmp(tokens[i + 4].c_str(), primary.c_str()) == 0) && (strcasecmp(tokens[i + 5].c_str(), key.c_str()) == 0));
            }
            // if it's a primary key, mark it and try to get the max string length
            if (is_primary_key)
            {
                col.is_primary_key = true;
                if (i + 2 < tokens.size() && is_number(tokens[i + 2]))
                {
                    col.max_str_len = stoi(tokens[i + 2]);
                    check = 0;
                }
            }
            // if it's not a primary key, still grab the max string length if there is one
            if (check != 0)
            {
                col.max_str_len = 0;
                if (i + 2 < tokens.size() && is_number(tokens[i + 2]))
                    col.max_str_len = stoi(tokens[i + 2]);
            }

            // add this column to our collection and move to the next column
            column_data.push_back(col);
            column_id++;
        }
    }

    // now lets actually create the schema with all the column info we gathered
    schema_t new_schema;
    new_schema.table_name = tokens[2];
    new_schema.num_cols = (int)column_data.size();
    new_schema.num_rows = 0;
    new_schema.column_data = column_data;

    // write the schema to disk
    int result = create_new_table_schema(new_schema);

    // log whether the table creation was successful
    if (result != -1)
        logger("Successfully created schema\n", LOG_SUCCESS);
    else
        logger("Error in creating schema!\n", LOG_ERROR);

    // grab the schema we just created so we can display it to the user
    display_table_schema(tokens[2]);//tokens[2] is the table name
}