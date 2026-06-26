# Simplified SQL Interpreter
This project, written for a databse class, is a C++ interpreter for a simplified version of SQL supporting only a subset of available commands. Table creation, deletion, insertion, selection, and aleration are supported.

<img width="544" height="483" alt="SQL Interpreter Demo Cropped" src="https://github.com/user-attachments/assets/c7c9249d-0d97-4b82-ace1-0c0c45eef564" />

## Usage
Execute `./driver` with no arguments to enter an interactive loop where you may enter commands one at a time.

Execute `./driver <filename>` with a file containing SQL commands, such as the included files, to have the interpreter execute the commands within.

## Persistance
Databases are stored as folders in the root directory of the program with the same name as that of the database. Tables are stored as files within their respective database folder, again with the same name as that of the table. Table attributes are stored on the first line, separated by commas. Records are stored in these same files on the following lines.
When a transaction is started, a `.lock` is created to prevent other instances of the program from attempting to modify the same table.

## Compiling
While an already compiled executable is includes, should you wish to compile the code yourself, a make file setup to use g++ is included. I used the exact command `g++ -std=c++17 driver.cpp -lstdc++fs -o driver`.

## Available Commands
The following commands are valid commands for the interpreter.

`CREATE DATABASE <database_name>`               
(Creates a folder representing the database)

`CREATE TABLE <table_name>`                     
(Creates a file representing the table)

`DROP DATABASE <database_name>`                 
(Deletes the folder representing the database)

`DROP TABLE <table_name>`                       
(Deltes the file representing the table)

`USE <database_name>`                           
(Sets the specified database as the active database for all commands)

`SELECT ...`                                    
(Displays records that matches the expression. FROM, WHERE, and JOIN are supported)

`ALTER TABLE <table_name> ADD ...`             
(Adds a column to the table)

`INSERT <table_name> INTO ...`                 
(Inserts a record into the table)

`UPDATE <table_name> set ..`                    
(Updates records in a table that match the expression)

`DELETE FROM <table_name> WHERE ...`            
(Deletes records in a table that match the expression)

`BEGIN TRANSACTION`                             
(Creates a lock on the table until the transaction is commited)

`COMMIT`                                        
(Ends the current transaction and release the lock on the table)

`.EXIT`                                         
(Halts the interpreter)
