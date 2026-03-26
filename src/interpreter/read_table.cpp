#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

//creating a function to convert the data in cell to string
string cell_to_string(cell_data_t cellData)
{
    switch (cellData.cell_data_type)
    {
    case INT:
        return to_string(*static_cast<int *>(cellData.cell_data.get()));

    case UNSIGNED_INT:
        return to_string(*static_cast<unsigned int *>(cellData.cell_data.get()));

    case BOOLEAN:
        return (*static_cast<bool *>(cellData.cell_data.get())) ? "true" : "false";

    case FLOATING_POINT:
        return to_string(*static_cast<float *>(cellData.cell_data.get()));

    case DOUBLE_FLOATING_POINT:
        return to_string(*static_cast<double *>(cellData.cell_data.get()));

    case UNSIGNED_CHAR:
        return to_string(
            static_cast<unsigned int>(*static_cast<unsigned char *>(cellData.cell_data.get())));

    case STRING:
        return string(static_cast<char *>(cellData.cell_data.get()));
        
    default:
        throw runtime_error("Unknown cell data type");
    }
}

void read_table(vector<string> tokens)
{
    string table_name;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        string from="FROM";
        if (strcasecmp(tokens[i].c_str(),from.c_str())==0)
        {
            table_name = tokens[i + 1];
            break;
        }
    }

    schema_t table_schema =
        get_schema_from_schema(table_name + "__schema_data.bin");

    // ensure schema was actually loaded; if num_cols is zero the table does not exist
    if (table_schema.num_cols == 0) {
        logger("Cannot read table: table does not exist!\n", LOG_ERROR);
        logger("Tables avalaible are:\n",LOG_WARNING);
        display_tables();
        return;
    }

    // WHERE detection
    int where_pos = -1;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        string where="WHERE";
        if (strcasecmp(tokens[i].c_str(),where.c_str())==0)
        {
            where_pos = i;
            break;
        }
    }

    int start_row = 0;
    int end_row = table_schema.num_rows;

    if (where_pos != -1)
    {
        int target_row = stoi(tokens[where_pos + 3]);
        start_row = target_row;
        end_row = target_row + 1;
    }

    vector<string> columns;
    vector<vector<string>> rows;

    vector<unsigned char> col_id;

    // SELECT *
    if (tokens[1] == "*")
    {
        for (int i = 0; i < table_schema.num_cols; i++)
        {
            columns.push_back(table_schema.column_data[i].col_name);
            col_id.push_back(table_schema.column_data[i].col_id);
        }
    }
    else
    {
        size_t select_end =
            (where_pos == -1) ? tokens.size() - 3 : where_pos;

        for (size_t j = 1; j < select_end; j += 2)
        {
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

        for (size_t i = 0; i < col_id.size(); i++)
        {
            int column = col_id[i];

            cell_data_t cellData =
                get_table_cell_data(row, column, table_schema);

            row_values.push_back(cell_to_string(cellData));
        }

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