# Database Engine Documentation

## Overview

This document describes the core API for the file-based database engine. The engine handles all the low-level operations for creating tables, reading data, writing records, modifying cells, and deleting rows. If you're building an interpreter or query engine on top of this database, this guide will help you understand what functions are available and how to use them correctly.

---

## Understanding Schemas

At the heart of the database engine is the schema, which defines the structure of a table. Think of a schema as the blueprint that tells the engine how to lay out data on disk.

### Schema Structure

A schema contains the following information:

- **table_name**: The name of the table (max 255 characters)
- **num_cols**: The number of columns in the table
- **num_rows**: How many rows currently exist in the table
- **total_row_len_inbytes**: The total size of a single row in bytes
- **column_data**: A vector containing details about each column

Each column carries this information:

- **col_id**: The column index (0-based)
- **col_name**: The name of the column (max 255 characters)
- **data_type**: The type of data stored in this column
- **is_primary_key**: Whether this column is a primary key
- **is_string**: Whether the column stores a string
- **max_str_len**: For string columns, the maximum length allowed

### File Storage

When you create a schema, the engine generates two binary files for that table:

1. **{table_name}__schema_data.bin**: Stores the schema information and column metadata
2. **{table_name}__table_data.bin**: Stores the actual row data

These files use a fixed binary format. The schema file contains offsets and metadata that allow the engine to locate and read specific cells in the table data file.

---

## Supported Data Types

The engine supports the following data types. You must specify the exact type when defining columns:

| Type | C++ Equivalent | Size | Details |
|------|---|---|---|
| INT | int | 4 bytes | Signed 32-bit integer |
| UNSIGNED_INT | unsigned int | 4 bytes | Unsigned 32-bit integer |
| STRING | char array | variable | Fixed-length string, padded to max_str_len |
| BOOLEAN | bool | 1 byte | True or false value |
| FLOATING_POINT | float | 4 bytes | Single precision floating point |
| DOUBLE_FLOATING_POINT | double | 8 bytes | Double precision floating point |
| UNSIGNED_CHAR | unsigned char | 1 byte | Single unsigned byte |
| NULL_TYPE | void | - | Used internally when data is uninitialized |

### String Handling

String columns require special attention. When you define a STRING column, you must specify `max_str_len`, which is the maximum number of characters that column can store. This size is important because the engine allocates fixed space for each string in the table file.

If you try to store a string longer than `max_str_len`, the engine will truncate it or reject it. When reading, the engine returns the raw character buffer, and you'll need to handle null termination properly.

---

## Creating a Schema and Table

To create a new table, you need to build a schema and pass it to the `create_new_table_schema()` function.

### Function Signature

```cpp
int create_new_table_schema(schema_t table_schema);
```

### What It Does

This function creates both the schema file and the initial table data file on disk. It initializes the row count to 0 and sets up all the metadata needed by the engine.

### Return Values

- **0**: Success. Schema created successfully.
- **-1**: Failure. Usually because the table name or a column name exceeds 255 characters.

### Example: Creating a Students Table

```cpp
#include "engine/TableCreate.h"

// Step 1: Define your columns
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
    .max_str_len = 10,  // Roll numbers are up to 10 chars
};

col_item_t name_col = {
    .is_primary_key = false,
    .col_id = 2,
    .data_type = STRING,
    .col_name = "Name",
    .is_string = true,
    .max_str_len = 255
};

// Step 2: Build the schema
vector<col_item_t> column_data = {id_col, roll_col, name_col};

schema_t new_schema;
new_schema.table_name = "Students";
new_schema.num_cols = (int)column_data.size();
new_schema.num_rows = 0;
new_schema.column_data = column_data;

// Step 3: Create the table
int result = create_new_table_schema(new_schema);

if (result == 0) {
    // Table created successfully
} else {
    // Error creating table
}
```

### Important Notes

- String columns must have `is_string = true` and a valid `max_str_len`.
- Non-string columns should have `is_string = false` and `max_str_len = 0`.
- The `col_id` values must start at 0 and increment by 1 for each column in order.
- All column names must be 255 characters or less.
- After calling this function, the schema files exist on disk but contain no data rows yet.

---

## Writing Data to Tables

Once you've created a schema, you can insert rows of data into the table.

### Function Signatures

```cpp
int append_record_to_table(schema_t &table_schema, vector<row_data_t> data);
```

### What It Does

This function appends a complete row to the table data file and updates the row count in the schema. The row is always added at the end of the table.

### Input Parameters

- **table_schema**: A reference to your schema. This will be modified to update the row count.
- **data**: A vector of `row_data_t` structs, one for each column in the table.

Each `row_data_t` contains:
- **col_id**: Which column this data is for
- **data**: A shared_ptr to the actual data

The data must be in column order and must match the types defined in the schema.

### Return Values

- **0**: Success. Row added.
- **-1**: Failure. Usually a file I/O error.

### Casting and Creating Shared Pointers

When you create the row data, you need to use `make_shared<T>()` to wrap your data in a shared pointer. Here's how to do it for different types:

```cpp
// For INT
make_shared<int>(42)

// For UNSIGNED_INT
make_shared<unsigned int>(100)

// For STRING
make_shared<string>("John Doe")

// For BOOLEAN
make_shared<bool>(true)

// For FLOATING_POINT
make_shared<float>(3.14f)

// For DOUBLE_FLOATING_POINT
make_shared<double>(3.14159)

// For UNSIGNED_CHAR
make_shared<unsigned char>(255)
```

### Example: Inserting a Student Record

```cpp
#include "engine/TableWrite.h"

schema_t students_schema; // Assume this is loaded or created
// ... (schema would contain the Students table definition from earlier)

vector<row_data_t> new_record = {
    {0, make_shared<unsigned int>(1001)},    // ID = 1001
    {1, make_shared<string>("b25bb1012")},   // Roll No = "b25bb1012"
    {2, make_shared<string>("John Doe")},    // Name = "John Doe"
};

int result = append_record_to_table(students_schema, new_record);

if (result == 0) {
    // Record inserted successfully
    // students_schema.num_rows is now updated
} else {
    // Error inserting record
}
```

### Important Notes

- The order of items in the `data` vector matters. They must be in the same order as your column definitions.
- For STRING columns, the string will be truncated to `max_str_len` if it's too long.
- The `append_record_to_table()` function modifies the schema object passed to it. Its `num_rows` field will be incremented.
- After insertion, the new row is written to disk immediately.

---

## Reading Data from Tables

To retrieve data from the table, you need to load the schema first, then read individual cells.

### Function Signatures

```cpp
schema_t get_schema_from_schema(string schema_file_name);
cell_data_t get_table_cell_data(int row, int col, schema_t schema_of_schema);
```

### Reading a Schema

**Function**: `get_schema_from_schema(string schema_file_name)`

This function reads the schema information from the binary schema file on disk and reconstructs a full `schema_t` object in memory.

**Input**: The filename of the schema file (usually `{table_name}__schema_data.bin`)

**Return**: A `schema_t` object containing all the table metadata

**Return Notes**: If the file doesn't exist or is corrupted, an empty schema with default values is returned.

**Example**:
```cpp
#include "engine/TableRead.h"

schema_t students_schema = get_schema_from_schema("Students__schema_data.bin");

// Now you can check the schema
cout << "Table has " << students_schema.num_cols << " columns" << endl;
cout << "Table has " << students_schema.num_rows << " rows" << endl;
```

### Reading a Cell

**Function**: `get_table_cell_data(int row, int col, schema_t schema_of_schema)`

This function reads a single cell from the table.

**Input**:
- **row**: 0-based row index
- **col**: 0-based column index
- **schema_of_schema**: The schema for the table

**Return**: A `cell_data_t` struct containing:
- **cell_data_type**: The type of the data
- **cell_data**: A shared_ptr to the actual data

**Return Values on Error**:
- If the row or column doesn't exist, returns a `cell_data_t` with `cell_data_type = NULL_TYPE` and `cell_data = nullptr`

### Extracting Data from a Cell

After reading a cell, you need to cast the shared_ptr to the correct type. Here's how to do it:

```cpp
cell_data_t cell = get_table_cell_data(0, 1, schema);

// Check the type first
if (cell.cell_data_type == NULL_TYPE) {
    // Data not found
    return;
}

// Use switch statement to cast to the correct type
switch (cell.cell_data_type) {
    case INT:
        int val = *static_cast<int*>(cell.cell_data.get());
        break;
    
    case UNSIGNED_INT:
        unsigned int val = *static_cast<unsigned int*>(cell.cell_data.get());
        break;
    
    case STRING:
        // For strings, the result is a null-terminated char array
        string val = string(static_cast<char*>(cell.cell_data.get()));
        break;
    
    case BOOLEAN:
        bool val = *static_cast<bool*>(cell.cell_data.get());
        break;
    
    case FLOATING_POINT:
        float val = *static_cast<float*>(cell.cell_data.get());
        break;
    
    case DOUBLE_FLOATING_POINT:
        double val = *static_cast<double*>(cell.cell_data.get());
        break;
    
    case UNSIGNED_CHAR:
        unsigned char val = *static_cast<unsigned char*>(cell.cell_data.get());
        break;
    
    default:
        // Unknown type
        break;
}
```

### Example: Reading an Entire Row

```cpp
#include "engine/TableRead.h"

schema_t schema = get_schema_from_schema("Students__schema_data.bin");

// Read row 0
int row_index = 0;

// Read each column in the row
for (int col = 0; col < schema.num_cols; col++) {
    cell_data_t cell = get_table_cell_data(row_index, col, schema);
    
    if (cell.cell_data_type == NULL_TYPE) {
        cout << "Column " << col << ": NULL" << endl;
        continue;
    }
    
    // You would need to add a switch statement here to handle each type
    // as shown above
}
```

---

## Modifying Data

To change the value of a cell in an existing row, use the modify function.

### Function Signature

```cpp
int cell_data_modify(schema_t schema_of_schema, cell_data_t after, int row, int col);
```

### What It Does

This function overwrites the value of a single cell with new data. The row and column must already exist.

### Input Parameters

- **schema_of_schema**: The schema for the table
- **after**: A `cell_data_t` struct containing the new data
- **row**: 0-based row index
- **col**: 0-based column index

The `after` parameter must contain:
- **cell_data_type**: The type of the new data (must match the original column type)
- **cell_data**: A shared_ptr to the new data value

### Return Values

- **0**: Success. Cell modified.
- **-1**: Failure. Usually because the new string data exceeds `max_str_len` for that column.
- **-2**: Failure. The data type of the new value doesn't match the column's original type.

### Type Mismatch Enforcement

The engine will reject any attempt to modify a cell with data of a different type. If your schema says a column is INT, you cannot modify it with STRING data.

### Example: Updating a Student's Name

```cpp
#include "engine/TableModify.h"

schema_t schema = get_schema_from_schema("Students__schema_data.bin");

// Change the name in row 0, column 2
cell_data_t new_name = {
    .cell_data_type = STRING,
    .cell_data = make_shared<string>("Jane Doe")
};

int result = cell_data_modify(schema, new_name, 0, 2);

if (result == 0) {
    // Successfully modified
} else if (result == -1) {
    // String too long for that column
} else if (result == -2) {
    // Type mismatch
}
```

### Important Notes

- You cannot change the type of data in a column. If the column was defined as STRING, you can only store STRING values in it.
- String data will be truncated if it exceeds the column's `max_str_len`.
- The modification happens immediately on disk.
- You do not need to pass the old data; the engine will read it, check that the new type matches, then overwrite it.

---

## Deleting Rows

To remove a row from the table, use the delete function.

### Function Signature

```cpp
int delete_row_from_table(int row, schema_t &table_schema);
```

### What It Does

This function removes a row from the table. The engine does this by:
1. Reading all rows except the one being deleted
2. Writing them to a temporary file
3. Replacing the original table data file with the temporary file
4. Automatically updating the schema's row count

Rows after the deleted row are shifted down (their row indices decrease by 1).

### Input Parameters

- **row**: 0-based index of the row to delete
- **table_schema**: A reference to the schema for the table (passed by reference, so it will be modified)

### Return Values

- **0**: Success. Row deleted and schema updated.
- **-1**: Failure. Usually because the row index is out of bounds or a file I/O error occurred.

### Row Index Shifting

When you delete a row, the indices of all subsequent rows shift down. For example:
- If you have rows 0, 1, 2, 3 and delete row 1
- The rows are now at indices 0, 1, 2 (so what was row 2 is now row 1)

### Schema Update

Since the function takes the schema by reference, it automatically updates `schema.num_rows` when a row is deleted. You don't need to reload the schema from disk after deletion.

### Example: Deleting a Student Record

```cpp
#include "engine/TableDelete.h"

schema_t schema = get_schema_from_schema("Students__schema_data.bin");

int row_to_delete = 1;

int result = delete_row_from_table(row_to_delete, schema);

if (result == 0) {
    // Row deleted successfully
    // schema.num_rows is automatically decremented
    cout << "Rows remaining: " << schema.num_rows << endl;
} else {
    // Error deleting row (row might not exist)
}
```

### Important Notes

- Deleting a row is expensive for large tables because the entire table must be rewritten.
- The `table_schema` parameter is passed by reference and will be modified. Its `num_rows` field will be decremented.
- Be careful with row indices after deletion; they shift down immediately.
- Unlike previous versions, you do not need to reload the schema from disk after deletion. The row count is updated in the schema object you passed.

---

## Complete Workflow Example

Here's a complete example that demonstrates creating a table, inserting data, reading it, modifying it, and deleting a row.

```cpp
#include "engine/TableCreate.h"
#include "engine/TableWrite.h"
#include "engine/TableRead.h"
#include "engine/TableModify.h"
#include "engine/TableDelete.h"
#include <iostream>

using namespace std;

int main() {
    // STEP 1: Define columns
    col_item_t id_col = {
        .is_primary_key = true,
        .col_id = 0,
        .data_type = UNSIGNED_INT,
        .col_name = "ID",
        .is_string = false,
        .max_str_len = 0,
    };

    col_item_t email_col = {
        .is_primary_key = false,
        .col_id = 1,
        .data_type = STRING,
        .col_name = "Email",
        .is_string = true,
        .max_str_len = 100,
    };

    // STEP 2: Create schema
    vector<col_item_t> columns = {id_col, email_col};
    schema_t schema;
    schema.table_name = "Users";
    schema.num_cols = columns.size();
    schema.num_rows = 0;
    schema.column_data = columns;

    // STEP 3: Create table
    if (create_new_table_schema(schema) != 0) {
        cout << "Failed to create table" << endl;
        return 1;
    }
    cout << "Table created" << endl;

    // STEP 4: Insert rows
    vector<row_data_t> row1 = {
        {0, make_shared<unsigned int>(1)},
        {1, make_shared<string>("alice@example.com")},
    };
    append_record_to_table(schema, row1);

    vector<row_data_t> row2 = {
        {0, make_shared<unsigned int>(2)},
        {1, make_shared<string>("bob@example.com")},
    };
    append_record_to_table(schema, row2);

    cout << "Rows inserted" << endl;

    // STEP 5: Read schema from disk
    schema = get_schema_from_schema("Users__schema_data.bin");
    cout << "Table has " << schema.num_rows << " rows" << endl;

    // STEP 6: Read a cell
    cell_data_t email_cell = get_table_cell_data(0, 1, schema);
    if (email_cell.cell_data_type != NULL_TYPE) {
        string email = string(static_cast<char*>(email_cell.cell_data.get()));
        cout << "First row email: " << email << endl;
    }

    // STEP 7: Modify a cell
    cell_data_t new_email = {
        .cell_data_type = STRING,
        .cell_data = make_shared<string>("alice.new@example.com")
    };
    if (cell_data_modify(schema, new_email, 0, 1) == 0) {
        cout << "Email updated" << endl;
    }

    // STEP 8: Delete a row
    if (delete_row_from_table(1, schema) == 0) {
        cout << "Row deleted" << endl;
    }

    return 0;
}
```

---

## Common Pitfalls and How to Avoid Them

### 1. String Length Mismatch

If you define a string column with `max_str_len = 10` and try to insert a 20-character string, it will be silently truncated. Always ensure your interpreter validates string lengths before insertion.

### 2. Accessing Non-Existent Rows

If you try to read or modify a row index that doesn't exist, the engine won't throw an error in all cases. Always check that your row index is less than `schema.num_rows`:

```cpp
if (row_index >= schema.num_rows) {
    // Error: row doesn't exist
}
```

### 3. Type Mismatches on Read

Always check `cell.cell_data_type` before casting. Don't assume a cell contains the type you expect:

```cpp
cell_data_t cell = get_table_cell_data(0, 1, schema);
if (cell.cell_data_type != schema.column_data[1].data_type) {
    // Type mismatch - handle error
}
```

### 4. Forgetting Column Order

When inserting rows, the order matters. The `row_data_t` vector must have entries for each column in the same order as the schema definitions:

```cpp
// Right: columns are in order 0, 1, 2
vector<row_data_t> correct = {
    {0, data0},
    {1, data1},
    {2, data2},
};

// Wrong: columns are out of order
vector<row_data_t> wrong = {
    {1, data1},
    {0, data0},
    {2, data2},
};
```

---

## Memory Management

The engine uses `shared_ptr` for data management. When you create row data with `make_shared<T>()`, the shared pointer automatically handles cleanup. You don't need to manually delete data.

Similarly, when you read data using `get_table_cell_data()`, the returned cell's `cell_data` is a shared_ptr that will be cleaned up automatically when the `cell_data_t` object goes out of scope.

---

## File Naming Convention

Always remember that the engine creates files with specific naming patterns:

- Schema file: `{table_name}__schema_data.bin`
- Table data file: `{table_name}__table_data.bin`
- Temporary files during deletion: `{table_name}_temp_table_data.bin`

When calling `get_schema_from_schema()`, use the full filename including `__schema_data.bin`.

---

## Summary of Core Functions

| Function | Purpose | Key Points |
|----------|---------|-----------|
| `create_new_table_schema()` | Create a new table | Returns 0 on success, -1 on failure |
| `append_record_to_table()` | Insert a row | Updates schema.num_rows, returns 0 on success |
| `get_schema_from_schema()` | Load schema from disk | Returns schema object, check num_rows after |
| `get_table_cell_data()` | Read a single cell | Returns cell_data_t, check cell_data_type |
| `cell_data_modify()` | Update a cell | Enforces type matching, returns 0 on success |
| `delete_row_from_table()` | Remove a row | Automatically decrements schema.num_rows, returns 0 on success |