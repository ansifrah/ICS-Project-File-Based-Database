#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

void check_insertRow(vector<string> tokens, vector<string> lower_tok) {
    int flag = 0;
    int l = tokens.size();

    string table_name = tokens[2];
    // int tableExists = 0;
    // vector<string> tablenames;
    // namespace fs = filesystem;
    string suffix = "__schema_data.bin";
    // for (const auto &e : fs::directory_iterator(".")) { 
    //     string f = e.path().filename().string();
    //     if (f.size() > suffix.size() && f.compare(f.size() - suffix.size(), suffix.size(), suffix) == 0) {
    //         string tableName = f.substr(0, f.size() - suffix.size());
    //         tablenames.push_back(tableName);
    //     }
    // }
    // int u = tablenames.size();
    // for (int y = 0; y<u; y++) {
    //     if (tablenames[y] == table_name) {
    //         tableExists = 1;
    //         break;
    //     }
    // }

    vector<string> row;
    vector<vector<string>> rows;
    string temp = "";
    bool in_quote = false;

    for (int iter = 4; iter < l-2; iter++) { 
        if ((!in_quote) && (tokens[iter] == "(" || tokens[iter] == ",")) {
            continue;
        }
        if (tokens[iter] == "\'") {
            row.push_back("\'" + tokens[iter+1] + "\'");
            iter += 2;
            continue;
        }
        else {
            row.push_back(tokens[iter]);
        }
        if(tokens[iter] == ")") {
            rows.push_back(row);
            row = {};
        }
    }

    schema_t tableSchema = get_schema_from_schema(table_name + suffix);

    int errorCode = -1;
//0 for incorrect row length
//1 if string length exceeds limit
//2 if integer is given incorrectly in input
//3 if unsigned integer is given incorrectly in input
//4 for true/false
//5 for float
//6 for double
//7 for unsigned char
//8 if primary key is not unique
    
    vector<string> primKeys;
    size_t id;
    for (size_t it = 0; it<tableSchema.num_cols; it++) {
        if (tableSchema.column_data[it].is_primary_key) {
            id = tableSchema.column_data[it].col_id;
        }
    }
    for (int it = 0; it < tableSchema.num_rows; it++) {
        cell_data_t cell = get_table_cell_data(it, id, tableSchema);
        if (cell.cell_data_type == INT) {
            int val = *static_cast<int*>(cell.cell_data.get());
            string data = to_string(val);
            primKeys.push_back(data);
        }
        else if (cell.cell_data_type == UNSIGNED_INT) {
            unsigned int val = *static_cast<unsigned int*>(cell.cell_data.get());
            string data = to_string(val);
            primKeys.push_back(data);
        }
        else if (cell.cell_data_type == STRING) {
            string val = string(static_cast<char*>(cell.cell_data.get()));
            primKeys.push_back(val);
        }
        else if (cell.cell_data_type == BOOLEAN) {
            bool val = *static_cast<bool*>(cell.cell_data.get());
            string data = "false";
            if (val == true) {
                data = "true";
            }
            primKeys.push_back(data);
        }
        else if (cell.cell_data_type == FLOATING_POINT) {
            float val = *static_cast<float*>(cell.cell_data.get());
            string data = to_string(val);
            primKeys.push_back(data);
        } 
        else if (cell.cell_data_type == DOUBLE_FLOATING_POINT) {
            double val = *static_cast<double*>(cell.cell_data.get());
            string data = to_string(val);
            primKeys.push_back(data);
        }
        else if (cell.cell_data_type == UNSIGNED_CHAR) {
            unsigned char val = *static_cast<unsigned char*>(cell.cell_data.get());
            string data = to_string(static_cast<int>(val));
            primKeys.push_back(data);
        }
    }
    for (size_t iter1 = 0; iter1 < rows.size(); iter1++) {
        row = rows[iter1];
        if (row.size() != tableSchema.num_cols) {
            errorCode = 0;
            break;
        }
        string pkcheck = "";
        for (size_t iter2 = 0; iter2 < row.size(); iter2++) {
            string data = row[iter2];
            printf("hi");
            if (tableSchema.column_data[iter2].data_type == STRING) {
                if (data.size() < 2 || data.front() != '\'' || data.back() != '\'') {
                    errorCode = 9;                    
                }
                else {
                    string strVal = data.substr(1, data.size() - 2);
                    if (tableSchema.column_data[iter2].max_str_len < strVal.size()) {
                        errorCode = 1;
                    }
                    else if (iter2 == id) {
                        int notUnique = 0;
                        for (size_t h = 0; h<primKeys.size(); h++) {
                            if (primKeys[h] == data) {
                                notUnique = 1;
                            }
                        }
                        if (notUnique == 1) {
                            errorCode = 8;
                        }
                        else {
                            pkcheck = data;
                        }
                    }
                }
            }
            else if (tableSchema.column_data[iter2].data_type == INT) {
                if (data.size() == 1 && (data[0] < '0' || data[0] > '9')) {
                    errorCode = 2;
                }
                else if (data.size() > 9) {
                    errorCode = 2;
                }
                else {
                    int fl = 0;
                    for (size_t i = 1; i < data.size(); i++) {
                        if (data[i] < '0' || data[i] > '9') {
                            fl = 1;
                        }
                    }
                    if (data[0] != '-' && (data[0] < '0' || data[0] > '9')) {
                        errorCode = 2;
                    }
                    else if (fl == 1) {
                        errorCode = 2;
                    }
                    else {
                        if (iter2 == id) {
                            int notUnique = 0;
                            for (size_t h = 0; h<primKeys.size(); h++) {
                                if (primKeys[h] == data) {
                                    notUnique = 1;
                                }
                            }
                            if (notUnique == 1) {
                                errorCode = 8;
                            }
                            else {
                                pkcheck = data;
                            }
                        }
                    }
                }
            }
            else if (tableSchema.column_data[iter2].data_type == UNSIGNED_INT) {
                int fl = 0;
                for (size_t i = 0; i<data.size(); i++) {
                    if (data[i] < '0' || data[i] > '9') {
                        fl = 1;
                    }
                }
                if (data.size()>10 || fl == 1) {
                    errorCode = 3;
                }
                else {
                    if (iter2 == id) {
                        int notUnique = 0;
                        for (size_t h = 0; h<primKeys.size(); h++) {
                            if (primKeys[h] == data) {
                                notUnique = 1;
                            }
                        }
                        if (notUnique == 1) {
                            errorCode = 8;
                        }
                        else {
                            pkcheck = data;
                        }
                    }                    
                }
            }
            else if (tableSchema.column_data[iter2].data_type == BOOLEAN) {
                if (data != "true" && data != "false") {
                    errorCode = 4;
                }
                else {
                    if (iter2 == id) {
                        int notUnique = 0;
                        for (size_t h = 0; h<primKeys.size(); h++) {
                            if (primKeys[h] == data) {
                                notUnique = 1;
                            }
                        }
                        if (notUnique == 1) {
                            errorCode = 8;
                        }
                        else {
                            pkcheck = data;
                        }
                    }
                }
            }
            else if (tableSchema.column_data[iter2].data_type == FLOATING_POINT || tableSchema.column_data[iter2].data_type == DOUBLE_FLOATING_POINT) {
                int dot = 0;
                for (size_t itera = 0; itera < data.size(); itera++) {
                    if (data[itera] == '.') {
                        dot = 1;
                    }
                }
                float f;
                int a = sscanf(data.c_str(), "%f", &f);
                if (a == 0 || dot == 0) {
                    errorCode = 5;
                }
                else {
                    if (iter2 == id) {
                        int notUnique = 0;
                        for (size_t h = 0; h<primKeys.size(); h++) {
                            if (primKeys[h] == data) {
                                notUnique = 1;
                            }
                        }
                        if (notUnique == 1) {
                            errorCode = 8;
                        }
                        else {
                            pkcheck = data;
                        }
                    }
                }
            }
            else if (tableSchema.column_data[iter2].data_type == UNSIGNED_CHAR) {
                int hl = 0;
                for (size_t itera = 0; itera < data.size(); itera++) {
                    if (data[itera] < '0' || data[itera] > '9') {
                        hl = 1;
                    }
                }
                int uchar = -1;
                if (!hl) {
                    uchar = stoi(data);
                }
                if (data.size() == 0 || hl == 1 || uchar > 255) {
                    errorCode = 7;
                }
                else {
                    if (iter2 == id) {
                        int notUnique = 0;
                        for (size_t h = 0; h<primKeys.size(); h++) {
                            if (primKeys[h] == data) {
                                notUnique = 1;
                            }
                        }
                        if (notUnique == 1) {
                            errorCode = 8;
                        }
                        else {
                            pkcheck = data;
                        }
                    }
                }
            }
        }
        if (errorCode == -1) {
            if (pkcheck != "") {
                primKeys.push_back(pkcheck);
            }
        }
    }
    for (size_t it = 0; it < primKeys.size(); it++) {
        printf("primKey: %s\n", primKeys[it].c_str());
    }

    for (int i = 5; i<l-2; i++) {
        if (lower_tok[i] == "create" || lower_tok[i] == "table" || lower_tok[i] == ";" || lower_tok[i] == "insert" || lower_tok[i] == "into" || lower_tok[i] == "values" || lower_tok[i] == "select" || lower_tok[i] == "from" || lower_tok[i] == "where" || lower_tok[i] == "delete" || lower_tok[i] == "set" || lower_tok[i] == "int" || lower_tok[i] == "unsigned_int" || lower_tok[i] == "boolean" || lower_tok[i] == "floating_point" || lower_tok[i] == "double_floating_point" || lower_tok[i] == "unsigned_char" || lower_tok[i] == "string" || lower_tok[i] == "null_type") {
        flag = 1;
        break;
        }
    }

    // if (tableExists == 0) {
    //     printf("table does not exist! \n");
    // }

    if (errorCode == 0) {
        printf("row length is incorrect \n");
    }

    else if (errorCode == 1) {
        printf("string length exceeds limit \n");
    }

    else if (errorCode == 2) {
        printf("integer given incorrectly \n");
    }

    else if (errorCode == 3) {
        printf("unsigned int given incorrectly \n");
    }

    else if (errorCode == 4) {
        printf("boolean value accepts only true or false \n");
    }

    else if (errorCode == 5) {
        printf("floating point given incorrectly \n");
    }

    else if (errorCode == 6) {
        printf("double floating point given incorrectly \n");
    }

    else if (errorCode == 7) {
        printf("unsigned char given incorrectly \n");
    }

    else if (errorCode == 8) {
        printf("column of primary key is not unique \n");
    }

    else if (errorCode == 9) {
        printf("string data must be within single quotes \n");
    }
    
    else if (flag == 1) {
        printf("Error: Attempt to insert to insert Keyword! \n");
        printf("Keywords: CREATE, TABLE, INSERT, INTO, VALUES, SELECT, FROM, WHERE, DELETE, SET, ; and data types \n");
    }

    else {
        insert_into_table(tokens);
    }    
}