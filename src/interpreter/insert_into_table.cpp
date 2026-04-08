#include "interpreter/interpreter.h"
#include "engine/TableWrite.h"
#include "engine/TableRead.h"

void insert_into_table(vector<string> tokens)
{
    string table_name = tokens[2];
    schema_t new_schema = get_schema_from_schema(table_name + "__schema_data.bin");
    size_t num_cols = new_schema.num_cols;
    if (num_cols == 0)
    {
        logger("Cannot insert data: table does not exist!\n", LOG_ERROR);
        logger("Tables availaible are:\n", LOG_WARNING);
        display_tables();
        return;
    }

    auto j = tokens.begin() + 4;

    while (j != tokens.end())
    {
        if (*j == "(")
        {
            j++;

            vector<row_data_t> data_to_insert;

            for (size_t i = 0; i < num_cols; i++)
            {
                string value = *j;
                if (*j == "'")
                {
                    j++;
                    value = *j;
                }
                if (value == "NULL")
                {
                    // represent null cell with nullptr
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), shared_ptr<void>(nullptr)});
                    // advance past NULL token and optional comma before next column
                    j++;
                    if (j != tokens.end() && *j == ",")
                        j++;
                    continue; // move to next column
                }

                switch (new_schema.column_data[i].data_type)
                {
                case INT:
                {
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<int>(stoi(value))});
                    break;
                }

                case UNSIGNED_INT:
                {
                    unsigned int v = static_cast<unsigned int>(stoul(value));
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<unsigned int>(v)});
                    break;
                }

                case STRING:
                {
                    string_extractor(value);
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<string>(value)});
                    break;
                }

                case BOOLEAN:
                {
                    bool v = (value == "true");
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<bool>(v)});
                    break;
                }

                case FLOATING_POINT:
                {
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<float>(stof(value))});
                    break;
                }

                case DOUBLE_FLOATING_POINT:
                {
                    data_to_insert.push_back(
                        {static_cast<unsigned char>(i), make_shared<double>(stod(value))});
                    break;
                }

                case UNSIGNED_CHAR:
                {
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

                if (*j == "," || *j == "'") // skip comma between values
                    j++;
            }

            int result = append_record_to_table(new_schema, data_to_insert);

            if (result != 0)
            {
                logger("error inserting row\n", LOG_ERROR);
                return;
            }

            // move until next row or end
            while (j != tokens.end() && *j != "(")
                j++;
        }
        else
        {
            j++;
        }
    }

    logger("all rows inserted successfully!\n", LOG_SUCCESS);
}