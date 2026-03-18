#include "interpreter/interpreter.h"
#include "engine/TableCreate.h"
#include "engine/TableWrite.h"
#include "engine/TableRead.h"

void create_table(vector<string> tokens)
{
    unsigned char column_id = 0;
    vector<col_item_t> column_data;

    unordered_map<string, col_data_type_t> type_map = {
        {"INT", INT},
        {"UNSIGNED_INT", UNSIGNED_INT},
        {"STRING", STRING},
        {"BOOLEAN", BOOLEAN},
        {"FLOATING_POINT", FLOATING_POINT},
        {"DOUBLE_FLOATING_POINT", DOUBLE_FLOATING_POINT},
        {"UNSIGNED_CHAR", UNSIGNED_CHAR}};

    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "," || tokens[i] == "(" || tokens[i] == ")" || is_number(tokens[i]))
            continue;
        int check = -1;
        if (type_map.count(tokens[i]))
        {
            col_item_t col{};

            col.col_id = column_id;
            col.data_type = type_map[tokens[i]];
            col.col_name = tokens[i - 1];
            col.is_string = (col.data_type == STRING);
            col.is_primary_key = false;

            string primary="PRIMARY";
            string key="KEY";
            bool is_primary_key=(i + 2 < tokens.size()) && (strcasecmp( tokens[i + 1].c_str(),primary.c_str())==0) && (strcasecmp(tokens[i+2].c_str(),key.c_str())==0);
            if (is_primary_key)
            {
                col.is_primary_key = true;
                if (i + 4 < tokens.size() && is_number(tokens[i + 4]))
                {
                    col.max_str_len = stoi(tokens[i + 4]);
                    check = 0;
                }
            }
            if (check != 0)
            {
                col.max_str_len = 0;
                if (i + 2 < tokens.size() && is_number(tokens[i + 2]))
                    col.max_str_len = stoi(tokens[i + 2]);
            }

            column_data.push_back(col);
            column_id++;
        }
    }

    // Building schema
    schema_t new_schema;
    new_schema.table_name = tokens[2];
    new_schema.num_cols = (int)column_data.size();
    new_schema.num_rows = 0;
    new_schema.column_data = column_data;

    // Creating the table
    int result = create_new_table_schema(new_schema);

    if (result != -1)
        logger("Successfully created schema\n", LOG_SUCCESS);
    else
        logger("Error in creating schema!\n", LOG_ERROR);
    // checking
    string table=tokens[2];
    schema_t table_schema = get_schema_from_schema(table+"__schema_data.bin");
    for (size_t i = 0; i < table_schema.num_cols; i++)
    {
        cout << table_schema.column_data[i].is_primary_key << " ";
        cout << (int)table_schema.column_data[i].col_id << " ";
        cout << (int)table_schema.column_data[i].data_type << " ";
        cout << table_schema.column_data[i].col_name << " ";
        cout << table_schema.column_data[i].is_string << " ";
        cout << table_schema.column_data[i].max_str_len << " ";
        cout << endl;
    }
}