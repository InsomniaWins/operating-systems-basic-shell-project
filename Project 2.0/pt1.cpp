// compile command:
//    g++ "Project 2.0/pt1.cpp" -lpthread -o "part1"
// run command:
//    ./"part1"


// external headers/includes
#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>

// custom headers/includes
#include "minishell.h"

// namespace for ease of use of standard library
using namespace std;


// parses the path variable for absolute directories of commands/programs
vector<string> parsePath();

// looks through path environment variables for inputted command name
// returns empty string if no command path is found
string lookupPath(string, vector<string>);

// parses command into a commandType structure
int parseCommand(string, struct commandType *);

// print the input prompt
void printPrompt();


int main(){

    //Shell initialization
    
    // get vector of all path directories
    vector<string> paths = parsePath();
    /*    uncomment to print all dirs in PATH
    for (int i = 0; i < parsedPaths.size(); i++) {
        cout << '"' << parsedPaths[i] << '"' << '\n';
    }
    */

    // start shell main loop
    while (true) {
        
        // print user input prompt
        printPrompt();

        // get user input string
        string inputCommand;
        getline(cin, inputCommand);
        
        // parse input into command struct
        commandType command;
        parseCommand(inputCommand, &command);
        /*    uncomment to print all command arguments
        for (int i = 0; i < command.arguments.size(); i++) {
            cout << '"' << command.arguments[i] << '"' << '\n';
        }*/

        // if command is quit command (no need for new process/thread)
        // TODO: make quit command NOT case sensitive
        if (command.arguments[0] == "quit") {
            // exit main loop and finish shell process
            break;
        }


        // get path of command
        string commandPath = lookupPath(command.arguments[0], paths);

        // ask user for input again if path of command could not be found
        if (commandPath.empty()) {
            cout << "command: " << command.arguments[0] << " is not a recognized command!\n";
            break;
        }
        
        cout << "commandPath: " << commandPath << "\n";

        int pid = fork();
        if (pid == 0) {
            // run command in child
            // TODO: execute command
            cout << "Hello from pid: " << getpid() << "\n";
            exit(EXIT_SUCCESS);
        } else {
            // wait for child to finish executing command
            waitpid(pid, NULL, 0);
        }
    }

    //Shell termination

    cout << "finished shell program\n";
}


// TODO: test if function works with absolute paths, might not
// returns directory of command inputted
// searches through given array of directories for command directory to return
// if none match, return empty string
string lookupPath(string commandName, vector<string> paths){
    
    // param descriptions:
    //   commandName - name of command inputted
    //   directories - array of directories to check if command (i.e. "gcc") is inside

    // if first character of command begins with '/' character
    // then the command is an absolute path
    if (commandName[0] == '/') {
        // if the directory exists, return the directory
        if (access(commandName.c_str(), R_OK) == 0) {
            // returns a copy of commandName to avoid accidental tampering with string commandName
            return string(commandName);
        } else { // else, return empty
            return string();
        }
    }


    // past this point must mean that the command inputted is not an absolute path


    // loop through path variables
    for (int i = 0; i < paths.size(); i++) {
        // if path variable matched the inputted command name, return path variable directory

        string currentAbsoluteDirectory = paths[i] + '/' + commandName;

        // uncomment to check the directories that were checked
        cout << "[lookupPath] checking directory:  " << currentAbsoluteDirectory << "\n";

        if (access(currentAbsoluteDirectory.c_str(), R_OK) == 0) {
            // command exists in directory, return new string of current directory
            return string(paths[i]);
        }

    }


    // command is not an absolute directory
    // command could not be found in PATH
    // so, the command could not be found
    // therefore, print error and return empty
    return string();
}



// prints a prompt for user to enter a command
void printPrompt(){
    cout << "Enter you command: ";
}

// gets and parses the path string by using ':' as a delemiter/splitter
// then returns a vector of all directories in path string
vector<string> parsePath() {

    // init directories vector
    vector<string> directories;

    // get path string
    string pathString = getenv("PATH");

    // parse
    int copyPosition = 0; // index of character to start substr method
    for (int currentCharacterIndex = 0; currentCharacterIndex < pathString.length(); currentCharacterIndex++) {
        // if delimeter char
        if (pathString[currentCharacterIndex] == ':') {
            // get directory as string
            int directoryLength = currentCharacterIndex - copyPosition;
            string directoryString = pathString.substr(copyPosition, directoryLength);
            
            // add to return vector
            directories.push_back(directoryString);

            // new substr position to after the ':' character
            copyPosition = currentCharacterIndex+1;
        }
    }

    return directories;
    //error check 
}


// parses command input string into a commandType structure
int parseCommand(string commandString, struct commandType * commandStruct) {

    // param descriptions:
    //   commandString - the whole line of text inputted to the terminal
    //   commandStruct - pointer to struct that will be filled with command info

    // TODO: command probably needs to trim extra whitespaces from end of command
    // to prevent additional args like ["arg1","arg2", ... ,"",""]

    // parse
    int copyPosition = 0; // index of character to start substr method
    for (int currentCharacterIndex = 0; currentCharacterIndex < commandString.length(); currentCharacterIndex++) {
        
        // check if current character is whitespace
        bool charIsWhitespace = false;
        for (int i = 0; i < sizeof(WHITESPACE); i++) {
            if (WHITESPACE[i] == commandString[currentCharacterIndex]) {
                charIsWhitespace = true;
                break;
            }
        }

        // if current character is whitespace, then the next arg is found
        if (charIsWhitespace) {
            // get arg as string
            int argLength = currentCharacterIndex - copyPosition;
            string argString = commandString.substr(copyPosition, argLength);
            
            // add arg to struct
            commandStruct->arguments.push_back(argString);

            // new substr position to after the whitespace
            copyPosition = currentCharacterIndex+1;
        } else if (currentCharacterIndex+1 == commandString.length()) {
            // end of commandString reached

            int argLength = commandString.length() - copyPosition;
            string argString = commandString.substr(copyPosition, argLength);

            // add final arg to struct
            commandStruct->arguments.push_back(argString);

            // dont need to update copyPosition, because end of string occured
        }
    }
    
    return 0;
}