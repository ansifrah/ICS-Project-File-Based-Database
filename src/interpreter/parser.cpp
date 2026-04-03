#include "interpreter/interpreter.h"

void parser() {
    printf("Welcome to our interpreter!!!\n");
    while (true) {
        printf("~$ ");
        string query;
        getline(cin, query);
        
        vector<string> tokens = tokenize(query);
        vector<string> lower_tok;
        int l = tokens.size();

        // for (int i = 0; i<l; i++) {
        //     string s = tokens[i];
        //     string lower = "";
        //     int k = s.size();
        //     for (int j = 0; j<k; j++) {
        //         lower += (char) tolower((unsigned char) s[j]);
        //     }
        //     lower_tok.push_back(lower);
        // }

        // making lower case tokens
        for (int i=0; i<l; i++){
            string s = tokens[i];
            for (int j=0; j<(int)s.size(); j++){
                s[j] = (char)tolower(s[j]);
            }
            lower_tok.push_back(s);
        }

        if (lower_tok[0] == "exit") {
            break;
        }

        if (lower_tok[0] == "create" && lower_tok[1] == "table" && tokens[l-1] == ";" && tokens[3] == "(" && tokens[l-2] == ")") {
            check_createTable(tokens, lower_tok);
        }
        
        else if (lower_tok[0] == "insert" && lower_tok[1] == "into" && lower_tok[3] == "values" && tokens[4] == "(" && tokens[l-2] == ")" && tokens[l-1] == ";") {
            check_insertRow(tokens, lower_tok);
        }

        else if (lower_tok[0] == "select" && tokens[l-1] == ";") {
            int from = 0;
            for (int i = 2; i<l-2; i++) {
                if (lower_tok[i] == "from") {
                    from = 1;
                }
            }

            if (from == 0) {
                printf("Error: FROM not found! \n");
            }

            else {
                read_table(tokens);
            }
        }

        else if (lower_tok[0] == "delete" && lower_tok[l-3] == "from" && lower_tok[l-1] == ";" && lower_tok[l-4] == ")" && lower_tok[1] == "(") {
            check_deleteRow(tokens);
        }

        // else if (lower_tok[0] == "update" && lower_tok[2] == "set" && lower_tok[4] == "=" && lower_tok[6] == "where" && lower_tok[8] == "=" && lower_tok[10] == ";") {
        //     check_modifyRow(tokens, lower_tok);
        // }

        else if (lower_tok[0] == "display" && lower_tok[1] == "tables" && lower_tok[2] == ";" && lower_tok.size() == 3) {
            display_tables();
        }

        else {
            printf("Error: Check your syntax! \n");
        }
    }
}