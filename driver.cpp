// Luna Perez-Herrera
// CS 457 Project 4
// v1.4

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <experimental/filesystem>
#include <algorithm>
#include <cctype>
#include <locale>

using namespace std;
namespace filesystem = std::experimental::filesystem;
string currentDatabase = "";
bool transactionActive = false;
vector<vector<string>> currentTransactionData;
string currentTransactionTableFields = "";
string currentTransactionTableName = "";
bool changesMade = false;

// Function prototypes
void createDatabase(string databaseName);
void createTable(vector<string> inputLineSplit);
void deleteDatabase(string databaseName);
void deleteTable(string tableName);
void selectData(vector<string> inputLineSplit);
void alterTable(vector<string> inputLineSplit);
void insertData(vector<string> inputLineSplit);
void updateTable(vector<string> inputLineSplit);
void deleteData(vector<string> inputLineSplit);
void inputModeLoop(string inputFile);
void interactiveModeLoop();
void processCommands(vector<string> inputLineSplit);
string reconstructFields(vector<string> inputLineSplit, int startIndex, int endIndex);
vector<string> getTableData(string tableName);
vector<string> splitTableFirstField(string firstLine);
vector<string> splitTableData(string tableData);
vector<vector<string>> reconstructTableData(string tableName);
void writeTableData(string tableName, vector<vector<string>> tableData, string originalTableFields);
int getTableFieldIndex(vector<string> fields, string fieldName);
void joinSelectData(vector<string> inputLineSplit);
void innerJoinSelectData(vector<string> tableNames, vector<string> tableAlias, vector<string> joinConditions);
void outerJoinSelectData(vector<string> tableNames, vector<string> tableAlias, vector<string> joinConditions);
void beginTransaction();
void commitTransaction();
void writeTransactionData();

// Helper function prototypes
bool checkIfUsingDatabase();
vector<string> splitStringByWhiteSpace(string str);
vector<string> capitalizeStringVector(vector<string> strVector);
string capitalizeString(string str);
vector<string> removeTrailingSemicolon(vector<string> strVector);
string removeTrailingAndLeadingInvisibleCharacters(string str);
string removeTrailingInvisibleCharacters(string str);
string removeLeadingInvisibleCharacters(string str);
string removeTrailingAndLeadingParenthesis(string str);
string removeAllQuotes(string str);
string removeAllCommas(string str);
vector<string> splitStringByDelimiter(string str, char delimiter);

// Main driver
int main(int argc, char *argv[])
{
    bool interactiveMode = false;

    // Validate command line arguments
    if(argc != 2 && argc != 1)
    {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    // Enable interactive mode if no arguments are passed
    if(argc == 1)
    {
        cout << "No arguments detected, entering interactive mode" << endl;
        cout << "Type '.EXIT' to quit" << endl;
        interactiveMode = true;

    }

    // Determine if interactive mode or file input mode
    if(interactiveMode)
    {
        interactiveModeLoop();
        return 1;
    }
    else
    {
        inputModeLoop(argv[1]);
        return 1;
    }
}

// Handles file input mode. Loops until end of file
void inputModeLoop(string inputFile)
{
    // First check for it file exists
    if(!filesystem::exists(inputFile))
    {
        cout << "File " << inputFile << " does not exist" << endl;
        exit(1);
    }

    // Open specified input file
    ifstream input(inputFile);
    // Second check for if file exists
    if(!input)
    {
        cout << "File " << inputFile << " does not exist" << endl;
        exit(1);
    }

    // Read input file line by line
    string currentLine;
    vector<string> inputLineSplit;
    while(getline(input, currentLine))
    {
        // Remove trailing new line
        currentLine = removeTrailingInvisibleCharacters(currentLine);

        // Skips empty lines and comments
        if (currentLine.empty() || currentLine.substr(0, 2) == "--" || currentLine.length() < 2) {
            continue;
        }

        // Check for a semicolon at the end of the line
        if(currentLine.back() != ';' && currentLine != ".EXIT" && currentLine != ".exit")
        {
            cout << "Error: Missing semicolon at end of line." << endl;
            continue;
        }

        // Preform input processing
        inputLineSplit = splitStringByWhiteSpace(currentLine); // Splits line by whitespace
        for(int i = 0; i < inputLineSplit.size(); i++)
        {
            inputLineSplit[i] = removeTrailingInvisibleCharacters(inputLineSplit[i]); // Removes trailing invisible characters
        }
        inputLineSplit = removeTrailingSemicolon(inputLineSplit); // Removes trailing semicolon
        inputLineSplit = capitalizeStringVector(inputLineSplit); // Capitalizes all strings in vector

        // Check for commands
        processCommands(inputLineSplit);
    }
}

// Handles interactive mode input. Loops until user enters ".EXIT"
void interactiveModeLoop()
{
    // Initialize input variables
    string inputLine;
    vector<string> inputLineSplit;

    // Main loop
    while(true)
    {
        // Get user input
        cout << "sqlite > ";
        getline(cin, inputLine);
        inputLine = removeTrailingAndLeadingInvisibleCharacters(inputLine);

        if(inputLine.back() != ';' && inputLine != ".EXIT")
        {
            cout << "Error: Missing semicolon at end of line. Discarding input" << endl;
            continue;
        }

        // Preform input processing
        inputLineSplit = splitStringByWhiteSpace(inputLine);
        inputLineSplit = removeTrailingSemicolon(inputLineSplit);
        inputLineSplit = capitalizeStringVector(inputLineSplit);

        // Check for commands
        processCommands(inputLineSplit);
    }
}

// Matches the input command to the correct function
void processCommands(vector<string> inputLineSplit)
{
    // Check for commands
    if(inputLineSplit[0] == "CREATE" && inputLineSplit[1] == "DATABASE")
    {
        createDatabase(inputLineSplit[2]);
    }
    else if(inputLineSplit[0] == "CREATE" && inputLineSplit[1] == "TABLE")
    {
        createTable(inputLineSplit);
    }
    else if(inputLineSplit[0] == "DROP" && inputLineSplit[1] == "DATABASE")
    {
        deleteDatabase(inputLineSplit[2]);
    }
    else if(inputLineSplit[0] == "DROP" && inputLineSplit[1] == "TABLE")
    {
        deleteTable(inputLineSplit[2]);
    }
    else if(inputLineSplit[0] == "USE")
    {
        cout << "Using database " << inputLineSplit[1] << "." << endl;
        currentDatabase = inputLineSplit[1];
        if(!checkIfUsingDatabase())
        {
            currentDatabase = "";
        }
    }
    else if(inputLineSplit[0] == "SELECT")
    {
        selectData(inputLineSplit);
    }
    else if(inputLineSplit[0] == "ALTER")
    {
        alterTable(inputLineSplit);
    }
    else if(inputLineSplit[0] == "INSERT")
    {
        insertData(inputLineSplit);
    }
    else if(inputLineSplit[0] == "UPDATE")
    {
        updateTable(inputLineSplit);
    }
    else if(inputLineSplit[0] == "DELETE")
    {
        deleteData(inputLineSplit);
    }
    else if(inputLineSplit[0] == "BEGIN" && inputLineSplit[1] == "TRANSACTION")
    {
        beginTransaction();
    }
    else if(inputLineSplit[0] == "COMMIT")
    {
        commitTransaction();
    }
    else if(inputLineSplit[0] == ".EXIT")
    {
        cout << "All done." << endl;
        exit(1);
    }
    else
    {
        cout << "!Failed to execute command. Input did not match a known command." << endl;
    }
}

// Starts a transaction
void beginTransaction()
{
    if(transactionActive)
    {
        cout << "!Failed to begin transaction. Transaction already active." << endl;
    }
    else
    {
        transactionActive = true;
        cout << "Transaction starts." << endl;
    }
}

// Commits the transaction and writes the updated data to the file
void commitTransaction()
{
    if(!transactionActive)
    {
        cout << "!Failed to commit transaction. No active transaction." << endl;
    }
    else
    {
        transactionActive = false;

        if(changesMade)
        {
            writeTransactionData();
            changesMade = false;
            cout << "Transaction committed." << endl;
        }
        else
        {
            cout << "Transaction abort." << endl;
        }
    }
}

// Handles creating databases
void createDatabase(string databaseName)
{
    // Checks if database already exists
    if(filesystem::exists(databaseName))
    {
        cout << "!Failed to create database " << databaseName << " because it already exists." << endl;
    }
    else
    {
        // Creates database
        filesystem::create_directory(databaseName);
        cout << "Database " << databaseName << " created." << endl;
    }
}

// Handles creating tables
void createTable(vector<string> inputLineSplit)
{
    // Checks if database exists
    if(checkIfUsingDatabase())
    {
        // Checks if table already exists
        if(filesystem::exists(currentDatabase + "/" + inputLineSplit[2]))
        {
            cout << "!Failed to create table " << inputLineSplit[2] << " because it already exists." << endl;
        }
        else
        {
            // Create table file and write fields to first line of file
            ofstream tableFile(currentDatabase + "/" + inputLineSplit[2]);
            string fields = reconstructFields(inputLineSplit, 3, inputLineSplit.size());
            tableFile << fields;
            cout << "Table " << inputLineSplit[2] << " created." << endl;
            tableFile.close();
        }
    }
}

// Handles deleting databases
void deleteDatabase(string databaseName)
{
    // Checks if database exists
    if(filesystem::exists(databaseName))
    {
        // Deletes database
        filesystem::remove_all(databaseName);
        cout << "Database " << databaseName << " deleted." << endl;
    }
    else
    {
        cout << "!Failed to delete database " << databaseName << " because it does not exist." << endl;
    }
}

// Handles deleting tables
void deleteTable(string tableName)
{
    // Checks if database exists
    if(checkIfUsingDatabase())
    {
        // Checks if table exists
        if(filesystem::exists(currentDatabase + "/" + tableName))
        {
            // Deletes table
            filesystem::remove(currentDatabase + "/" + tableName);
            cout << "Table " << tableName << " deleted." << endl;
        }
        else
        {
            cout << "!Failed to delete table " << tableName << " because it does not exist." << endl;
        }
    }
}

// Handles select commands. Join are identified and passed on to joinSelectData()
void selectData(vector<string> inputLineSplit)
{
    // Displays table data
    ifstream infile(currentDatabase + "/" + inputLineSplit[3]);
    string line;
    
    if(inputLineSplit[1] == "*" && inputLineSplit.size() == 4)
    {
        // Displays all data
        while(getline(infile, line))
        {
            cout << line << endl;
        }
        
    }
    else if(inputLineSplit[1] == "*" && inputLineSplit.size() > 4)
    {
        joinSelectData(inputLineSplit);
    }
    else
    {
        // Display conditional data
        
        // Get field names
        vector<string> fields;
        int fromIndex = 0;
        for(int i = 1; i < inputLineSplit.size(); i++)
        {
            if(inputLineSplit[i] == "FROM")
            {
                fromIndex = i;
                break;
            }
            else if(inputLineSplit[i] != "SELECT" && inputLineSplit[i] != ",")
            {
                fields.push_back(removeAllCommas(inputLineSplit[i]));
            }
        }

        // Get table name
        string tableName = inputLineSplit[fromIndex + 1];

        // Get table data
        vector<vector<string>> tableData = reconstructTableData(tableName);

        // Copy first field line to temporay string
        ifstream infile(currentDatabase + "/" + tableName);
        string originalFieldLine;
        getline(infile, originalFieldLine);
        infile.close();

        // Get conditional field and value
        string searchField = inputLineSplit[fromIndex + 3];
        string searchValue = inputLineSplit[fromIndex + 5];
        string searchOperator = inputLineSplit[fromIndex + 4];

        // Get field index
        int searchFieldIndex = getTableFieldIndex(tableData[0], searchField);
        vector<int> fieldIndexes;
        for(int i = 0; i < fields.size(); i++)
        {
            fieldIndexes.push_back(getTableFieldIndex(tableData[0], fields[i]));
        }

        // Display first line of data
        cout << originalFieldLine << endl;

        // Display data according to operator
        if(searchOperator == "=")
        {
            for(int i = 1; i < tableData.size(); i++)
            {
                if(tableData[i][searchFieldIndex] == searchValue)
                {
                    for(int j = 0; j < fieldIndexes.size(); j++)
                    {
                        cout << tableData[i][fieldIndexes[j]];
                        if(j != fieldIndexes.size() - 1)
                        {
                            cout << ", ";
                        }
                    }
                    cout << endl;
                }
            }
        }
        else if(searchOperator == ">")
        {
            for(int i = 1; i < tableData.size(); i++)
            {
                if(tableData[i][searchFieldIndex] > searchValue)
                {
                    for(int j = 0; j < fieldIndexes.size(); j++)
                    {
                        cout << tableData[i][fieldIndexes[j]];
                        if(j != fieldIndexes.size() - 1)
                        {
                            cout << ", ";
                        }
                    }
                    cout << endl;
                }
            }
        }
        else if(searchOperator == "<")
        {
            for(int i = 1; i < tableData.size(); i++)
            {
                if(tableData[i][searchFieldIndex] < searchValue)
                {
                    for(int j = 0; j < fieldIndexes.size(); j++)
                    {
                        cout << tableData[i][fieldIndexes[j]];
                        if(j != fieldIndexes.size() - 1)
                        {
                            cout << ", ";
                        }
                    }
                    cout << endl;
                }
            }
        }
        else if(searchOperator == ">=")
        {
            for(int i = 1; i < tableData.size(); i++)
            {
                if(tableData[i][searchFieldIndex] >= searchValue)
                {
                    for(int j = 0; j < fieldIndexes.size(); j++)
                    {
                        cout << tableData[i][fieldIndexes[j]];
                        if(j != fieldIndexes.size() - 1)
                        {
                            cout << ", ";
                        }
                    }
                    cout << endl;
                }
            }
        }
        else if(searchOperator == "<=")
        {
            for(int i = 1; i < tableData.size(); i++)
            {
                if(tableData[i][searchFieldIndex] <= searchValue)
                {
                    for(int j = 0; j < fieldIndexes.size(); j++)
                    {
                        cout << tableData[i][fieldIndexes[j]];
                        if(j != fieldIndexes.size() - 1)
                        {
                            cout << ", ";
                        }
                    }
                    cout << endl;
                }
            }
        }
        else if(searchOperator == "!=")
        {
            for(int i = 1; i < tableData.size(); i++)
            {
                if(tableData[i][searchFieldIndex] != searchValue)
                {
                    for(int j = 0; j < fieldIndexes.size(); j++)
                    {
                        cout << tableData[i][fieldIndexes[j]];
                        if(j != fieldIndexes.size() - 1)
                        {
                            cout << ", ";
                        }
                    }
                    cout << endl;
                }
            }
        }
    }

    infile.close();
}

// Alters a table according to the input
void alterTable(vector<string> inputLineSplit)
{
    // Check if database exists
    if(checkIfUsingDatabase())
    {
        // Check if table exists
        if(filesystem::exists(currentDatabase + "/" + inputLineSplit[2]))
        {
            if(inputLineSplit[1] == "TABLE")
            {
                if(inputLineSplit[3] == "ADD")
                {
                    // Constructs a string with the fields to be added
                    string fields = reconstructFields(inputLineSplit, 4, inputLineSplit.size());
                    ofstream tableFile(currentDatabase + "/" + inputLineSplit[2], ios_base::app);

                    // Add fields to end of table file
                    tableFile << ", " << fields;
                    cout << "Table " << inputLineSplit[2] << " modified." << endl;
                    tableFile.close();
                }
                else
                {
                    cout << "!Failed to execute command. Input did not match a known command." << endl;
                }
            }
            else
            {
                cout << "!Failed to execute command. Input did not match a known command." << endl;
            }
        }
    }
}

// Handles inserting of data into a table
void insertData(vector<string> inputLineSplit)
{
    // Check if database exists
    if(checkIfUsingDatabase())
    {
        // Check if table exists
        if(filesystem::exists(currentDatabase + "/" + inputLineSplit[2]))
        {
            // Check if column exists
            if(inputLineSplit[1] == "INTO")
            {
                // Copy existing data to temporary vector
                vector<string> tableData;
                ifstream infile(currentDatabase + "/" + inputLineSplit[2]);
                string line;
                while(getline(infile, line))
                {
                    tableData.push_back(line);
                }
                infile.close();
                
                // Removes "VALUE" from inputLineSplit[3]"
                inputLineSplit[3].erase(0, 6);
                // Adds new data to temporary vector
                tableData.push_back(reconstructFields(inputLineSplit, 3, inputLineSplit.size()));
                // Remove any single and double quotes from temporary vector
                for(int i = 0; i < tableData.size(); i++)
                {
                    tableData[i].erase(remove(tableData[i].begin(), tableData[i].end(), '\''), tableData[i].end());
                    tableData[i].erase(remove(tableData[i].begin(), tableData[i].end(), '\"'), tableData[i].end());
                }

                // Write temporary vector back to table file
                ofstream tableFile(currentDatabase + "/" + inputLineSplit[2]);
                for(int i = 0; i < tableData.size(); i++)
                {
                    tableFile << tableData[i] << endl;
                }
                cout << "1 new record inserted." << endl;
            }
            else
            {
                cout << "!Failed to execute command. Input did not match a known command." << endl;
            }
        }
        else
        {
            cout << "!Failed to execute command. Input did not match a known command." << endl;
        }
    }
}

// Updates a table according to specified conditions and writes the changes to the table file
void updateTable(vector<string> inputLineSplit)
{
    // Check if database exists
    if(checkIfUsingDatabase())
    {
        // Check if table exists
        if(filesystem::exists(currentDatabase + "/" + inputLineSplit[1]))
        {
            if(transactionActive)
            {
                if(filesystem::exists(currentDatabase + "/" + inputLineSplit[1] + ".lock"))
                {
                    cout << "Error: Table " + inputLineSplit[1] + " is locked!" << endl;
                    return;
                }
                else
                {
                    ofstream lockFile(currentDatabase + "/" + inputLineSplit[1] + ".lock");
                }
            }

            if(inputLineSplit[2] == "SET")
            {
                // Copy first field line to temporay string
                ifstream infile(currentDatabase + "/" + inputLineSplit[1]);
                string originalFieldLine;
                getline(infile, originalFieldLine);
                infile.close();

                // Create temporary 2D vector
                vector<vector<string>> tableData = reconstructTableData(inputLineSplit[1]);

                // Identify column to be updated
                int columnSearchNumber = getTableFieldIndex(tableData[0], inputLineSplit[7]);
                int columnUpdateNumber = getTableFieldIndex(tableData[0], inputLineSplit[3]);
                string columnSearchCondition = removeAllQuotes(inputLineSplit[9]);
                string columnUpdateValue = removeAllQuotes(inputLineSplit[5]);
                string operator1 = inputLineSplit[8];
                string operator2 = inputLineSplit[4];

                // Update table data
                for(int i = 1; i < tableData.size(); i++)
                {
                    if(operator1 == "=")
                    {
                        if(operator2 == "=")
                        {
                            if(tableData[i][columnSearchNumber] == columnSearchCondition)
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                        else if(operator2 == ">")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) > stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                        else if(operator2 == "<")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) < stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                    }
                    else if(operator1 == ">")
                    {
                        if(operator2 == "=")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) >= stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                        else if(operator2 == ">")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) > stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                        else if(operator2 == "<")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) < stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                    }
                    else if(operator1 == "<")
                    {
                        if(operator2 == "=")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) <= stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                        else if(operator2 == ">")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) > stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                        else if(operator2 == "<")
                        {
                            if(stoi(tableData[i][columnSearchNumber]) < stoi(columnSearchCondition))
                            {
                                tableData[i][columnUpdateNumber] = columnUpdateValue;
                            }
                        }
                    }
                }

                if(!transactionActive)
                {
                    // Write temporary vector back to table file
                    writeTableData(inputLineSplit[1], tableData, originalFieldLine);
                    cout << "Table " << inputLineSplit[1] << " modified." << endl;
                }
                else
                {
                    // Save temporary vector to global variable for later commit
                    currentTransactionData = tableData;
                    currentTransactionTableFields = originalFieldLine;
                    currentTransactionTableName = inputLineSplit[1];
                    changesMade = true;
                    cout << "Table " << inputLineSplit[1] << " modified." << endl;
                }
            }
            else
            {
                cout << "!Failed to execute command." << endl;
            }
        }
        else
        {
            cout << "!Failed to execute command." << endl;
        }
    }
}

void writeTransactionData()
{
    // Delete .lock file
    filesystem::remove(currentDatabase + "/" + currentTransactionTableName + ".lock");

    // Write temporary vector back to table file
    writeTableData(currentTransactionTableName, currentTransactionData, currentTransactionTableFields);
}

// Deletes data from a table
void deleteData(vector<string> inputLineSplit)
{
    // Check if database exists
    if(checkIfUsingDatabase())
    {
        // Check if table exists
        if(filesystem::exists(currentDatabase + "/" + inputLineSplit[2]))
        {
            // Copy first field line to temporay string
            ifstream infile(currentDatabase + "/" + inputLineSplit[2]);
            string originalFieldLine;
            getline(infile, originalFieldLine);
            infile.close();

            // Create temporary 2D vector
            vector<vector<string>> tableData = reconstructTableData(inputLineSplit[2]);

            // Identify column to be updated
            int columnSearchNumber = getTableFieldIndex(tableData[0], inputLineSplit[4]);
            string columnSearchCondition = removeAllQuotes(inputLineSplit[6]);
            string operator1 = inputLineSplit[5];

            // Update table data
            for(int i = 1; i < tableData.size(); i++)
            {
                if(operator1 == "=")
                {
                    if(tableData[i][columnSearchNumber] == columnSearchCondition)
                    {
                        tableData.erase(tableData.begin() + i);
                    }
                }
                else if(operator1 == ">")
                {
                    if(stoi(tableData[i][columnSearchNumber]) > stoi(columnSearchCondition))
                    {
                        tableData.erase(tableData.begin() + i);
                    }
                }
                else if(operator1 == "<")
                {
                    if(stoi(tableData[i][columnSearchNumber]) < stoi(columnSearchCondition))
                    {
                        tableData.erase(tableData.begin() + i);
                    }
                }
            }

            // Write temporary vector back to table file
            writeTableData(inputLineSplit[2], tableData, originalFieldLine);
            cout << "Table " << inputLineSplit[2] << " modified." << endl;
        }
        else
        {
            cout << "!Failed to execute command." << endl;
        }
    }
}

// Takes the fields from a line of input and reconstructs them into a single string so they can be easily appended to a file
string reconstructFields(vector<string> inputLineSplit, int startIndex, int endIndex)
{
    string reconstructedString = "";
    for(int i = startIndex; i < endIndex-1; i++)
    {
        reconstructedString += inputLineSplit[i] + " ";
    }
    reconstructedString += inputLineSplit[endIndex-1];

    // Remove leading and trailing whitespace
    reconstructedString = removeTrailingAndLeadingInvisibleCharacters(reconstructedString);

    // Remove leading and trailing parentheses
    reconstructedString = removeTrailingAndLeadingParenthesis(reconstructedString);
    
    return reconstructedString;
}

// Returns a vector of strings for each line of data in a table file
vector<string> getTableData(string tableName)
{
    vector<string> tableData;
    ifstream infile(currentDatabase + "/" + tableName);
    string line;
    while(getline(infile, line))
    {
        tableData.push_back(line);
    }
    infile.close();
    return tableData;
}

// Intended to be used with the first line of a table file. Splits the line into a vector of strings and excludes data types so that only field names remain
// Excluding the data types make the output useful for matching to a condition
vector<string> splitTableFirstField(string firstLine)
{
    vector<string> initialTableFields = splitStringByWhiteSpace(removeAllCommas(firstLine));
    vector<string> tableFields;
    for(int i = 0; i < initialTableFields.size(); i++)
    {
        if(i % 2 == 0)
        {
            tableFields.push_back(initialTableFields[i]);
        }
    }

    return tableFields;
}

// Splits a given line from a table file into a vector of strings. Removes commas and quotes from data
vector<string> splitTableData(string tableData)
{
    vector<string> tableFields = splitStringByWhiteSpace(tableData);
    for(int i = 0; i < tableFields.size(); i++)
    {
        // Remove commas and quotes from data
        tableFields[i].erase(remove(tableFields[i].begin(), tableFields[i].end(), ','), tableFields[i].end());
        tableFields[i].erase(remove(tableFields[i].begin(), tableFields[i].end(), '"'), tableFields[i].end());
        tableFields[i].erase(remove(tableFields[i].begin(), tableFields[i].end(), '\''), tableFields[i].end());
    }
    return tableFields;
}

// Returns a 2D vector of the table data. The first row is the table fields with data types excluded so that they can be used for searching
// Constructs a 2D vector of strings from a table file for easier manipulation
vector<vector<string>> reconstructTableData(string tableName)
{
    vector<vector<string>> tableData;
    vector<string> tableFields = splitTableFirstField(getTableData(tableName)[0]);
    tableData.push_back(tableFields);

    vector<string> tableDataSplit;
    for(int i = 1; i < getTableData(tableName).size(); i++)
    {
        tableDataSplit = splitTableData(getTableData(tableName)[i]);
        tableData.push_back(tableDataSplit);
    }

    // Remove any empty rows
    for(int i = 0; i < tableData.size(); i++)
    {
        if(tableData[i].size() == 0)
        {
            tableData.erase(tableData.begin() + i);
        }
    }

    return tableData;
}

// Writes input table data to a new table file, overwriting the old one
void writeTableData(string tableName, vector<vector<string>> tableData, string originalTableFields)
{
    // Delete old table file
    remove((currentDatabase + "/" + tableName).c_str());

    // Create new table file
    ofstream outfile(currentDatabase + "/" + tableName);
    outfile << originalTableFields << endl;
    for(int i = 1; i < tableData.size(); i++)
    {
        for(int j = 0; j < tableData[i].size(); j++)
        {
            // Skip any empty rows
            if(tableData[i].size() == 0)
            {
                continue;
            }
            // Skip any empty columns
            if(tableData[i][j] == "" || tableData[i][j] == " " || tableData[i][j].size() == 0)
            {
                continue;
            }
            outfile << tableData[i][j] << ", ";
        }
        // Remove trailing comma
        outfile.seekp(-2, ios_base::end);
        if(i != tableData.size()-1)
        {
            outfile << endl;
        }
    }

    outfile.close();
}

// Returns the index of a matched field in a table
int getTableFieldIndex(vector<string> fields, string fieldName)
{
    for(int i = 0; i < fields.size(); i++)
    {
        if(fields[i] == fieldName)
        {
            return i;
        }
    }
    return -1;
}

// Matches a join statement with the correct function to handle it
void joinSelectData(vector<string> inputLineSplit)
{
    vector<string> tableNames;
    vector<string> tableAlias;
    vector<string> joinConditions;

    if(inputLineSplit[7] == "WHERE") // Case 1
    {
        tableNames.push_back(removeAllCommas(inputLineSplit[3]));
        tableNames.push_back(removeAllCommas(inputLineSplit[5]));
        tableAlias.push_back(removeAllCommas(inputLineSplit[4]));
        tableAlias.push_back(removeAllCommas(inputLineSplit[6]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[8]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[9]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[10]));

        innerJoinSelectData(tableNames, tableAlias, joinConditions);
    }
    else if(inputLineSplit[5] == "INNER") // Case 2
    {
        tableNames.push_back(removeAllCommas(inputLineSplit[3]));
        tableNames.push_back(removeAllCommas(inputLineSplit[7]));
        tableAlias.push_back(removeAllCommas(inputLineSplit[4]));
        tableAlias.push_back(removeAllCommas(inputLineSplit[8]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[10]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[11]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[12]));

        innerJoinSelectData(tableNames, tableAlias, joinConditions);
    }
    else if(inputLineSplit[5] == "LEFT") // Case 3
    {
        tableNames.push_back(removeAllCommas(inputLineSplit[3]));
        tableNames.push_back(removeAllCommas(inputLineSplit[8]));
        tableAlias.push_back(removeAllCommas(inputLineSplit[4]));
        tableAlias.push_back(removeAllCommas(inputLineSplit[9]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[11]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[12]));
        joinConditions.push_back(removeAllCommas(inputLineSplit[13]));

        outerJoinSelectData(tableNames, tableAlias, joinConditions);
    }
}

// Handles inner joins
void innerJoinSelectData(vector<string> tableNames, vector<string> tableAlias, vector<string> joinConditions)
{
    // Gets the field names and data from table 1
    ifstream infile1(currentDatabase + "/" + tableNames[0]);
    string originalFieldLine1;
    getline(infile1, originalFieldLine1);
    infile1.close();
    vector<string> tableFields1 = splitTableFirstField(originalFieldLine1);
    vector<vector<string>> tableData1 = reconstructTableData(tableNames[0]);

    // Gets the field names and data from table 2
    ifstream infile2(currentDatabase + "/" + tableNames[1]);
    string originalFieldLine2;
    getline(infile2, originalFieldLine2);
    infile2.close();
    vector<string> tableFields2 = splitTableFirstField(originalFieldLine2);
    vector<vector<string>> tableData2 = reconstructTableData(tableNames[1]);

    // Gets the join conditions
    string joinCondition1 = joinConditions[0];
    string joinCondition2 = joinConditions[2];

    // Further break up the join conditions on "." into field name and table alias
    vector<string> joinConditions1Alias = splitStringByDelimiter(joinCondition1, '.');
    vector<string> joinConditions2Alias = splitStringByDelimiter(joinCondition2, '.');

    // Get the field name from the join condition
    string joinConditions1FieldName = joinConditions1Alias[1];
    string joinConditions2FieldName = joinConditions2Alias[1];

    // Get the index of the join condition field in each table
    int joinConditionIndex1 = getTableFieldIndex(tableFields1, joinConditions1FieldName);
    int joinConditionIndex2 = getTableFieldIndex(tableFields2, joinConditions2FieldName);

    // Print the field names
    cout << originalFieldLine1 << ", " << originalFieldLine2 << endl;

    // Print the data from each table where the join conditions match
    for(int i = 1; i < tableData1.size(); i++)
    {
        for(int j = 1; j < tableData2.size(); j++)
        {
            if(tableData1[i][joinConditionIndex1] == tableData2[j][joinConditionIndex2])
            {
                for(int k = 0; k < tableData1[i].size(); k++)
                {
                    cout << tableData1[i][k] << ", ";
                }
                for(int k = 0; k < tableData2[j].size(); k++)
                {
                    cout << tableData2[j][k] << ", ";
                }
                cout << endl;
            }
        }
    }
}

// Handles outer joins
void outerJoinSelectData(vector<string> tableNames, vector<string> tableAlias, vector<string> joinConditions)
{
    // Get the table data for table 1
    ifstream infile1(currentDatabase + "/" + tableNames[0]);
    string originalFieldLine1;
    getline(infile1, originalFieldLine1);
    infile1.close();
    vector<string> tableFields1 = splitTableFirstField(originalFieldLine1);
    vector<vector<string>> tableData1 = reconstructTableData(tableNames[0]);

    // Get the table data for table 2
    ifstream infile2(currentDatabase + "/" + tableNames[1]);
    string originalFieldLine2;
    getline(infile2, originalFieldLine2);
    infile2.close();
    vector<string> tableFields2 = splitTableFirstField(originalFieldLine2);
    vector<vector<string>> tableData2 = reconstructTableData(tableNames[1]);

    // Break up the join conditions into respective table name and field name
    string joinCondition1 = joinConditions[0];
    string joinCondition2 = joinConditions[2];
    // Further break up the join conditions on "." into field name
    vector<string> joinConditions1Alias = splitStringByDelimiter(joinCondition1, '.');
    vector<string> joinConditions2Alias = splitStringByDelimiter(joinCondition2, '.');

    // Get the field name from the join condition
    string joinConditions1FieldName = joinConditions1Alias[1];
    string joinConditions2FieldName = joinConditions2Alias[1];

    // Get the index of the join condition field in each table
    int joinConditionIndex1 = getTableFieldIndex(tableFields1, joinConditions1FieldName);
    int joinConditionIndex2 = getTableFieldIndex(tableFields2, joinConditions2FieldName);

    // Prints the field names for table 1 and table 2
    cout << originalFieldLine1 << ", " << originalFieldLine2 << endl;

    // If there is no match in table 2, print out the data for table 1
    // If there is a match in table 2, print out the data for table 1 and table 2
    for(int i = 1; i < tableData1.size(); i++)
    {
        bool foundMatch = false;
        for(int j = 1; j < tableData2.size(); j++)
        {
            if(tableData1[i][joinConditionIndex1] == tableData2[j][joinConditionIndex2])
            {
                foundMatch = true;
                for(int k = 0; k < tableData1[i].size(); k++)
                {
                    cout << tableData1[i][k] << ", ";
                }
                for(int k = 0; k < tableData2[j].size(); k++)
                {
                    cout << tableData2[j][k] << ", ";
                }
                cout << endl;
            }
        }
        if(!foundMatch)
        {
            for(int k = 0; k < tableData1[i].size(); k++)
            {
                cout << tableData1[i][k] << ", ";
            }
            for(int k = 0; k < tableData2[0].size(); k++)
            {
                cout << "";
            }
            cout << endl;
        }
    }
}

// Helper functions

// Checks if a valid database is in use
bool checkIfUsingDatabase()
{
    if(currentDatabase == "")
    {
        cout << "!Failed to execute command. No database in use." << endl;
        return false;
    }
    else if(!filesystem::exists(currentDatabase))
    {
        cout << "!Failed to execute command. Database " << currentDatabase << " does not exist." << endl;
        return false;
    }
    else
    {
        return true;
    }
}

// Removes trailing semicolons from a vector of strings
vector<string> removeTrailingSemicolon(vector<string> strVector)
{
    vector<string> noSemicolonVector;
    for(string str : strVector)
    {
        if(str.back() == ';')
        {
            str.pop_back();
        }
        noSemicolonVector.push_back(str);
    }

    return noSemicolonVector;
}

// Capitalizes all strings in a vector
vector<string> capitalizeStringVector(vector<string> strVector)
{
    vector<string> capitalizedVector;
    for(string str : strVector)
    {
        capitalizedVector.push_back(capitalizeString(str));
    }

    return capitalizedVector;
}

// Capitalizes all characters in an input string
string capitalizeString(string str)
{
    string capitalizedString = str;
    for(int i = 0; i < str.length(); i++)
    {
        capitalizedString[i] = toupper(str[i]);
    }

    return capitalizedString;
}

// Splits the input string apart by whitespace into a vector of strings
// This is borrowed from a project in an earlier class where in turn it was 'borrowed' from StackOverflow
// Don't ask me to explain how or why this works. I just know that it does
vector<string> splitStringByWhiteSpace(string str)
{
    vector<string> stringSplit;
    istringstream buffer(str);

    for(string spaced; buffer >> spaced;)
    {
        stringSplit.push_back(spaced);
    }

    return stringSplit;
}

// Wrapper function to remove trailing and leading invisible characters from an input string
string removeTrailingAndLeadingInvisibleCharacters(string str)
{
    str = removeTrailingInvisibleCharacters(str);
    str = removeLeadingInvisibleCharacters(str);
    return str;
}

// Removes trailing invisible characters from a string
string removeTrailingInvisibleCharacters(string str)
{
    if(str.back() == '\r')
    {
        str.pop_back();
    }
    else if(str.back() == '\n')
    {
        str.pop_back();
    }
    else if(str.back() == '\t')
    {
        str.pop_back();
    }
    else if(str.back() == '\v')
    {
        str.pop_back();
    }
    else if(str.back() == '\f')
    {
        str.pop_back();
    }
    else if(str.back() == '\a')
    {
        str.pop_back();
    }
    else if(str.back() == '\b')
    {
        str.pop_back();
    }
    else if(str.back() == '\0')
    {
        str.pop_back();
    }
    else if(str.back() == '\e')
    {
        str.pop_back();
    }
    else if(str.back() == '\?')
    {
        str.pop_back();
    }

    return str;
}

// Removes leading invisible characters from a string
string removeLeadingInvisibleCharacters(string str)
{
    if(str.front() == '\r')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\n')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\t')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\v')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\f')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\a')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\b')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\0')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\e')
    {
        str.erase(str.begin());
    }
    else if(str.front() == '\?')
    {
        str.erase(str.begin());
    }

    return str;
}

// Removes trailing and leading parenthesis from an input string if they exist
string removeTrailingAndLeadingParenthesis(string str)
{
    if(str.front() == '(' && str.back() == ')')
    {
        str.erase(str.begin());
        str.pop_back();
    }

    return str;
}

// Removes all quotes from an input string
string removeAllQuotes(string str)
{
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i] == '"' || str[i] == '\'')
        {
            str.erase(str.begin() + i);
        }
    }

    return str;
}

// Removes all commas from an input string
string removeAllCommas(string str)
{
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i] == ',')
        {
            str.erase(str.begin() + i);
        }
    }

    return str;
}

// Splits an input string apart by a delimiter into a vector of strings
vector<string> splitStringByDelimiter(string str, char delimiter)
{
    vector<string> splitString;
    string tempString = "";
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i] == delimiter)
        {
            splitString.push_back(tempString);
            tempString = "";
        }
        else
        {
            tempString += str[i];
        }
    }
    splitString.push_back(tempString);

    return splitString;
}
