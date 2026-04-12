# Database User Documentation

This guide provides comprehensive instructions for using the File-Based Database Management System. It covers all available commands, data types, and includes practical examples to help you effectively manage your data.

## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Supported Data Types](#supported-data-types)
4. [Commands](#commands)
5. [Examples](#examples)

---

## Introduction

The File-Based Database Management System is a simple yet powerful database interface that allows you to manage structured data through an SQL-like command language. The system stores your data in binary files on disk, eliminating the need for complex database server setup.

## Getting Started

### Starting the Application

Run the database management system with:

```bash
./bin/ics_dbms
```

Once started, you'll see the command prompt. Type your commands and press Enter to execute them.

### Basic Command Structure

All commands must end with a semicolon (`;`). The system is case-insensitive for keywords but is case-sensitive for table and column names.

### Available Command Overview

- `CREATE TABLE` - Define and create a new table
- `INSERT INTO` - Add records to a table
- `SELECT` - Query and retrieve data
- `UPDATE` - Modify existing records
- `DELETE` - Remove records from a table
- `DISPLAY TABLES` - List all existing tables
- `DISPLAY SCHEMA` - Show the schema of a specific table
- `help` - Show available commands
- `exit` - Close the application

---

## Supported Data Types

When creating tables, you can use the following data types for your columns:

| Data Type | Description | Size | Syntax in CREATE TABLE |
|-----------|-------------|------|---------|
| `int` | Signed 32-bit integer | 4 bytes | `column_name int` |
| `unsigned_int` | Unsigned 32-bit integer | 4 bytes | `column_name unsigned_int` |
| `string` | Text string with maximum length | variable | `column_name string(length)` |
| `float` | Single-precision floating point | 4 bytes | `column_name float` |
| `double` | Double-precision floating point | 8 bytes | `column_name double` |
| `bool` | Boolean true or false | 1 byte | `column_name bool` |
| `unsigned_char` | Single unsigned byte | 1 byte | `column_name unsigned_char` |

### Notes on Data Types

- **Data types must be lowercase**: `int`, `string`, `float`, etc.
- **primary key keyword is lowercase**: Always write as `primary key`
- **Other keywords are case-insensitive**: `create`, `Create`, `CREATE` all work
- **String columns** require a maximum length specified in parentheses: `name string(50)` for a 50-character field
- **Numeric types** are fixed-size
- String values in queries must be enclosed in single quotes: `'John Doe'`
- Boolean values should be written as `true` or `false` (lowercase)

---

## File Storage and Management

### How Tables are Stored

When you create a table, the database engine creates two binary files in the current working directory from which you ran `./bin/ics_dbms`:

- `table_name__schema_data.bin` - Contains the table schema and column information
- `table_name__table_data.bin` - Contains the actual row data

### Current Working Directory

All database files are created in and read from the **current working directory** where you execute the `./bin/ics_dbms` command. If you run the command from different directories, the database files will be created in those respective directories.

Example:
```bash
cd /path/to/project && ./bin/ics_dbms   # Files created in /path/to/project
cd /home/user && /path/to/project/bin/ics_dbms  # Files created in /home/user
```

### DISPLAY TABLES Behavior

The `DISPLAY TABLES` command works by scanning the current working directory for files matching the schema file pattern (`*__schema_data.bin`). It displays the names of all tables found based on matching files.

### Important Warning: File Integrity

**The engine does not validate whether database files are actually valid or not.** It only checks if a filename matches the expected pattern for a schema file (`table_name__schema_data.bin`). This means:

- If database files become corrupted, the engine will attempt to read them without validation
- If you manually create or rename files to match the schema file pattern, the engine will try to read them
- Both scenarios can cause unexpected behavior or errors that are not handled by the engine or interpreter
- Data loss or application crashes may occur if files are damaged or improperly formatted

**Best Practices:**
- Do not manually modify or rename database files
- Do not copy database files between different directory structures without understanding the system
- Keep backups of important database files
- Do not create files manually matching the `*__schema_data.bin` pattern

---

## Commands

### 1. CREATE TABLE

Create a new table with specified columns and their data types.

#### Syntax

```
create table table_name (column_name type [primary key], column_name type(length) [primary key], ...);
```

#### Parameters

- `table_name` - Unique name for the table (case-insensitive)
- `column_name` - Name of the column (case-insensitive)
- `type` - One of the supported data types listed above (must be lowercase)
- For `string`: Use `string(max_length)` to specify the maximum number of characters
- `primary key` (optional and lowercase) - Marks a column as a primary key (comes after the data type)

#### Important Notes

- **Data types must be lowercase**: `int`, `string`, `float`, etc.
- **primary key is lowercase**: Always write exactly as `primary key`
- For **string columns**: Syntax is `column_name string(length)` where length is a number
- For **non-string columns**: `column_name type [primary key]`
- Other keywords (`create`, `table`) are case-insensitive
- Semicolon (`;`) is required at the end
- Schema and data files are created in the current working directory
- **Primary Key Requirement**: Exactly one user-defined column must be designated as the primary key using the `primary key` keyword. This is compulsory for all tables.

#### Examples

```sql
create table employees (emp_id int primary key, name string(100), salary float);

create table users (user_id int primary key, email string(50), active bool);

create table products (product_code unsigned_int primary key, productname string(100), price double);
```

### 2. INSERT INTO

Add one or more records to an existing table.

#### Syntax

```
insert into table_name values (value1, value2, ...), (value1, value2, ...), ...;
```

#### Parameters

- `table_name` - Name of the table to insert into
- `value` - Data value for each column in the order they were defined in CREATE TABLE
- Use `NULL` for NULL values
- Enclose string values in single quotes: `'value'`
- Separate multiple records with commas and parentheses
- Values must match the column data types and order

#### Examples

```sql
insert into employees values (1,'John Smith', 50000.50);

insert into users values (103, 'john@example.com', true), (104, 'jane@example.com', true);

insert into products values (1001, 'Laptop', 999.99, 15), (1002, 'Mouse', 25.50, 100);
```

### 3. SELECT

Query and retrieve data from a table.

#### Syntax

```
select * from table_name;

select column_name1, column_name2, ... from table_name;
```

#### Parameters

- `*` - Select all columns, or specify column names separated by commas
- `column_name` - Specific columns to retrieve
- `table_name` - Table to query from

#### Examples

```sql
select * from employees;

select name, salary from employees;
```

### 4. UPDATE

Modify values in existing records.

#### Syntax

```
update table_name set column_name = new_value where primary_key_column = value;
```

#### Parameters

- `table_name` - Table containing records to update
- `column_name` - Column to update (after SET)
- `new_value` - New value for the column
- `primary_key_column` - The primary key column of the table
- `value` - The value of the primary key to identify which record to update
- String values should be enclosed in single quotes: `'value'`

#### Examples

```sql
update employees set salary = 55000 where emp_id = 1;

update users set active = false where user_id = 101;

update products set price = 29.99 where product_code = 1001;
```

### 5. DELETE

Remove records from a table.

#### Syntax

```
delete (row_range) from table_name;
```

#### Parameters

- `row_range` - Specifies which rows to delete using row numbers (0-indexed):
  - Single row: `0` (deletes row 0)
  - Range of rows: `0-3` (deletes rows 0, 1, 2, 3)
  - Multiple specific rows: `0,2,4` (deletes rows 0, 2, and 4)
- `table_name` - Table to delete from

#### Examples

```sql
delete (0) from employees;

delete (0-2) from users;

delete (1,3,5) from products;
```

### 6. DISPLAY TABLES

List all tables that exist in the database.

#### Syntax

```
display tables;
```

#### How It Works

This command scans the current working directory for all schema files (`*__schema_data.bin`). It displays the name of each table found by matching filenames to the expected schema file format.

#### Example

```sql
display tables;
```

This will show all available tables with their basic properties.

#### Important Notes

- Only tables with valid schema files in the current directory are displayed
- The command searches the current working directory where `./bin/ics_dbms` was executed
- If tables are in a different directory, you need to run the application from that directory
- If schema files are manually deleted or moved, the corresponding tables will not appear in the list

### 7. DISPLAY SCHEMA

Show the schema of a specific table, including column names, data types, and primary key information.

#### Syntax

```
display schema table_name;
```

#### Parameters

- `table_name` - Name of the table whose schema you want to view (case-sensitive)

#### How It Works

This command reads the schema file (`table_name__schema_data.bin`) for the specified table and displays detailed information about:
- Column names and their order
- Data types for each column
- Which column is designated as the primary key
- Maximum length for string columns

#### Example

```sql
display schema employees;
```

This will show the complete schema structure of the employees table, including all columns, their data types, and primary key designation.

#### Important Notes

- The table must exist for the command to work
- If the table does not exist, an error message will be displayed
- The command is case-sensitive for table names
- This is useful for understanding table structure before performing operations

### 8. Additional Commands

#### Help

Display all available commands:

```
help
```

#### Exit

Close the application:

```
exit
```

---

## Examples

### Example 1: Building a Student Database

```sql
create table students (roll_id int primary key, name string(50), email string(100), gpa float);

insert into students values (1001, 'Alice Johnson', 'alice@university.edu', 3.85), (1002, 'Bob Smith', 'bob@university.edu', 3.72), (1003, 'Carol White', 'carol@university.edu', 3.91);

select * from students;

update students set gpa = 3.90 where roll_id = 1002;

select name, gpa from students;

delete (2) from students;

display schema students;
```

### Example 2: Managing an Inventory System

```sql
create table inventory (item_id unsigned_int primary key, item_name string(100), quantity int, unit_price float, in_stock bool);

insert into inventory values (1001, 'Widget A', 150, 9.99, true), (1002, 'Widget B', 75, 14.50, true), (1003, 'Gadget X', 0, 29.99, false);

select item_name, quantity from inventory;

update inventory set quantity = 200 where item_id = 1001;

select * from inventory;

delete (2) from inventory;
```

### Example 3: Working with Different Data Types

```sql
create table data_types_demo (user_id int primary key, count unsigned_int, value float, precise_value double, is_active bool, code unsigned_char, description string(255));

insert into data_types_demo values (101, 100, 3.14, 3.141592653589793, true, 65, 'Sample record with all data types');

select * from data_types_demo;

update data_types_demo set description = 'Updated record' where user_id = 101;
```

---

## Tips and Best Practices

1. **Table Naming** - Use descriptive, lowercase names for your tables (e.g., `customers`, `orders`, `products`)

2. **Column Naming** - Choose clear column names that describe the data they contain

3. **String Lengths** - Set appropriate maximum lengths for string columns. Longer strings consume more disk space per row, so avoid excessive limits

4. **Primary Keys** - Every table must have exactly one primary key column. Choose a column that uniquely identifies each record (e.g., ID numbers, codes, etc.)

5. **Backup Data** - The system stores data in binary files. Keep backups of important datasets

6. **NULL Values** - Use NULL for missing or unknown data in optional fields

7. **Query Syntax** - Always end commands with a semicolon

8. **Data Types** - Ensure inserted values match the column data type definitions

---

## Troubleshooting

### Common Issues

**"Cannot insert data: table does not exist!"**
- Make sure the table was created successfully before inserting data
- Check spelling of the table name (case-sensitive)

**"Error inserting row"**
- Ensure the number of values matches the number of columns
- Check that values are in the correct order
- Verify data types match column definitions
- Make sure string values are enclosed in single quotes

**"Error: Invalid Command!"**
- Check command syntax carefully
- Ensure command ends with semicolon (`;`)
- Verify table and column names are spelled correctly
- Make sure all data types are in lowercase

**"Cannot read table: table does not exist!"**
- Use `display tables;` to see available tables
- Check the table name spelling and case

### Getting Help

Type `help` in the command prompt to see all available commands at any time.

---

## Additional Resources

For information about the database engine architecture and internal implementation, refer to [engine documentation.md](engine%20documentation.md).