#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

// creating a function to convert the data in cell to string
string cell_to_string(cell_data_t cellData)
{
    // check what type of data we're dealing with and convert accordingly
    switch (cellData.cell_data_type)
    {
    case INT:
        return to_string(*static_cast<int *>(cellData.cell_data.get()));

    case UNSIGNED_INT:
        return to_string(*static_cast<unsigned int *>(cellData.cell_data.get()));

    case BOOLEAN:
        // convert boolean to "true" or "false" string
        return (*static_cast<bool *>(cellData.cell_data.get())) ? "true" : "false";

    case FLOATING_POINT:
        // convert float to string
        return to_string(*static_cast<float *>(cellData.cell_data.get()));

    case DOUBLE_FLOATING_POINT:
        return to_string(*static_cast<double *>(cellData.cell_data.get()));

    case UNSIGNED_CHAR:
        // cast the character to unsigned int first before converting to string
        return to_string(
            static_cast<unsigned int>(*static_cast<unsigned char *>(cellData.cell_data.get())));

    case STRING:
        // cast the void pointer to char pointer which is already a string-like data
        return string(static_cast<char *>(cellData.cell_data.get()));

    default:
        // if we somehow get an unknown type, throw an error
        throw runtime_error("Unknown cell data type");
    }
}

void read_table(vector<string> tokens)
{
    string table_name;

    // scan through tokens to find the FROM keyword, table name comes right after
    for (size_t i = 0; i < tokens.size(); i++)
    {
        string from = "FROM";
        // using strcasecmp to handle case-insensitive matching (FROM, from, From all work)
        if (strcasecmp(tokens[i].c_str(), from.c_str()) == 0)
        {
            table_name = tokens[i + 1];
            break;
        }
    }

    // load the schema for this table which tells us columns, data types, etc
    schema_t table_schema =
        get_schema_from_schema(table_name + "__schema_data.bin");

    // if the table doesn't exist, the schema will have 0 columns
    if (table_schema.num_cols == 0)
    {
        logger("Cannot read table: table does not exist!\n", LOG_ERROR);
        logger("Available tables:\n", LOG_WARNING);
        display_tables();
        return;
    }

    // read all rows
    int start_row = 0;
    int end_row = table_schema.num_rows;

    // these will store the column names and all the row data
    vector<string> columns;
    vector<vector<string>> rows;

    // track which columns we need to read by their IDs
    vector<unsigned char> col_id;

    // handle SELECT * (all columns) vs specific columns
    if (tokens[1] == "*")
    {
        // add all columns from the schema
        for (int i = 0; i < table_schema.num_cols; i++)
        {
            columns.push_back(table_schema.column_data[i].col_name);
            col_id.push_back(table_schema.column_data[i].col_id);
        }
    }
    else
    {
        // figure out where the column list ends (at the token before FROM)
        // tokens: SELECT col1, col2, ... FROM table_name
        // find FROM position and select_end is one position before it
        size_t select_end = tokens.size();
        for (size_t i = 0; i < tokens.size(); i++)
        {
            if (strcasecmp(tokens[i].c_str(), "FROM") == 0)
            {
                select_end = i;
                break;
            }
        }

        // go through each specified column (skip every other token which are commas)
        for (size_t j = 1; j < select_end; j += 2)
        {
            // match the specified column name with the schema
            for (int i = 0; i < table_schema.num_cols; i++)
            {
                if (table_schema.column_data[i].col_name == tokens[j])
                {
                    columns.push_back(tokens[j]);
                    col_id.push_back(table_schema.column_data[i].col_id);
                }
            }
        }
    }

    // Read rows
    for (int row = start_row; row < end_row; row++)
    {
        vector<string> row_values;

        // get each column value for this row
        for (size_t i = 0; i < col_id.size(); i++)
        {
            int column = col_id[i];

            // read the actual cell data from disk
            cell_data_t cellData =
                get_table_cell_data(row, column, table_schema);

            // convert the cell data to a readable string format
            row_values.push_back(cell_to_string(cellData));
        }

        // add this row to our results
        rows.push_back(row_values);
    }

    // Send to logger table
    log_table_t output_table = {
        .num_rows = (int)rows.size(),
        .num_cols = (int)columns.size(),
        .row_data = rows,
        .col_data = columns,
    };

    log_table(&output_table);
}