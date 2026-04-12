#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

void display_table_schema(const string &table_name)
{
    string tableName = table_name;
    schema_t table_schema = get_schema_from_schema(tableName + "__schema_data.bin");
    // check if the table actually exists, if not throw error and list available tables
    if (table_schema.num_cols == 0)
    {
        logger("Table does not exist!\n", LOG_ERROR);
        logger("Tables available are:\n", LOG_WARNING);
        display_tables();
        return;
    }

    size_t colnum = table_schema.num_cols;
    string data_type;
    string max_str_len;

    // reverse mapping from enum to string for prettier output
    unordered_map<col_data_type_t, string> type_map2 = {
        {INT, "int"},
        {UNSIGNED_INT, "unsigned int"},
        {STRING, "string"},
        {BOOLEAN, "bool"},
        {FLOATING_POINT, "float"},
        {DOUBLE_FLOATING_POINT, "double"},
        {UNSIGNED_CHAR, "unsigned char"}};

    // print the header row for our table of column info
    logger("\nSchema for table '", LOG_INFO);
    logger(table_name, LOG_INFO);
    logger("':\n", LOG_INFO);
    logger("-------------------------------------------------------------------------------------------------\n", LOG_WARNING);
    cout << left << setw(16) << "ColumnId"
         << setw(18) << "ColumnName"
         << setw(16) << "DataType"
         << setw(21) << "is_PrimaryKey"
         << setw(17) << "is_String"
         << setw(17) << "MaxStrLen"
         << "\n";
    logger("═════════════════════════════════════════════════════════════════════════════════════════════════\n", LOG_WARNING);

    // print out each column with all its attributes properly formatted
    for (size_t i = 0; i < colnum; i++)
    {
        data_type = type_map2[table_schema.column_data[i].data_type];
        // strings have a max length, other types don't really need one so just print N/A
        if (data_type != "string")
            max_str_len = "N/A";
        else
            max_str_len = to_string(table_schema.column_data[i].max_str_len);

        // output each column's info in spaced columns
        cout << left << setw(16) << (int)table_schema.column_data[i].col_id
             << setw(18) << table_schema.column_data[i].col_name
             << setw(16) << data_type
             << setw(21) << boolalpha << table_schema.column_data[i].is_primary_key
             << setw(17) << boolalpha << table_schema.column_data[i].is_string
             << setw(17) << max_str_len
             << '\n';
    }
    logger("-------------------------------------------------------------------------------------------------\n\n", LOG_WARNING);
}