#include "interpreter/interpreter.h"
#include "engine/TableWrite.h"
#include "engine/TableRead.h"

void insert_into_table(vector<string> tokens)
{
    // get table name from the tokenized input (should be around index 2)
    string table_name = tokens[2];
    // load the schema file to know what columns and data types we're dealing with
    schema_t new_schema = get_schema_from_schema(table_name + "__schema_data.bin");
    size_t num_cols = new_schema.num_cols;
    
    // if table doesn't exist, schema will be empty (num_cols = 0)
    if (num_cols == 0)
    {
        logger("Cannot insert data: table does not exist!\n", LOG_ERROR);
        logger("Tables availaible are:\n", LOG_WARNING);
        display_tables();
        return;
    }

    // move past "VALUES" keyword to start of actual data
    auto j = tokens.begin() + 4;

    // keep going through tokens until we've processed all rows
    while (j != tokens.end())
    {
        // look for opening parenthesis which marks start of a row
        if (*j == "(")
        {
            j++;

            // this will hold all the column values for this row
            vector<row_data_t> data_to_insert;

            // loop through each column in the schema
            for (size_t i = 0; i < num_cols; i++)
            {
                string value = *j;
                // NULL values get treated differently - just store a nullptr
                if (value == "NULL")
                {
                    // represent null cell with nullptr
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), shared_ptr<void>(nullptr)});
                    // move past NULL keyword and skip the comma if there is one
                    j++;
                    if (j != tokens.end() && *j == ",")
                        j++;
                    continue; // move to next column
                }

                // convert the string value to the right data type based on schema
                switch (new_schema.column_data[i].data_type)
                {
                case INT:
                {
                    // parse as regular integer
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<int>(stoi(value))});
                    break;
                }

                case UNSIGNED_INT:
                {
                    // parse as unsigned int, cast to the right type
                    unsigned int v = static_cast<unsigned int>(stoul(value));
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<unsigned int>(v)});
                    break;
                }

                case STRING:
                {
                    // remove quotes from string value
                    string_extractor(value);
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<string>(value)});
                    break;
                }

                case BOOLEAN:
                {
                    // convert "true"/"false" strings to actual boolean
                    bool v = (value == "true");
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<bool>(v)});
                    break;
                }

                case FLOATING_POINT:
                {
                    // parse as float
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<float>(stof(value))});
                    break;
                }

                case DOUBLE_FLOATING_POINT:
                {
                    // parse as double for better precision
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<double>(stod(value))});
                    break;
                }

                case UNSIGNED_CHAR:
                {
                    // parse integer and cast to unsigned char
                    unsigned char v = static_cast<unsigned char>(stoi(value));
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<unsigned char>(v)});
                    break;
                }

                default:
                {
                    logger("Unknown data type\n", LOG_ERROR);
                    return;
                }
                }

                j++; // move to next token

                if (*j == ",") // skip comma between values
                    j++;
            }

            // write this row to the table file
            int result = append_record_to_table(new_schema, data_to_insert);

            if (result != 0)
            {
                logger("error inserting row\n", LOG_ERROR);
                return;
            }

            // skip everything until we find the next row or reach the end
            while (j != tokens.end() && *j != "(")
                j++;
        }
        else
        {
            // skip any tokens that aren't the start of a row
            j++;
        }
    }

    logger("all rows inserted successfully!\n", LOG_SUCCESS);
}