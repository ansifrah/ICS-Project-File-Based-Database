#include "interpreter/interpreter.h"
#include "engine/TableRead.h"

//this function will only show 1 or 2 errors at a time
//if user has made many mistakes in query, many attempts will be made by user until they get the right query

void check_createTable(vector<string> tokens, vector<string> lower_tok) {
    int l = tokens.size();
    int flag = 0;
    //checking for uniqueness of table name
    string table_name = tokens[2];
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

    //checking if columns are defind properly
    //checking each column individually
    vector<string> col;
    vector<vector<string>> colData;
    vector<string> colNames = {" "};
    vector<string> dataTypes = {"INT", "UNSIGNED_INT", "BOOLEAN", "FLOATING_POINT", "DOUBLE_FLOATING_POINT", "UNSIGNED_CHAR", "NULL_TYPE"};
//did not include string in this as it is a special case
    for (int z = 4; z<l-2; z++) {
        if (tokens[z] != ",") {
            col.push_back(tokens[z]);
        }

        else {
            colData.push_back(col);
            col = {};
        }
    }
    colData.push_back(col);
    col = {};
    int errorCode = -1;
    //0 if colname is not unique
    //1 for inavild data type
    //2 if colname len > 255
    //3 for primary key error
    //4 string error

    int r = colData.size();
    for (int w = 0; w<r; w++) {
        col = colData[w];
        int t = col.size();

        if (t == 2) {
            string colName = col[0];
            string lowerCol = "";
            int kl = colName.size();
            for (int kl1 = 0; kl1<kl; kl1++) {
                lowerCol += (char) tolower((unsigned char) colName[kl1]);
            }
            if (lowerCol == "int" || lowerCol == "unsigned_int" || lowerCol == "string" || lowerCol == "boolean" || lowerCol == "floating_point" || lowerCol == "double_floating_point" || lowerCol == "unsigned_char") {
                flag = 1;
            }
            string dataType = "";
            int w1 = col[1].size();
            for (int iter = 0; iter<w1; iter++) {
                dataType += (char) toupper((unsigned char) col[1][iter]);
            }
            int a1 = colNames.size();
            for (int iter = 0; iter < a1; iter++) {
                if (colNames[iter] == colName) {
                    errorCode = 0;
                }
            }
            if (colName.size() > 255) {
                errorCode = 2;
            }
            colNames.push_back(colName);
            int exists = 0;
            int a2 = dataTypes.size();
            for (int iter = 0; iter < a2; iter++) {
                if (col[1] == dataTypes[iter]) {
                    exists = 1;
                }
            }
            if (col[1] == "STRING") {
                errorCode = 4;
            }
            else if (exists == 0) {
                errorCode = 1;
            }
        }

        else if (t == 4) {
            string colName = col[0];
            string lowerCol = "";
            int kl = colName.size();
            for (int kl1 = 0; kl1<kl; kl1++) {
                lowerCol += (char) tolower((unsigned char) colName[kl1]);
            }
            if (lowerCol == "int" || lowerCol == "unsigned_int" || lowerCol == "string" || lowerCol == "boolean" || lowerCol == "floating_point" || lowerCol == "double_floating_point" || lowerCol == "unsigned_char") {
                flag = 1;
            }
            string dataType = "";
            int w1 = col[1].size();
            for (int iter = 0; iter<w1; iter++) {
                dataType += (char) toupper((unsigned char) col[1][iter]);
            }
            int a1 = colNames.size();
            for (int iter = 0; iter < a1; iter++) {
                if (colNames[iter] == colName) {
                    errorCode = 0;
                }
            }
            if (colName.size() > 255) {
                errorCode = 2;
            }
            colNames.push_back(colName);
            int exists = 0;
            int a2 = dataTypes.size();
            for (int iter = 0; iter < a2; iter++) {
                if (col[1] == dataTypes[iter]) {
                    exists = 1;
                }
            }
            if (col[1] == "STRING") {
                errorCode = 4;
            }
            else if (exists == 0) {
                errorCode = 1;
            }

            string s1 = col[2];
            string s2 = col[3]; 
            //converting lowercase before checking if it is a primary key
            string s1l = "";
            string s2l = "";
            int a5 = s1.size();
            for (int iter = 0; iter<a5; iter++) {
                s1l += (char) tolower((unsigned char) s1[iter]);
            }
            int a6 = s2.size();
            for (int iter = 0; iter<a6; iter++) {
                s2l += (char) tolower((unsigned char) s2[iter]);
            }
            if (s1l != "primary" || s2l != "key") {
                errorCode = 3;
            }
        }

        else if (t == 5) {
            string colName = col[0];
            string lowerCol = "";
            int kl = colName.size();
            for (int kl1 = 0; kl1<kl; kl1++) {
                lowerCol += (char) tolower((unsigned char) colName[kl1]);
            }
            if (lowerCol == "int" || lowerCol == "unsigned_int" || lowerCol == "string" || lowerCol == "boolean" || lowerCol == "floating_point" || lowerCol == "double_floating_point" || lowerCol == "unsigned_char") {
                flag = 1;
            }
            string dataType = "";
            int w1 = col[1].size();
            for (int iter = 0; iter<w1; iter++) {
                dataType += (char) toupper((unsigned char) col[1][iter]);
            }
            int a1 = colNames.size();
            for (int iter = 0; iter < a1; iter++) {
                if (colNames[iter] == colName) {
                    errorCode = 0;
                }
            }
            if (colName.size() > 255) {
                errorCode = 2;
            }
            colNames.push_back(colName);
            if (col[1] != "STRING" || col[2] != "(" || col[4] != ")") {
                errorCode = 4;
            }
            else {
                int b1 = col[3].size();
                int wrongSymb = 0;
                for (int iter = 0; iter < b1; iter++) {
                    if (col[3][iter] == '.' || col[3][iter] == '-') {
                        wrongSymb = 1;
                    }
                }
                if (wrongSymb == 1) {
                    errorCode = 5;
                }
                else if (stoi(col[3]) == 0) {
                    errorCode = 5;
                }
            }
        }

        else if (t == 7) {
            string colName = col[0];
            string lowerCol = "";
            int kl = colName.size();
            for (int kl1 = 0; kl1<kl; kl1++) {
                lowerCol += (char) tolower((unsigned char) colName[kl1]);
            }
            if (lowerCol == "int" || lowerCol == "unsigned_int" || lowerCol == "string" || lowerCol == "boolean" || lowerCol == "floating_point" || lowerCol == "double_floating_point" || lowerCol == "unsigned_char") {
                flag = 1;
            }
            string dataType = "";
            int w1 = col[1].size();
            for (int iter = 0; iter<w1; iter++) {
                dataType += (char) toupper((unsigned char) col[1][iter]);
            }
            int b2 = colNames.size();
            for (int iter = 0; iter < b2; iter++) {
                if (colNames[iter] == colName) {
                    errorCode = 0;
                }
            }
            if (colName.size() > 255) {
                errorCode = 2;
            }
            colNames.push_back(colName);
            if (col[1] != "STRING" || col[2] != "(" ||  col[4] != ")") {
                errorCode = 4;
            }
            else {
                int b1 = col[5].size();
                int wrongSymb = 0;
                for (int iter = 0; iter < b1; iter++) {
                    if (col[3][iter] == '.' || col[3][iter] == '-') {
                        wrongSymb = 1;
                    }
                }
                if (wrongSymb == 1) {
                    errorCode = 5;
                }
                else if (stoi(col[3]) == 0) {
                    errorCode = 5;
                }
            }
            string s1 = col[5];
            string s2 = col[6]; 
            //converting lowercase before checking if it is a primary key
            string s1l = "";
            string s2l = "";
            int c1 = s1.size();
            int c2 = s2.size();
            for (int iter = 0; iter<c1; iter++) {
                s1l += (char) tolower((unsigned char) s1[iter]);
            }
            for (int iter = 0; iter<c2; iter++) {
                s2l += (char) tolower((unsigned char) s2[iter]);
            }
            if (s1l != "primary" || s2l != "key") {
                errorCode = 3;
            }
        }

        else {
            errorCode = 6;
        }
    }

    //making sure that one and only primary key exists
    int pk = 0;
    for (int i = 6; i<l-3; i++) {
        if (lower_tok[i] == "primary" && lower_tok[i+1] == "key") {
            pk += 1;
        }
    }
          
    //making sure that keywords are not used for column names
    for (int i = 4; i<l-2; i++) {
        if (lower_tok[i] == "create" || lower_tok[i] == "table" || lower_tok[i] == ";" || lower_tok[i] == "insert" || lower_tok[i] == "into" || lower_tok[i] == "values" || lower_tok[i] == "select" || lower_tok[i] == "from" || lower_tok[i] == "where" || lower_tok[i] == "delete" || lower_tok[i] == "set") {
            flag = 1;
            break;
        }
    }

    if (pk == 0) {
        printf("Error: PRIMARY KEY not found! \n");

        if (flag == 1) {
            printf("Error: Attempt to name column after Keyword or \';\' or data type! \n");
            printf("Keywords: \"CREATE, TABLE, INSERT, INTO, VALUES, SELECT, FROM, WHERE, DELETE, SET\" \n");
        }
    }

    else if (pk > 1) {
        printf("only 1 PRIMARY KEY is allowed \n");

        if (flag == 1) {
            printf("Error: Attempt to name column after Keyword or \';\' or data type! \n");
            printf("Keywords: \"CREATE, TABLE, INSERT, INTO, VALUES, SELECT, FROM, WHERE, DELETE, SET\" \n");
        }
    }

    else if (tableExists == 1) {
        printf("table name already exists \n");
    }

    else if (table_name.size() > 255) {
        printf("table name can have a maximum of 255 characters \n");
    }

    else if (flag == 1) {
        printf("Error: Attempt to name column after Keyword or \';\' or data type! \n");
        printf("Keywords: \"CREATE, TABLE, INSERT, INTO, VALUES, SELECT, FROM, WHERE, DELETE, SET\" \n");
    }

    else if (errorCode == 0) {
        printf("Column names are not unique \n");
    }

    else if (errorCode == 1) {
        printf("Check data types \n");
    }

    else if (errorCode == 2) {
        printf("Column name is too long, maximum of 255 characters \n");
    }

    else if (errorCode == 3) {
        printf("Did you mean PRIMARY KEY? \n");
    }

    else if (errorCode == 4) {
        printf("error while declaring string column \n");
    }

    else if (errorCode == 5) {
        printf("string length should be an integer greater than 0 \n");
    }

    else if (errorCode == 6) {
        printf("Error: Check your syntax \n");
    }

    //if all conditions are passed table is created
    else {
        create_table(tokens);
    }
}