#include "utils/utils.h"

bool string_size_check(string data) {
    return data.size() <= MAX_COL_LEN;
}

char* get_c_str_buffer_col_data(string data){
    char *buffer = new char[MAX_COL_LEN]();
    strcpy(buffer, data.c_str());
    return buffer;
}

size_t get_size_col_data_type(col_item_t column_data) {
    col_data_type_t data_type = column_data.data_type;

    switch (data_type) {
        case INT:
            return sizeof(int);
        case UNSIGNED_INT:
            return sizeof(unsigned int);
        case STRING:
            return column_data.max_str_len;
        case BOOLEAN:
            return sizeof(bool);
        case FLOATING_POINT:
            return sizeof(float);
        case DOUBLE_FLOATING_POINT:
            return sizeof(double);
        case UNSIGNED_CHAR:
            return sizeof(unsigned char);
        default:
            return 0;
    }
}

size_t get_size_data_exp_string(col_data_type_t data_type){
    switch(data_type){
        case INT: {
            return sizeof(int);
        }
        case UNSIGNED_INT: {
            return sizeof(unsigned int);
        }
        case BOOLEAN: {
            return sizeof(bool);
        }
        case FLOATING_POINT: {
            return sizeof(float);
        }
        case DOUBLE_FLOATING_POINT: {
            return sizeof(double);
        }
        case UNSIGNED_CHAR: {
            return sizeof(unsigned char);
        }
        default:
            return 0;
    }
}