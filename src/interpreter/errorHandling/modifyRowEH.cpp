#include "interpreter/interpreter.h"
#include "engine/TableRead.h"
#include "engine/TableModify.h"

// function to validate data types based on the schema definitions
bool validate_data_type(const string &val, int type, int max_len)
{
    try
    {
        if (type == INT)
        {
            size_t pos;
            stoi(val, &pos);
            return pos == val.length();
        }
        else if (type == UNSIGNED_INT)
        {
            if (!val.empty() && val[0] == '-')
                return false;
            size_t pos;
            stoul(val, &pos);
            return pos == val.length();
        }
        else if (type == STRING)
        {
            // Strings must be enclosed in single quotes based on tokenizer
            if (val.length() < 2 || val.front() != '\'' || val.back() != '\'')
                return false;
            // Check if length without quotes exceeds max_str_len
            if ((int)(val.length() - 2) > max_len)
                return false;
            return true;
        }
        else if (type == BOOLEAN)
        {
            string lower_val = val;
            for (char &c : lower_val)
                c = tolower((unsigned char)c);
            return (lower_val == "true" || lower_val == "false");
        }
        else if (type == FLOATING_POINT || type == DOUBLE_FLOATING_POINT)
        {
            size_t pos;
            stod(val, &pos);
            return pos == val.length();
        }
        else if (type == UNSIGNED_CHAR)
        {
            size_t pos;
            int v = stoi(val, &pos);
            return pos == val.length() && v >= 0 && v <= 255;
        }
    }
    catch (...)
    {
        // Catches errors from stoi/stod/stoul
        return false;
    }
    return false;
}

// function to check if a value already exists in a given column 
// this function works only on primary key column (if that one is being modified)
bool is_value_unique(schema_t schema, int colIdx, int type, const string &val)
{
    for (int r = 0; r < schema.num_rows; r++)
    {
        cell_data_t cell = get_table_cell_data(r, colIdx, schema);
        if (cell.cell_data_type == NULL_TYPE)
            continue;

        try
        {
            if (type == INT)
            {
                if (stoi(val) == *static_cast<int *>(cell.cell_data.get()))
                    return false;
            }
            else if (type == UNSIGNED_INT)
            {
                if (stoul(val) == *static_cast<unsigned int *>(cell.cell_data.get()))
                    return false;
            }
            else if (type == STRING)
            {
                // Remove the single quotes from the tokenized string for comparison
                string clean_val = val.substr(1, val.length() - 2);
                string cell_val = string(static_cast<char *>(cell.cell_data.get()));
                if (clean_val == cell_val)
                    return false;
            }
            else if (type == BOOLEAN)
            {
                string lower_val = val;
                for (char &c : lower_val)
                    c = tolower((unsigned char)c);
                bool b = (lower_val == "true");
                if (b == *static_cast<bool *>(cell.cell_data.get()))
                    return false;
            }
            else if (type == FLOATING_POINT)
            {
                if (stof(val) == *static_cast<float *>(cell.cell_data.get()))
                    return false;
            }
            else if (type == DOUBLE_FLOATING_POINT)
            {
                if (stod(val) == *static_cast<double *>(cell.cell_data.get()))
                    return false;
            }
            else if (type == UNSIGNED_CHAR)
            {
                unsigned char c = static_cast<unsigned char>(stoi(val));
                if (c == *static_cast<unsigned char *>(cell.cell_data.get()))
                    return false;
            }
        }
        catch (...)
        {
            return false; // Catches errors from try block
        }
    }
    return true; // Value not found, meaning it is unique
}

void check_modifyRow(vector<string> tokens, vector<string> lower_tok)
{
    int l = tokens.size();

    // Minimum length and base syntax check
    // UPDATE table SET col1 = val1 WHERE col = val ; (minimum 11 tokens)
    if (l < 11)
    {
        logger("Error: Incomplete UPDATE statement. Check your syntax.\n", LOG_ERROR);
        return;
    }

    if (lower_tok[0] != "update" || lower_tok[2] != "set" || lower_tok[l - 5] != "where" || tokens[l - 1] != ";")
    {
        logger("Error: Syntax error. Expected format: UPDATE tableName SET col1 = val1, ... WHERE col = val;\n", LOG_ERROR);
        return;
    }

    if (tokens[l - 3] != "=")
    {
        logger("Error: Syntax error in WHERE clause. Expected '='.\n", LOG_ERROR);
        return;
    }

    // Check if table exists
    string tbName = tokens[1];
    string schemaName = tbName + "__schema_data.bin";
    schema_t schema = get_schema_from_schema(schemaName);

    // If num_cols is 0, the schema file wasn't found or is empty
    if (schema.num_cols == 0)
    {
        printf(ANSI_COLOR_RED "Error: Table '%s' does not exist." ANSI_COLOR_RESET "\n", tbName.c_str());
        cout << "Tables availaible are:\n";
        display_tables();
        return;
    }

    // Verify SET clause columns, syntax, types, and primary key constraints
    int i = 3; // Start at the first column after SET
    while (i < l - 5)
    {
        if (tokens[i + 1] != "=")
        {
            printf(ANSI_COLOR_RED "Error: Syntax error in SET clause. Expected '=' after column '%s'." ANSI_COLOR_RESET "\n", tokens[i].c_str());
            return;
        }

        string colName = tokens[i];
        string val = tokens[i + 2];

        // Check if column exists in the schema
        int colIdx = -1;
        for (int c = 0; c < schema.num_cols; c++)
        {
            if (schema.column_data[c].col_name == colName)
            {
                colIdx = c;
                break;
            }
        }

        if (colIdx == -1)
        {
            printf(ANSI_COLOR_RED "Error: Column '%s' does not exist in table '%s'." ANSI_COLOR_RESET "\n", colName.c_str(), tbName.c_str());
            return;
        }

        // Validate data type against schema definitions
        int type = schema.column_data[colIdx].data_type;
        int max_len = schema.column_data[colIdx].max_str_len;

        if (!validate_data_type(val, type, max_len))
        {
            if (type == STRING)
            {
                printf(ANSI_COLOR_RED "Error: Data type mismatch or string too long for column '%s'. Max allowed length is %d." ANSI_COLOR_RESET "\n", colName.c_str(), max_len);
            }
            else
            {
                printf(ANSI_COLOR_RED "Error: Data type mismatch for column '%s'." ANSI_COLOR_RESET "\n", colName.c_str());
            }
            return;
        }

        // Primary Key Uniqueness Check
        if (schema.column_data[colIdx].is_primary_key)
        {
            if (!is_value_unique(schema, colIdx, type, val))
            {
                printf(ANSI_COLOR_RED "Error: Primary key constraint violation. Value '%s' already exists in column '%s'." ANSI_COLOR_RESET "\n", val.c_str(), colName.c_str());
                return;
            }
        }

        i += 3;
        // If we haven't reached the WHERE clause yet, expect a comma separator
        if (i < l - 5)
        {
            if (tokens[i] != ",")
            {
                logger("Error: Syntax error in SET clause. Expected ',' between assignments.\n", LOG_ERROR);
                return;
            }
            i++; // Skip the comma for the next iteration
        }
    }

    // Catch-all for malformed trailing SET syntax
    if (i != l - 5)
    {
        logger("Error: Syntax error in SET clause.\n", LOG_ERROR);
        return;
    }

    // Verify WHERE clause column and type
    string whereColName = tokens[l - 4];
    string whereVal = tokens[l - 2];

    int whereColIdx = -1;
    for (int c = 0; c < schema.num_cols; c++)
    {
        if (schema.column_data[c].col_name == whereColName)
        {
            whereColIdx = c;
            break;
        }
    }

    if (whereColIdx == -1)
    {
        printf(ANSI_COLOR_RED "Error: WHERE column '%s' does not exist in table '%s'." ANSI_COLOR_RESET "\n", whereColName.c_str(), tbName.c_str());
        return;
    }

    int whereType = schema.column_data[whereColIdx].data_type;
    int whereMaxLen = schema.column_data[whereColIdx].max_str_len;

    if (!validate_data_type(whereVal, whereType, whereMaxLen))
    {
        printf(ANSI_COLOR_RED "Error: Data type mismatch in WHERE clause for column '%s'." ANSI_COLOR_RESET "\n", whereColName.c_str());
        return;
    }

    // Execute this function if all checks passed
    modify_row(tokens);
}