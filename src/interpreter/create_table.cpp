#include "interpreter/interpreter.h"
#include "engine/TableCreate.h"
#include "engine/TableWrite.h"
#include "engine/TableRead.h"
#include <iomanip>

void create_table(vector<string> tokens)
{
    unsigned char column_id = 0;
    vector<col_item_t> column_data;

    unordered_map<string, col_data_type_t> type_map = {
        {"int", INT},
        {"unsigned_int", UNSIGNED_INT},
        {"string", STRING},
        {"bool", BOOLEAN},
        {"float", FLOATING_POINT},
        {"double", DOUBLE_FLOATING_POINT},
        {"unsigned_char", UNSIGNED_CHAR}};

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

            bool is_primary_key;
            string primary = "PRIMARY";
            string key = "KEY";
            // primary key check when data type not string
            if (!col.is_string)
            {
                is_primary_key = ((i + 2 < tokens.size()) && (strcasecmp(tokens[i + 1].c_str(), primary.c_str()) == 0) && (strcasecmp(tokens[i + 2].c_str(), key.c_str()) == 0));
            }
            // primary key check when data type is string
            else
            {
                is_primary_key = ((i + 5 < tokens.size()) && (strcasecmp(tokens[i + 4].c_str(), primary.c_str()) == 0) && (strcasecmp(tokens[i + 5].c_str(), key.c_str()) == 0));
            }
            if (is_primary_key)
            {
                col.is_primary_key = true;
                if (i + 2 < tokens.size() && is_number(tokens[i + 2]))
                {
                    col.max_str_len = stoi(tokens[i + 2]);
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

    // printing the info of columns created
    string table_name = tokens[2];
    schema_t table_schema = get_schema_from_schema(table_name + "__schema_data.bin");
    size_t colnum = table_schema.num_cols;
    string data_type;
    string max_str_len;

    unordered_map<col_data_type_t, string> type_map2 = {
        {INT, "int"},
        {UNSIGNED_INT, "unsigned int"},
        {STRING, "string"},
        {BOOLEAN, "bool"},
        {FLOATING_POINT, "float"},
        {DOUBLE_FLOATING_POINT, "double"},
        {UNSIGNED_CHAR, "unsigned char"}};

    cout << left << setw(16) << "ColumnId"
         << setw(18) << "ColumnName"
         << setw(16) << "DataType"
         << setw(21) << "is_PrimaryKey"
         << setw(17) << "is_String"
         << setw(17) << "MaxStrLen"
         << "\n";

    for (size_t i = 0; i < colnum; i++)
    {
        data_type = type_map2[table_schema.column_data[i].data_type];
        if (data_type != "string")
            max_str_len = "N/A";
        else
            max_str_len = to_string(table_schema.column_data[i].max_str_len);

        cout << left << setw(16) << (int)table_schema.column_data[i].col_id
             << setw(18) << table_schema.column_data[i].col_name
             << setw(16) << data_type
             << setw(21) << boolalpha << table_schema.column_data[i].is_primary_key
             << setw(17) << boolalpha << table_schema.column_data[i].is_string
             << setw(17) << max_str_len
             << '\n';
    }
}