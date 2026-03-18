#include<stdio.h>
#include"utils/logging.h"
#include"engine/TableCreate.h"
#include"engine/TableWrite.h"
#include"engine/TableRead.h"
#include"engine/TableModify.h"
#include"engine/TableDelete.h"
#include "interpreter/interpreter.h"

// void sample_testing(){
//     logger("Test info\n", LOG_INFO);
//     logger("Test success\n", LOG_SUCCESS);
//     logger("Test error\n", LOG_ERROR);
//     logger("Test warning\n", LOG_WARNING);

//     vector<string> columns = {"Name", "Email"};
//     vector<vector<string>> rows = {
//         { "John Doe", "b25xx10xx@iitj.ac.in" },
//         { "Ben Dover", "b24yy101x@iitj.ac.in" },
//     };

//     log_table_t sample_table = {
//         .num_rows = (int)rows.size(),
//         .num_cols = (int)columns.size(),
//         .row_data = rows,
//         .col_data = columns,
//     };
    
//     log_table(&sample_table);
// }

void testing_engine(){
    col_item_t id_col = {
        .is_primary_key = true,
        .col_id = 0,
        .data_type = UNSIGNED_INT,
        .col_name = "ID",
        .is_string = false,
        .max_str_len = 0,
    };

    col_item_t roll_col = {
        .is_primary_key = false,
        .col_id = 1, 
        .data_type = STRING,
        .col_name = "Roll No",
        .is_string = true,
        .max_str_len = 10,
    };

    col_item_t name_col = {
        .is_primary_key = false,
        .col_id = 2,
        .data_type = STRING,
        .col_name = "Name",
        .is_string = true,
        .max_str_len = 255
    };

    vector<col_item_t> column_data = {id_col, roll_col, name_col};

    schema_t new_schema;

    new_schema.table_name = "Students";
    new_schema.num_cols = (int)column_data.size();
    new_schema.num_rows = 0;
    new_schema.column_data = column_data;

    int res = create_new_table_schema(new_schema);

    if (res != -1) logger("Successfully created schema\n", LOG_SUCCESS);
    else logger("Error in creating schema!\n", LOG_ERROR);
// //testing by ashhar
//     schema_t table_schema=get_schema_from_schema("Student__schema_data.bin");
//         for (size_t i = 0; i < table_schema.num_cols; i++)
//     {
//         cout << table_schema.column_data[i].is_primary_key<< "\n";
//         cout << table_schema.column_data[i].col_id<< "\n";
//         cout << table_schema.column_data[i].data_type << "\n";
//         cout << table_schema.column_data[i].col_name<< "\n";
//         cout << table_schema.column_data[i].is_string << "\n";
//         cout << table_schema.column_data[i].max_str_len<< "\n";
//     }


    vector<row_data_t> data_to_insert = {
        {0, make_shared<int>(0)},
        {1, make_shared<string>("b25bb1012")},
        {2, make_shared<string>("Harish Babu Balaji")},
    };

    vector<row_data_t> data_to_insert_2 = {
        {0, make_shared<int>(0)},
        {1, make_shared<string>("b25ee1001")},
        {2, make_shared<string>("Abhishek Reddy")},
    };

    vector<row_data_t> data_to_insert_3 = {
        {0, make_shared<int>(0)},
        {1, make_shared<string>("b25me1002")},
        {2, make_shared<string>("Rahul")},
    };


    res = append_record_to_table(new_schema, data_to_insert);
    int res2 = append_record_to_table(new_schema, data_to_insert_2);
    int res3 = append_record_to_table(new_schema, data_to_insert_3);

    if (res != -1 && res2 != -1 && res3 != -1) logger("Successfully Written Data\n", LOG_SUCCESS);
    else logger("Error in writing data!\n", LOG_ERROR);
}

void testing_read(){
    schema_t test_schema_of_schema = get_schema_from_schema("Students__schema_data.bin");

    cout << test_schema_of_schema.table_name << endl;
    cout << static_cast<unsigned int>(test_schema_of_schema.num_cols) << endl;
        cout << test_schema_of_schema.num_rows << endl;
        cout << test_schema_of_schema.total_row_len_inbytes << endl;
        
    for (int i = 0; i < test_schema_of_schema.num_cols; i++)
    {
        cout << static_cast<unsigned int>(test_schema_of_schema.column_data[i].col_id) <<" ";
        cout << test_schema_of_schema.column_data[i].col_name << " ";
        cout << test_schema_of_schema.column_data[i].is_primary_key << " ";
        cout << static_cast<unsigned int>(test_schema_of_schema.column_data[i].data_type)<<" ";
        cout << test_schema_of_schema.column_data[i].is_string <<" ";
        cout << test_schema_of_schema.column_data[i].max_str_len << endl;
    }
    // cell_data_t cellData = get_table_cell_data(1, 2, test_schema_of_schema);

    // cout << "data type " << cellData.cell_data_type << endl;

    // switch (cellData.cell_data_type)
    // {
    //     case INT:
    //         cout << *static_cast<int*>(cellData.cell_data.get());
    //         break;

    //     case UNSIGNED_INT:
    //         cout << *static_cast<unsigned int*>(cellData.cell_data.get()) << endl;
    //         break;

    //     case BOOLEAN:
    //         cout << (*static_cast<bool*>(cellData.cell_data.get()) ? "true" : "false") << endl;
    //         break;

    //     case FLOATING_POINT:
    //         cout << *static_cast<float*>(cellData.cell_data.get()) << endl;
    //         break;

    //     case DOUBLE_FLOATING_POINT:
    //         cout << *static_cast<double*>(cellData.cell_data.get()) << endl;
    //         break;

    //     case UNSIGNED_CHAR:
    //         cout << static_cast<unsigned int>(
    //             *static_cast<unsigned char*>(cellData.cell_data.get())
    //         ) << endl;
    //         break;

    //     case STRING:
    //         cout << static_cast<char*>(cellData.cell_data.get()) << endl;
    //         break;

    //     case NULL_TYPE:
    //         cout << "Null Data read" << endl;
    //         break;
    // }
}

void testing_modify()
{
    schema_t test_schema_of_schema = get_schema_from_schema("Students__schema_data.bin");

    cell_data_t after = {
        .cell_data_type = STRING,
        .cell_data = make_shared<string>("Abhishek Reddy N")
    };
    cell_data_modify(test_schema_of_schema, after, 1, 2);
}

// void testing_modify()
// {
//     schema_t test_schema_of_schema = get_schema_from_schema("Students__schema_data.bin");

//     cell_data_t after = {
//         .cell_data_type = STRING,
//         .cell_data = make_shared<string>("b25me1067")
//     };
//     cell_data_modify(test_schema_of_schema, after, 2, 1);
// }

// void testing_table_delete(){
//     schema_t table_schema = get_schema_from_schema("Students__schema_data.bin");

//     int res = delete_row_from_table(0, table_schema);

//     if (res != -1) logger("Successfully Deleted Row\n", LOG_SUCCESS);
//     else logger("Error in deleting data!\n", LOG_ERROR);
// }


int main(){
    // testing_engine();
    // testing_read();
    // sample_testing();
    // testing_modify();
    // vector<string> s=tokenize("create roll from Table(1,ashhar,3)");
    // for(auto i=s.begin();i<s.end();i++){
    //     cout<<(*i)<<endl;
    // }
    // create_table(tokenize("CREATE TABLE Student(ID INT, ROLL_NO STRING PRIMARY_KEY(10), Name STRING(20),Department STRING(20) );"));
    // insert_into_table(tokenize("INSERT INTO Student (1,b25ee1009,Ashhar);"));
 create_table(tokenize("create TABLE Test(ID INT primary key,Name STRING(20));"));
insert_into_table(tokenize(
"insert INTO Test VALUES (1,'Ansari'),(2,'Ifrah');"));
// delete_row("Student",0);
// insert_into_table(tokenize("INSERT INTO Student VALUES(0,NULL,'Ashhar','Electrical'),(1,'b25bb1009',NULL,'Bio');"));

// show contents after insert to verify NULL handling
read_table(tokenize("SELECT * FROM Test;"));
// delete_row(tokenize("DELETE (0,1) FROM Test"));
// read_table(tokenize("SELECT * from Test;"));
    // cout<<is_identifier("PEROSN_ID");
    // read_table(tokenize("SELECT ID,NAME FROM STUDENTS;"));
    // vector<string> tokens=tokenize("'Hello John' my name is 20.89");
    // vector<string> tokens=tokenize("1-4");
    // for(auto i=tokens.begin();i<tokens.end();i++){
    //     cout<<*i<<" ";
    // }
    return 0;
}