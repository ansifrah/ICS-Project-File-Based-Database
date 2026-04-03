#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <bits/stdc++.h>
using namespace std;
#include <filesystem>

vector<string> tokenize(const string& s);
bool is_valid_table_name(const string& s);
bool is_number(const string& s);
void string_extractor(string &value);
void create_table(vector<string> tokens);
void insert_into_table(vector<string> tokens);
void read_table(vector<string> tokens);
void delete_row(vector<string> tokens);
void modify_row(vector<string> tokens);
void parser();
void display_tables();

void check_createTable(vector<string> tokens, vector<string> lower_tok);
void check_insertRow(vector<string> tokens, vector<string> lower_tok);
void check_deleteRow(vector<string> tokens);
void check_modifyRow(vector<string> tokens, vector<string> lower_tok);

#endif