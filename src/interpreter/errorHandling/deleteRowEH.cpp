#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

void check_deleteRow(vector<string> tokens) {
    int l = tokens.size();

    string table_name = tokens[l-2];
    int tableExists = 0;
    vector<string> tablenames;
    namespace fs = filesystem;
    string suffix = "__schema_data.bin";
    for (const auto &e : fs::directory_iterator(".")) { 
        string f = e.path().filename().string();
        if (f.size() > suffix.size() && f.compare(f.size() - suffix.size(), suffix.size(), suffix) == 0) {
            string tableName = f.substr(0, f.size() - suffix.size());
            tablenames.push_back(tableName);
        }
    }
    int u = tablenames.size();
    for (int y = 0; y<u; y++) {
        if (tablenames[y] == table_name) {
            tableExists = 1;
            break;
        }
    }

    int errorCode = -1;
    //0 for using incorrect characters in row id
    //1 if id numbers not in range
    //2 error related to comma
    //3 id numbers not separated by comma
    //4 for general syntax error

    int countDash = 0;
    int countComma = 0;
    for (int i = 2; i < l-4; i++) {
        string str = tokens[i];
        int l1 = str.size();
        for (int j = 0; j < l1; j++) {
            if ((str[j] < '0' || str[j] > '9') && str[j] != ',' && str[j] != '-') {
                errorCode = 0;
            }
        }
    }

    for (int i = 2; i < l-4; i++) { //keeping the counting part separate for clarity
        if (tokens[i] == "-") {
                countDash += 1;
        }
        else if (tokens[i] == ",") {
            countComma += 1;
        }
    }

    if (tableExists != 0) {
        schema_t tableSchema = get_schema_from_schema(table_name + suffix);

        if (countDash == 1 && countComma == 0) {
            if (tokens[2] != "-" && tokens[4] != "-") {
                int lowerLim = stoi(tokens[2]);
                int upperLim = stoi(tokens[4]);
                if (l != 9) {
                    errorCode = 4;
                }
                else if ((lowerLim > tableSchema.num_rows) || (upperLim > tableSchema.num_rows) || (lowerLim > upperLim)) {
                    errorCode = 1;
                }
            }
            else {
                errorCode = 4;
            }
        }

        else if (countComma != 0 && countDash == 0) {
            for (int y = 2; y < l-4; y++) {
                if ((y % 2 == 0) && (tokens[y] == ",")) {
                    errorCode = 2;
                }
                else if ((y % 2 == 1) && (tokens[y] != ",")) {
                    errorCode = 2;
                }
            }
            for (int y = 2; y < l-4; y = y+2) {
                if (stoi(tokens[y]) > tableSchema.num_rows) {
                    errorCode = 1;
                }
            }
        }

        else if (countComma == 0 && countDash == 0) {
            if (l != 7) {
                errorCode = 3;
            }
            else if (stoi(tokens[2]) > tableSchema.num_rows) {
                errorCode = 1;
            }
        }

        else {
            errorCode = 4;
        }
    }

    if (tableExists == 0) {
        printf("table does not exist! \n");
    }

    else if (errorCode == 0) {
        printf("wrong syntax while specifying row id \n");
    }

    else if (errorCode == 1) {
        printf("check id numbers \n");
    }

    else if (errorCode == 2) {
        printf("check your commas \n");
    }

    else if (errorCode == 3) {
        printf("id numbers should be separated by commas \n");
    }

    else if (errorCode == 4) {
        printf("error: check your syntax \n");
    }

    else {
        delete_row(tokens);
    }
}