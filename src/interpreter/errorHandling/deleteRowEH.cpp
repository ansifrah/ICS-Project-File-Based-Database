#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

void check_deleteRow(vector<string> tokens)
{
    int l = tokens.size();

    string table_name = tokens[l - 2];
    int tableExists = 0;
    vector<string> tablenames;
    namespace fs = filesystem;
    string suffix = "__schema_data.bin";
    for (const auto &e : fs::directory_iterator("."))
    {
        string f = e.path().filename().string();
        if (f.size() > suffix.size() && f.compare(f.size() - suffix.size(), suffix.size(), suffix) == 0)
        {
            string tableName = f.substr(0, f.size() - suffix.size());
            tablenames.push_back(tableName);
        }
    }
    int u = tablenames.size();
    for (int y = 0; y < u; y++)
    {
        if (tablenames[y] == table_name)
        {
            tableExists = 1;
            break;
        }
    }

    int errorCode = -1;
    // 0 for using incorrect characters in row id
    // 1 if id numbers not in range
    // 2 error related to comma
    // 3 id numbers not separated by comma
    // 4 for general syntax error

    int countDash = 0;
    int countComma = 0;
    for (int i = 2; i < l - 4; i++)
    {
        string str = tokens[i];
        int l1 = str.size();
        for (int j = 0; j < l1; j++)
        {
            char temp = str[j];
            if ((temp < '0' || temp > '9') && temp != ',' && temp != '-')
            {
                errorCode = 0;
            }
        }
    }

    for (int i = 2; i < l - 4; i++)
    { // keeping the counting part separate for clarity
        string str = tokens[i];
        for (size_t it = 0; it < str.size(); it++)
        {
            char temp = str[it];
            if (temp == '-')
            {
                countDash += 1;
            }
            else if (temp == ',')
            {
                countComma += 1;
            }
        }
    }

    if (tableExists != 0)
    {
        schema_t tableSchema = get_schema_from_schema(table_name + suffix);

        if (countDash == 1 && countComma == 0)
        {
            string rowdIds = tokens[2];

            if (rowdIds[0] != '-' && rowdIds[rowdIds.length() - 1] != '-')
            {
                string lowerLim = "";
                string upperLim = "";
                int dashIdx = -1;
                for (size_t iter = 0; iter < rowdIds.size(); iter++)
                {
                    if (rowdIds[iter] == '-')
                    {
                        dashIdx = iter;
                        break;
                    }
                }
                lowerLim = rowdIds.substr(0, dashIdx);
                upperLim = rowdIds.substr(dashIdx + 1, rowdIds.length() - dashIdx - 1);
                int lLim = stoi(lowerLim);
                int uLim = stoi(upperLim);
                if (lLim >= uLim || lLim >= tableSchema.num_rows || uLim >= tableSchema.num_rows)
                {
                    errorCode = 1;
                }
            }
            else
            {
                errorCode = 1;
            }
        }

        else if (countComma != 0 && countDash == 0)
        {
            for (int y = 2; y < l - 4; y++)
            {
                if ((y % 2 == 0) && (tokens[y] == ","))
                {
                    errorCode = 2;
                }
                else if ((y % 2 == 1) && (tokens[y] != ","))
                {
                    errorCode = 2;
                }
            }
            for (int y = 2; y < l - 4; y = y + 2)
            {
                if (stoi(tokens[y]) >= tableSchema.num_rows)
                {
                    errorCode = 1;
                }
            }
        }

        else if (countComma == 0 && countDash == 0)
        {
            if (l != 7)
            {
                errorCode = 3;
            }
            else if (stoi(tokens[2]) >= tableSchema.num_rows)
            {
                errorCode = 1;
            }
        }

        else
        {
            errorCode = 4;
        }
    }

    if (tableExists == 0)
    {
        logger("Error: Table does not exist! \n", LOG_ERROR);
        logger("Tables availaible are:\n",LOG_WARNING);
        display_tables();
    }

    else if (errorCode == 0)
    {
        logger("Error: Wrong syntax while specifying row id \n", LOG_ERROR);
    }

    else if (errorCode == 1)
    {
        logger("Error: Check id numbers \n", LOG_ERROR);
    }

    else if (errorCode == 2)
    {
        logger("Error: Check your commas \n", LOG_ERROR);
    }

    else if (errorCode == 3)
    {
        logger("Error: Id numbers should be separated by commas \n", LOG_ERROR);
    }

    else if (errorCode == 4)
    {
        logger("Error: Check your syntax \n", LOG_ERROR);
    }

    else
    {
        delete_row(tokens);
    }
}