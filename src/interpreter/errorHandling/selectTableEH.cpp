#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

void check_selectData(vector<string> tokens, vector<string> lower_tok)
{
    int l = tokens.size();

    // Basic length and syntax check (min: SELECT * FROM table ;)
    if (l < 5)
    {
        logger("Error: Incomplete SELECT statement.\n", LOG_ERROR);
        return;
    }

    if (lower_tok[0] != "select" || tokens[l - 1] != ";")
    {
        logger("Error: Syntax error. Expected format: SELECT col1, col2 FROM tableName ;\n", LOG_ERROR);
        return;
    }

    // Finding the FROM keyword
    int fromIdx = -1;
    for (int i = 1; i < l - 1; i++)
    {
        if (lower_tok[i] == "from")
        {
            fromIdx = i;
            break;
        }
    }

    if (fromIdx == -1)
    {
        logger("Error: Syntax error. Missing 'FROM' keyword.\n", LOG_ERROR);
        return;
    }
    if (fromIdx == 1)
    {
        logger("Error: Syntax error. Missing columns to select.\n", LOG_ERROR);
        return;
    }

    // Extract and check the requested columns
    vector<string> selected_cols;
    if (fromIdx == 2 && tokens[1] == "*")
    {
        selected_cols.push_back("*");
    }
    else
    {
        for (int i = 1; i < fromIdx; i++)
        {
            if (tokens[i] != ",")
            {
                selected_cols.push_back(tokens[i]);
            }
            else
            {
                // Ensure no consecutive commas or trailing commas before FROM
                if (i == 1 || i == fromIdx - 1 || tokens[i - 1] == ",")
                {
                    logger("Error: Syntax error in column list.\n", LOG_ERROR);
                    return;
                }
            }
        }
    }

    // getting the table name
    if (fromIdx >= l - 2)
    {
        logger("Error: Syntax error. Missing table name after FROM.\n", LOG_ERROR);
        return;
    }
    string tbName = tokens[fromIdx + 1];

    // Checking if the table exists
    string schemaName = tbName + "__schema_data.bin";
    schema_t schema = get_schema_from_schema(schemaName);

    if (schema.num_cols == 0)
    {
        printf(ANSI_COLOR_RED "Error: Table '%s' does not exist." ANSI_COLOR_RESET "\n", tbName.c_str());
        cout << "Tables available are:\n";
        display_tables();
        return;
    }

    // checking if the requested columns actually exist in the schema
    if (selected_cols[0] != "*")
    {
        for (size_t i = 0; i < selected_cols.size(); i++)
        {
            bool found = false;
            for (int c = 0; c < schema.num_cols; c++)
            {
                if (schema.column_data[c].col_name == selected_cols[i])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                printf(ANSI_COLOR_RED "Error: Column '%s' does not exist in table '%s'." ANSI_COLOR_RESET "\n", selected_cols[i].c_str(), tbName.c_str());
                return;
            }
        }
    }

    // Check WHERE clause (if present)
    int whereIdx = -1;
    for (int i = fromIdx + 2; i < l - 1; i++)
    {
        if (lower_tok[i] == "where")
        {
            whereIdx = i;
            break;
        }
    }

    if (whereIdx != -1)
    {
        // Syntax should be WHERE ID = value ;
        if (whereIdx != l - 5 || tokens[whereIdx + 2] != "=")
        {
            logger("Error: Syntax error in WHERE clause. Expected format: WHERE ID = value ;\n", LOG_ERROR);
            return;
        }

        string whereColName = tokens[whereIdx + 1];
        string whereVal = tokens[whereIdx + 3];

        // only id can be given in where clause
        string lowerWhereCol = "";
        for (char c : whereColName)
            lowerWhereCol += tolower(c);

        if (lowerWhereCol != "id")
        {
            logger("Error: Unsupported WHERE condition. Only 'ID' is allowed.\n", LOG_ERROR);
            return;
        }

        // finding the ID column in the schema
        int colIdx = -1;
        for (int c = 0; c < schema.num_cols; c++)
        {
            string lowerSchemaCol = "";
            for (size_t iter = 0; iter < schema.column_data[c].col_name.size(); iter++)
                lowerSchemaCol += (char)tolower(schema.column_data[c].col_name[iter]);

            if (lowerSchemaCol == "id")
            {
                colIdx = c;
                break;
            }
        }

        if (colIdx == -1)
        {
            printf(ANSI_COLOR_RED "Error: Column 'ID' does not exist in table '%s'." ANSI_COLOR_RESET "\n", tbName.c_str());
            return;
        }
        else
        {
            string rowIDnum = tokens[l - 2];
            for (size_t it = 0; it < rowIDnum.size(); it++)
            {
                if (rowIDnum[it] < '0' || rowIDnum[it] > '9')
                {
                    logger("Error: invalid ID Number \n", LOG_ERROR);
                    return;
                }
            }
            int colID = stoi(rowIDnum);
            if (colID >= schema.num_rows)
            {
                logger("Error: Row ID out of bounds \n", LOG_ERROR);
                return;
            }
        }
    }
    else
    {
        // If there is no WHERE clause, there shouldn't be anything between the table name and ';'
        if (fromIdx + 2 != l - 1)
        {
            printf(ANSI_COLOR_RED "Error: Syntax error near '%s'." ANSI_COLOR_RESET "\n", tokens[fromIdx + 2].c_str());
            return;
        }
    }

    // executing the function if all checks are passed
    read_table(tokens);
}