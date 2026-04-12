# File-Based Database Management System

A lightweight, file-based database management system implemented in C++ that provides an intuitive SQL-like interface for data management without requiring external database servers.

## Overview

This project implements a complete database management system with a command-line interface, allowing users to create tables, insert records, query data, and perform updates and deletions through a simple SQL-like query language. The database engine stores all data in binary files on disk, making it portable and easy to use for educational purposes or lightweight applications.

## Features

- Create and manage multiple tables with flexible schema definition
- Insert, query, update, and delete records efficiently
- Support for multiple data types including integers, strings, floats, and booleans
- Primary key constraints on table columns
- Simple SQL-like command interface
- File-based persistence with no external dependencies

## Team Members

- Harish Babu
- Abhishek
- Rahul
- Ashhar Ansari

## Getting Started

### Prerequisites

- GCC compiler (g++) with C++17 support
- GNU Make
- Linux or Unix-like environment

### Building from Source

To compile the project, run:

```bash
make compile
```

This will create an executable file named `ics_dbms` in the `bin/` directory.

### Running the Application

To start the database management system:

```bash
./bin/ics_dbms
```

This will launch an interactive command-line interface where you can begin executing database commands.

## Documentation

For detailed information about using the database system, refer to the following documentation files:

- [DOCUMENTATION.md](DOCUMENTATION.md) - Complete user guide with command syntax, data types, and examples
- [engine documentation.md](engine%20documentation.md) - Technical documentation of the database engine internals

## Build Commands

- `make compile` - Compile the source code
- `make run` - run the application executable
- `make clean` - Remove compiled binaries
- `make rebuild` - Clean and recompile